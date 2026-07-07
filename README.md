# Service Recovery Scheduler

A small C++17 application that simulates how a supervisor recovers failing
services. Each service belongs to a category (Critical, Major, Minor) and is
driven through an escalating sequence of recovery actions (Restart → Stop →
Disable) whenever it reports a failure. A global policy can reject recovery for
fatal signals such as `SIGSEGV` and `SIGILL`.

## Project Structure

```
service_recovery_scheduler/
├── CMakeLists.txt          # Top-level build (library + app + tests)
├── build.sh                # Helper script for build/run/test/coverage/analyze
├── include/                # Public headers
│   ├── Action.h
│   ├── Common.h
│   ├── Policy.h
│   ├── Scheduler.h
│   └── Service.h
├── src/                    # Implementation
│   ├── Action.cpp
│   ├── Policy.cpp
│   ├── Scheduler.cpp
│   ├── Service.cpp
│   └── main.cpp
└── tests/                  # GoogleTest unit tests
    ├── CMakeLists.txt
    ├── test_policy.cpp
    ├── test_service.cpp
    └── test_scheduler.cpp
```

The core logic is compiled into a static library (`service_recovery_lib`) so it
can be linked by both the application and the unit tests.

## Requirements

- CMake ≥ 3.16
- A C++17 compiler (g++ or clang)
- Internet access on first configure (GoogleTest is fetched automatically)
- Optional: `gcovr` for coverage; `cppcheck` or `clang-tidy` for static analysis

## Quick Start

The easiest way to work with the project is through `build.sh`:

```bash
./build.sh build     # configure and build (default)
./build.sh run       # build, then run the application
./build.sh test      # build, then run the unit tests
./build.sh clean     # remove build directories
```

If the script is not executable yet:

```bash
chmod +x build.sh
```

### All commands

| Command                | Description                                             |
| ---------------------- | ------------------------------------------------------- |
| `./build.sh build`     | Configure and build the project (default).              |
| `./build.sh run`       | Build, then run the application.                        |
| `./build.sh run-notest`| Build **without** unit tests, then run the application. |
| `./build.sh test`      | Build, then run the unit tests.                         |
| `./build.sh all`       | Build, run the application, then run the unit tests.    |
| `./build.sh coverage`  | Build with coverage, run tests, generate a report.      |
| `./build.sh analyze`   | Run static analysis on the source code.                 |
| `./build.sh clean`     | Remove `build/` and `build-coverage/`.                  |

## Build

Using the helper script:

```bash
./build.sh build
```

Or with CMake directly:

```bash
cmake -S . -B build
cmake --build build
```

The application binary is produced at `build/service_recovery_scheduler`.

To build without the unit tests (skips the GoogleTest download):

```bash
cmake -S . -B build -DBUILD_TESTING=OFF
cmake --build build
```

## Run

```bash
./build.sh run
```

Or run the binary directly after building:

```bash
./build/service_recovery_scheduler
```

## Unit Tests

The tests use [GoogleTest](https://github.com/google/googletest), fetched
automatically via CMake `FetchContent` on the first configure.

```bash
./build.sh test
```

Or with CMake/CTest directly:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Clean

```bash
./build.sh clean
```

Or remove the build directories manually:

```bash
rm -rf build build-coverage
```
