#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "backend/glfw_adaptor.h"
#include <QuartzCore/CAMetalLayer.h>
#include <Metal/Metal.hpp>

/*
  NOTE: This class does not handle per frame rendering
        When getting per frame objs, create them in the render method
        (renderPassDescritors etc..)
*/

class Window
{
public:
  Window();
  ~Window();

  // Getters
  GLFWwindow *getGLFWWindow() const;
  CA::MetalLayer *getMetalLayer() const;

private:
  GLFWwindow *glfwWindow;
  CA::MetalLayer *metalLayer;
  NS::Window *window;
};
