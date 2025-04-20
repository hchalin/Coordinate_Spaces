// Prevent GLFW from including OpenGL headers (we're using Metal instead)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Enable private implementation details for Apple's frameworks
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

// Include Apple's framework headers for Metal and related APIs
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <AppKit/AppKit.hpp>

// Backend
#include "backend/glfw_adaptor.h"

#include "window.h"
#include "renderer.h"

#include <iostream>

int main(){
  // Test glfw
  if (!glfwInit())
  {
      std::cerr << "Failed to initialize GLFW window!" << std::endl;
      return -1;
  }

  try {
    Window window;
    Renderer renderer(window);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error from main: " << e.what() << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwTerminate();

  return EXIT_SUCCESS;
}
