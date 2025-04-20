cmake_minimum_required(VERSION 3.15)
project(3d-Transformations VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g")




add_executable(Transformations
        src/common/vec4.cpp
        src/Primitive/primitive.cpp
        src/shaders/readShaderFile.cpp
        src/backend/glfw_adaptor.mm
        src/window.cpp
        src/renderer.cpp
        src/main.cpp
        src/common/Transform.cpp
)

# Find GLFW
find_package(glfw3 REQUIRED)

# Include directories
target_include_directories(Transformations PRIVATE
        ${CMAKE_SOURCE_DIR}/dependencies/        # Includes Eigen (and other headers if needed)
        ${GLFW_INCLUDE_DIRS}
        SYSTEM /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks
)
# Link GLFW library
target_link_libraries(Transformations PRIVATE glfw)

target_include_directories(Transformations
  PRIVATE
  dependencies
)

find_library(METAL Metal)
find_library(FOUNDATION Foundation)
find_library(QUARTZCORE QuartzCore)
find_library(APPKIT AppKit)
#find_library(ACCELERATE Accelerate)

target_link_libraries(Transformations PRIVATE
  stdc++
  "-framework Metal"
        "-framework MetalKit"
        "-framework ModelIO"
        "-framework Foundation"
        "-framework QuartzCore"
        #"-framework Accelerate"
        "-framework AppKit" objc
)


