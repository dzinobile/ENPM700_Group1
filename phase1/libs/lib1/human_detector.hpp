#pragma once
#include <string>
#include <vector>
#include <opencv2/core.hpp>
#include "camera_model.hpp"

/**
 * @file human_detector.hpp
 * @brief Interface for interactive ROI selection, corner detection, and feature picking
 *        on an image stream, with pinhole back-projection to a flat ground plane.
 *
 * @details
 * The HumanDetector class provides a small UI workflow:
 *   1) User draws a rectangle (ROI) with the mouse.
 *   2) Corners (Shi–Tomasi) are detected within the ROI.
 *   3) User clicks a detected corner to select it.
 *   4) The selected pixel (u,v) can be mapped to ground coordinates (X,0,Z)
 *      assuming a pinhole camera, known intrinsics K = [fx 0 cx; 0 fy cy; 0 0 1],
 *      known camera height h above a flat ground plane, and zero tilt.
 *
 * Back-projection model (zero-tilt, flat ground):
 *   Z = fy * h / (v - cy),     X = Z * (u - cx) / fx,     Y = 0
 *
 * @note This header contains declarations only. See human_detector.cpp for definitions.
 * @see pixelToGround()
 */
class HumanDetector : public CameraModel {
public:
  /**
   * @brief UI mode for the interaction loop.
   */
  enum class Mode {
    DRAW_BOX,     ///< User drags a rectangle to define ROI.
    PICK_FEATURE  ///< User clicks a detected feature to select it.
  };

  /**
   * @brief Tunable parameters for detection, selection, and HUD.
   *
   * @note Reasonable defaults are provided for quick start.
   */
  struct Params {
    int    max_corners         = 200;   ///< Max corners to detect inside ROI (goodFeaturesToTrack).
    double quality_level       = 0.01;  ///< Minimal accepted quality of image corners (0..1).
    double min_distance        = 8.0;   ///< Minimum possible Euclidean distance between corners (px).
    int    block_size          = 3;     ///< Block size for corner detection (odd, e.g., 3,5,7).
    bool   use_harris          = false; ///< Use Harris detector instead of Shi–Tomasi if true.
    double choose_max_pix_dist = 12.0;  ///< Max click distance (px) to snap to nearest corner.
    float  camera_height_m     = 0.77f;  ///< Camera height h above ground (meters).
    bool   draw_hud            = true;  ///< Draw textual HUD instructions on the display.
  };

/**
 * @brief Construct a HumanDetector and initialize the CameraModel base.
 *
 * @param window_name     Name of the OpenCV window used for visualization.
 * @param intrinsics_path Path to intrinsics input:
 *                        - CSV file → loads K and D via CameraModel::loadFromFile()
 *                        - Video file (.mp4/.MOV) → calibrates via CameraModel::calibrateFromFile()
 *
 * @pre @p intrinsics_path must point to a readable CSV or supported video file.
 * @post The base class members (K_mat, D_mat, rvecs, tvecs) are populated.
 * @throws std::runtime_error / logs if loading or calibration fails internally.
 * @see CameraModel::loadFromFile(), CameraModel::calibrateFromFile()
 */
HumanDetector(const std::string& window_name);

/**
* @brief Construct a HumanDetector with explicit runtime parameters and
*        initialize the CameraModel base from @p intrinsics_path.
*
* @param window_name     Name of the OpenCV window used for visualization.
* @param intrinsics_path Path to intrinsics input (CSV/video as above).
* @param p               Parameter bundle controlling detection, selection radius, HUD, etc.
*
* @pre @p intrinsics_path must point to a readable CSV or supported video file.
* @post The base class members (K_mat, D_mat, rvecs, tvecs) are populated and
*       @p p is copied into the detector’s configuration.
* @throws std::runtime_error / logs if loading or calibration fails internally.
* @see Params, CameraModel::loadFromFile(), CameraModel::calibrateFromFile()
*/
HumanDetector(const std::string& window_name,
    const Params& p);


  /**
   * @brief Bind the mouse callback for @p window_name to this instance.
   *
   * @details Must be called once after construction and before interactive use.
   */
  void bindWindow();

  /**
   * @brief Set the current BGR frame and update the internal grayscale copy.
   *
   * @param bgr Input BGR frame (CV_8UC3).
   *
   * @post Triggers a redraw of overlays to the window.
   * @throws cv::Exception if @p bgr is empty or not 3-channel (assert).
   */
  void setFrame(const cv::Mat& bgr);

  /**
   * @brief Redraw overlays (ROI, corners, chosen point, HUD) and show via imshow.
   *
   * @note Safe to call frequently (e.g., in a video loop).
   */
  void redraw();

  /**
   * @brief Reset state to the initial DRAW_BOX mode.
   *
   * @post Clears ROI, detected features, and chosen point; requests a redraw.
   */
  void reset();

  /**
   * @brief Convenience key handler (e.g., press 'r' to reset).
   *
   * @param key Key code from cv::waitKey(...).
   */
  void handleKey(int key);

  /**
   * @brief Whether a feature has been chosen by the user.
   * @return true if a feature was selected; false otherwise.
   */
  bool hasChosen() const;

  /**
   * @brief Get the last chosen feature’s image coordinates.
   * @return (u,v) of the chosen feature (image pixels).
   * @pre hasChosen() must be true for the value to be meaningful.
   */
  cv::Point2f lastChosen() const;

  /**
   * @brief Current UI mode.
   * @return Mode::DRAW_BOX or Mode::PICK_FEATURE.
   */
  Mode mode() const;

  /**
   * @brief Access the last rendered display image (with overlays).
   * @return const reference to the display image.
   * @note Useful for testing or saving screenshots.
   */
  const cv::Mat& display() const;

  /**
   * @brief Access the current ROI rectangle.
   * @return const reference to the ROI (may be empty if not finalized).
   */
  const cv::Rect& box() const;

  /**
   * @brief Access the vector of detected features (image coordinates).
   * @return const reference to the detected corner list.
   */
  const std::vector<cv::Point2f>& features() const;

  /**
   * @brief Set the camera height above the ground plane.
   *
   * @param h Height in meters.
   * @post Affects subsequent pixelToGround() computations.
   */
  void setCameraHeight(float h);

  /**
   * @brief Access the intrinsic matrix.
   * @return const reference to K.
   */
  const cv::Matx33f& K() const;

  /**
   * @brief Map an image pixel (u,v) to ground coordinates (X,0,Z).
   *
   * @param uv Image point in pixels (u,v).
   * @return 3D point (X,0,Z) in the camera’s horizontal plane coordinates.
   *
   * @details Uses:
   *   Z = fy * h / (v - cy)  and  X = Z * (u - cx) / fx,  Y ≡ 0
   *   where (fx, fy, cx, cy) are taken from K, and h is the camera height.
   *
   * @throws std::runtime_error if (v - cy) ≈ 0, causing a singular depth.
   *
   * @warning Assumes zero tilt and a flat ground plane. For general poses,
   *          use a full homography or 3D ray/plane intersection with known extrinsics.
   */
  cv::Point3f pixelToGround(const cv::Point2f& uv) const;

private:
  /**
   * @brief Static trampoline that forwards to the instance mouse handler.
   *
   * @param event  OpenCV mouse event.
   * @param x      Cursor x in pixels.
   * @param y      Cursor y in pixels.
   * @param flags  Event flags (modifiers).
   * @param userdata Pointer to this HumanDetector instance.
   *
   * @note Installed via bindWindow().
   */
  static void MouseThunk(int event, int x, int y, int flags, void* userdata);

  /**
   * @brief Instance mouse handler implementing the UI state machine.
   *
   * @param event OpenCV mouse event.
   * @param x     Cursor x in pixels.
   * @param y     Cursor y in pixels.
   * @param flags Event flags (modifiers).
   */
  void onMouse(int event, int x, int y, int flags);

  /**
   * @brief Detect corners within the finalized ROI and store them in @ref features_.
   *
   * @details Uses cv::goodFeaturesToTrack on the grayscale ROI with parameters
   *          from @ref params_. Offsets ROI-local coordinates into full-image coordinates.
   */
  void detectFeaturesInBox();

  /**
   * @brief Clamp the ROI rectangle to lie within the current frame’s bounds.
   */
  void clampBoxToImage();

  /**
   * @brief Normalize a possibly negative-width/height rectangle into canonical form.
   *
   * @param r Input rectangle (possibly constructed from two arbitrary corners).
   * @return Rect with top-left <= bottom-right.
   */
  static cv::Rect normalizeRect(const cv::Rect& r);

  // ---- Configuration (intrinsics + tunables) ----
  std::string window_name_;  ///< Name of the OpenCV window for rendering.
  cv::Matx33f K_;            ///< Camera intrinsics (fx, fy, cx, cy).
  Params params_;            ///< Parameters for detection/selection/HUD.

  // ---- Runtime state (images, ROI, features, UI flags) ----
  cv::Mat src_bgr_;          ///< Latest input frame (BGR).
  cv::Mat gray_;             ///< Grayscale version of @ref src_bgr_.
  cv::Mat display_;          ///< Render target with overlays.
  cv::Rect box_;             ///< Current ROI (normalized, clamped).
  bool box_finalized_ = false;   ///< True if ROI is finalized.

  std::vector<cv::Point2f> features_; ///< Detected corners in image coords.
  bool feature_chosen_ = false;       ///< True once a feature has been selected.
  cv::Point2f chosen_pt_{};           ///< Last chosen feature (u,v).

  Mode mode_ = Mode::DRAW_BOX;        ///< Current UI mode.
  bool dragging_ = false;             ///< True while mouse drag is active.
  cv::Point start_pt_{};              ///< Drag start point (pixels).
};

/**
 * @example
 * \code{.cpp}
 * #include "human_detector.hpp"
 * #include <opencv2/opencv.hpp>
 *
 * int main() {
 *   cv::Matx33f K(800,0,640, 0,800,360, 0,0,1);
 *   HumanDetector hd("homography-pick", K);
 *   hd.bindWindow();
 *
 *   cv::VideoCapture cap(0);
 *   for (;;) {
 *     cv::Mat frame; cap >> frame;
 *     if (frame.empty()) break;
 *     hd.setFrame(frame);
 *     hd.redraw();
 *     int k = cv::waitKey(1);
 *     hd.handleKey(k);
 *     if (k == 27) break; // ESC
 *   }
 * }
 * \endcode
 */
