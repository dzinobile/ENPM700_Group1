# Group 1 TDD Exercise
## C++ Badges
[![CICD Workflow status](https://github.com/dzinobile/ENPM700_TDD_group1/actions/workflows/run-unit-test-and-upload-codecov.yml/badge.svg)](https://github.com/dzinobile/ENPM700_TDD_group1/actions/workflows/run-unit-test-and-upload-codecov.yml)


[![codecov](https://codecov.io/gh/dzinobile/ENPM700_TDD_group1/graph/badge.svg?token=78SFfF5a5p)](https://codecov.io/gh/dzinobile/ENPM700_TDD_group1)


## 👥 Team Structure

| Part | Driver | Navigator | Description |
|------|---------|------------|--------------|
| **Part 1** | **Daniel Zinobile** | **Anvesh Som** | Create initial design and test cases |
| **Part 2** | **Dayanidhi Kandade** | **Shreya Kalyanaraman (Group Leader)** | Implement Part 1 design and confirm test cases pass |

The group leader is responsible for coordinating schedules, managing pull requests, and ensuring code integration between pairs.


## 🧠 Test-Driven Robotics Software Development

This repository is part of a **group project** focused on applying **Test-Driven Development (TDD)** principles to robotics software engineering.  
The objective is to collaboratively design, implement, and test a modular, maintainable C++ codebase for a robotic control system — following industry-grade software engineering practices such as pair programming, CI/Code Coverage, unit testing, and documentation with Doxygen.


## ✅ Key Learning Goals
- Design modular robotics software using C++17/20
- Apply **TDD principles** (write tests first, then implement)
- Practice **pair programming** and **role rotation**
- Conduct **design and code reviews** via GitHub pull requests
- Apply the **PImpl idiom** to achieve strong encapsulation
- Use **Doxygen**, **CMake**, **clang-format**, and **cppcheck** for professional-grade code quality

## UML Diagrams
### Flow
<img src="media/flow.png" width="700"> 

### Structure
<img src="media/structure.png" width="1200">


## Build and Run Files

### Prerequisites
- **C++17** toolchain (gcc/clang), **CMake >= 3.16**
- **GoogleTest** (fetched by CMake or provided by system)
- **gcovr** or **lcov** for coverage
- **cppcheck**, **clang-format**
- **Doxygen** for docs

### Clone
```bash

git clone git@github.com:dzinobile/ENPM700_TDD_group1.git
cd ENPM700_TDD_group1
```

---

### Build & Test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build

# Run unit tests
./build/test/cpp-check
```

---


### Static Analysis (cppcheck) & Formatting

```bash
# Static analysis (save to results/)
mkdir -p results
cppcheck --enable=all --inline-suppr --error-exitcode=1              --suppress=missingIncludeSystem              -I include --std=c++17 --language=c++              --project=build/compile_commands.json 2> results/cppcheck.txt

# Formatting
clang-format -i --style=Google $(find . -name *.cpp -o -name *.hpp | grep -v "/build/")
```

---

### Doxygen Docs

```bash
doxygen /build/Doxyfile.docs
make -C docs/latex/ pdf

```

## Project Structure

```text
.
├── libs/ 
│   ├── lib1/ # Library for PID
│       ├── CMakeLists.txt
│       ├── pid.cpp
│       └── pid.hpp
├── test/ # GoogleTest unit tests
│   ├── CMakeLists.txt 
│   ├── main.cpp
│   └── test.cpp
├── docs/ # UML images, design notes (kept in repo)
│   ├── html/ #HTML doxygen docs
│   └── latex/ #PNG and PDF images from doxygen
├── results/ # Static analysis & coverage artifacts (versioned as text/HTML)
├── cmake-modules/
│   └── CodeCoverage.cmake
├── media/ #UML diagrams
│   ├── structure.png
│   └── flow.png
├── scripts/
│   └── config-clangd.bash
├── CMakeLists.txt
└── README.md
```
