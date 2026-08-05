# RFD 0074 prototype harness: build SlugHorn's core static library against
# the Emscripten toolchain, as a subdirectory (not standalone), so its
# CMakeLists.txt does not auto-configure the pybind11 Python bindings that
# only apply when SlugHorn is the top-level project.
#
# This mirrors how a real consumer (usd-viewer's web build) would vendor
# SlugHorn: as a subdirectory dependency, core options only.
cmake_minimum_required(VERSION 3.15)
project(slughorn_wasm_harness LANGUAGES C CXX)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/slughorn" slughorn)
