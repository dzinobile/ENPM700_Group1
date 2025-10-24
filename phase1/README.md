# Group 1 TDD Exercise
## C++ Badges
[![CICD Workflow status](https://github.com/dzinobile/ENPM700_TDD_group1/actions/workflows/run-unit-test-and-upload-codecov.yml/badge.svg)](https://github.com/dzinobile/ENPM700_TDD_group1/actions/workflows/run-unit-test-and-upload-codecov.yml)


[![codecov](https://codecov.io/gh/dzinobile/ENPM700_TDD_group1/graph/badge.svg?token=78SFfF5a5p)](https://codecov.io/gh/dzinobile/ENPM700_TDD_group1)



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

