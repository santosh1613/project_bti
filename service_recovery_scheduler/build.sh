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
#   clean       Remove the build directory
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
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
    rm -rf "${BUILD_DIR}"
    echo "Removed ${BUILD_DIR}"
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
    clean)
        clean
        ;;
    *)
        echo "Unknown command: ${cmd}" >&2
        echo "Usage: $0 [build|run|run-notest|test|all|clean]" >&2
        exit 1
        ;;
esac
