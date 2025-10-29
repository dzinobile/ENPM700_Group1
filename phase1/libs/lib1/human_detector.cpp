#include "human_detector.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Base must be initialized explicitly
HumanDetector::HumanDetector(const std::string& window_name)
: window_name_(window_name),
params_(Params{}) {
CV_Assert(!K_mat.empty());
CV_Assert(K_mat.type() == CV_32F && K_mat.rows == 3 && K_mat.cols == 3);
cv::namedWindow(window_name_);
}

HumanDetector::HumanDetector(const std::string& window_name,
    const Params& p)
: window_name_(window_name),
params_(p) {
CV_Assert(!K_mat.empty());
CV_Assert(K_mat.type() == CV_32F && K_mat.rows == 3 && K_mat.cols == 3);
cv::namedWindow(window_name_);
}

void HumanDetector::bindWindow() {
  cv::setMouseCallback(window_name_, &HumanDetector::MouseThunk, this);
}

void HumanDetector::setFrame(const cv::Mat& bgr) {
  CV_Assert(!bgr.empty() && bgr.channels() == 3);
  src_bgr_ = bgr.clone();
  cv::cvtColor(src_bgr_, gray_, cv::COLOR_BGR2GRAY);
  if (display_.empty() || display_.size() != src_bgr_.size()) {
    display_.create(src_bgr_.size(), src_bgr_.type());
  }
  redraw();
}

void HumanDetector::redraw() {
  if (src_bgr_.empty()) return;
  display_ = src_bgr_.clone();

  if (dragging_ || box_finalized_) {
    cv::rectangle(display_, box_, cv::Scalar(0,255,255), 2);
  }

  for (const auto& p : features_) {
    cv::circle(display_, p, 3, cv::Scalar(0,255,0), cv::FILLED, cv::LINE_AA);
  }

  if (feature_chosen_) {
    cv::circle(display_, chosen_pt_, 6, cv::Scalar(0,0,255), 2, cv::LINE_AA);
  }

  if (params_.draw_hud) {
    int y = 22;
    auto put = [&](const std::string& s) {
      cv::putText(display_, s, {10,y}, cv::FONT_HERSHEY_SIMPLEX, 0.6,
                  cv::Scalar(255,255,255), 2, cv::LINE_AA);
      y += 24;
    };
    if (mode_ == Mode::DRAW_BOX) {
      put("Step 1: Drag a rectangle (Left mouse). Release to finalize.");
    } else {
      put("Step 2: Click a feature dot to select it. Press 'r' to redo box.");
      put("(Z=fy*h/(v-cy), X=Z*(u-cx)/fx)");
    }
  }

  cv::imshow(window_name_, display_);
}

void HumanDetector::reset() {
  features_.clear();
  feature_chosen_ = false;
  dragging_ = false;
  box_finalized_ = false;
  box_ = {};
  mode_ = Mode::DRAW_BOX;
  redraw();
}

void HumanDetector::handleKey(int key) {
  if (key == 'r' || key == 'R') reset();
}

bool HumanDetector::hasChosen() const { return feature_chosen_; }
cv::Point2f HumanDetector::lastChosen() const { return chosen_pt_; }
HumanDetector::Mode HumanDetector::mode() const { return mode_; }
const cv::Mat& HumanDetector::display() const { return display_; }
const cv::Rect& HumanDetector::box() const { return box_; }
const std::vector<cv::Point2f>& HumanDetector::features() const { return features_; }
void HumanDetector::setCameraHeight(float h) { params_.camera_height_m = h; }
const cv::Matx33f& HumanDetector::K() const { return K_; }

cv::Point3f HumanDetector::pixelToGround(const cv::Point2f& uv) const {
  const float fx = static_cast<float>(K_mat.at<float>(0,0));
  const float fy = static_cast<float>(K_mat.at<float>(1,1));
  const float cx = static_cast<float>(K_mat.at<float>(0,2));
  const float cy = static_cast<float>(K_mat.at<float>(1,2));
  const float denom = (uv.y - cy);
  if (std::abs(denom) < 1e-6f) {
    throw std::runtime_error("pixelToGround: v ~= cy → singular depth");
  }
  std::cout<<fx<<fy;
  const float Z = (fy * params_.camera_height_m) / denom;
  const float X = Z * ((uv.x - cx) / fx);
  return {X, 0.0f, Z};
}

// --- Mouse plumbing ---
void HumanDetector::MouseThunk(int event, int x, int y, int flags, void* userdata) {
  auto* self = static_cast<HumanDetector*>(userdata);
  if (self) self->onMouse(event, x, y, flags);
}

void HumanDetector::onMouse(int event, int x, int y, int /*flags*/) {
  if (src_bgr_.empty()) return;

  if (mode_ == Mode::DRAW_BOX) {
    if (event == cv::EVENT_LBUTTONDOWN) {
      dragging_ = true;
      start_pt_ = {x,y};
      box_ = cv::Rect(start_pt_, start_pt_);
    } else if (event == cv::EVENT_MOUSEMOVE && dragging_) {
      box_ = cv::Rect(start_pt_, cv::Point(x,y));
      clampBoxToImage();
      redraw();
    } else if (event == cv::EVENT_LBUTTONUP && dragging_) {
      dragging_ = false;
      box_ = normalizeRect(cv::Rect(start_pt_, cv::Point(x,y)));
      clampBoxToImage();
      if (box_.width < 4 || box_.height < 4) {
        std::cout << "[warn] Box too small, try again.\n";
        box_finalized_ = false;
      } else {
        box_finalized_ = true;
        detectFeaturesInBox();
        mode_ = Mode::PICK_FEATURE;
      }
      redraw();
    }
  } else {
    if (event == cv::EVENT_LBUTTONDOWN) {
      const cv::Point2f click(static_cast<float>(x), static_cast<float>(y));
      double best_d2 = params_.choose_max_pix_dist * params_.choose_max_pix_dist;
      int best_idx = -1;
      for (int i = 0; i < static_cast<int>(features_.size()); ++i) {
        const auto& p = features_[i];
        const double dx = p.x - click.x, dy = p.y - click.y;
        const double d2 = dx*dx + dy*dy;
        if (d2 < best_d2) { best_d2 = d2; best_idx = i; }
      }
      if (best_idx < 0) {
        std::cout << "[warn] Click near a green dot to choose a feature.\n";
        return;
      }
      chosen_pt_ = features_[best_idx];
      feature_chosen_ = true;
      redraw();

      std::cout << chosen_pt_ << "\n";
      std::cout << K_mat << "\n";
      try {
        const auto Xw = pixelToGround(chosen_pt_);
        std::cout << "Value of Z is => " << Xw.z << "\n";
        std::cout << "Value of X is => " << Xw.x << "\n";
      } catch (const std::exception& e) {
        std::cout << "[error] " << e.what() << "\n";
      }
    }
  }
}

// --- Features ---
void HumanDetector::detectFeaturesInBox() {
  features_.clear();
  if (box_.width <= 1 || box_.height <= 1) return;
  CV_Assert(!gray_.empty());

  cv::Mat gray_roi = gray_(box_);
  std::vector<cv::Point2f> pts;
  cv::goodFeaturesToTrack(gray_roi, pts,
                          params_.max_corners,
                          params_.quality_level,
                          params_.min_distance,
                          cv::noArray(),
                          params_.block_size,
                          params_.use_harris);

  features_.reserve(pts.size());
  for (const auto& p : pts) {
    features_.emplace_back(p.x + static_cast<float>(box_.x),
                           p.y + static_cast<float>(box_.y));
  }

  if (features_.empty()) {
    std::cout << "[warn] No features found in ROI.\n";
  } else {
    std::cout << "[info] Found " << features_.size() << " features in ROI.\n";
  }
}

// --- Utils ---
void HumanDetector::clampBoxToImage() {
  const cv::Rect canvas(0, 0, src_bgr_.cols, src_bgr_.rows);
  box_ &= canvas;
}

cv::Rect HumanDetector::normalizeRect(const cv::Rect& r) {
  const int x1 = std::min(r.x, r.br().x);
  const int y1 = std::min(r.y, r.br().y);
  const int x2 = std::max(r.x, r.br().x);
  const int y2 = std::max(r.y, r.br().y);
  return cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
}
