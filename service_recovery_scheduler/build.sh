#!/usr/bin/env bash
#
# Build, run, and test the service_recovery_scheduler project.
#
# Usage:
#   ./build.sh [command]
#
# Commands:
#   build       Configure and build the project (default)
#   run         Build, then run the application
#   run-notest  Build WITHOUT unit tests, then run the application
#   test        Build, then run the unit tests
#   all         Build, run the application, then run the unit tests
#   coverage    Build with coverage, run tests, generate a coverage report
#   analyze     Run static analysis on the source code
#   clean       Remove the build directories
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
COVERAGE_DIR="${SCRIPT_DIR}/build-coverage"
APP_NAME="service_recovery_scheduler"

configure_and_build() {
    cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" "$@"
    cmake --build "${BUILD_DIR}"
}

run_app() {
    "${BUILD_DIR}/${APP_NAME}"
}

run_tests() {
    ctest --test-dir "${BUILD_DIR}" --output-on-failure
}

clean() {
    rm -rf "${BUILD_DIR}" "${COVERAGE_DIR}"
    echo "Removed ${BUILD_DIR} and ${COVERAGE_DIR}"
}

run_coverage() {
    if ! command -v gcovr >/dev/null 2>&1; then
        echo "Error: gcovr is not installed (try: pip install gcovr)." >&2
        exit 1
    fi

    rm -rf "${COVERAGE_DIR}"
    cmake -S "${SCRIPT_DIR}" -B "${COVERAGE_DIR}" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_CXX_FLAGS="--coverage -O0 -g" \
        -DCMAKE_EXE_LINKER_FLAGS="--coverage"
    cmake --build "${COVERAGE_DIR}"
    ctest --test-dir "${COVERAGE_DIR}" --output-on-failure

    local html_dir="${COVERAGE_DIR}/coverage-html"
    mkdir -p "${html_dir}"

    echo
    echo "=================== Coverage Summary ==================="
    gcovr --root "${SCRIPT_DIR}" \
        --filter 'src/' --filter 'include/' --exclude 'src/main.cpp' --txt
    gcovr --root "${SCRIPT_DIR}" \
        --filter 'src/' --filter 'include/' --exclude 'src/main.cpp' \
        --html-details -o "${html_dir}/index.html"
    echo "======================================================="
    echo "HTML report: ${html_dir}/index.html"
}

run_analyze() {
    local includes=("-I${SCRIPT_DIR}/include")
    local sources=("${SCRIPT_DIR}"/src/*.cpp)

    if command -v cppcheck >/dev/null 2>&1; then
        echo "Running cppcheck..."
        cppcheck --enable=warning,style,performance,portability \
            --std=c++17 --inline-suppr --error-exitcode=1 \
            "-I${SCRIPT_DIR}/include" "${SCRIPT_DIR}/src"
    elif command -v clang-tidy >/dev/null 2>&1; then
        echo "Running clang-tidy..."
        clang-tidy "${sources[@]}" -- -std=c++17 "${includes[@]}"
    else
        echo "cppcheck/clang-tidy not found; falling back to GCC -fanalyzer..."
        g++ -std=c++17 -Wall -Wextra -fanalyzer -fsyntax-only \
            "${includes[@]}" "${sources[@]}"
        echo "Static analysis (GCC -fanalyzer) completed with no blocking errors."
    fi
}

cmd="${1:-build}"

case "${cmd}" in
    build)
        configure_and_build
        ;;
    run)
        configure_and_build
        run_app
        ;;
    run-notest)
        configure_and_build -DBUILD_TESTING=OFF
        run_app
        ;;
    test)
        configure_and_build
        run_tests
        ;;
    all)
        configure_and_build
        run_app
        run_tests
        ;;
    coverage)
        run_coverage
        ;;
    analyze)
        run_analyze
        ;;
    clean)
        clean
        ;;
    *)
        echo "Unknown command: ${cmd}" >&2
        echo "Usage: $0 [build|run|run-notest|test|all|coverage|analyze|clean]" >&2
        exit 1
        ;;
esac
