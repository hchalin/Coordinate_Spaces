#include "window.h"

Window::Window()
    : glfwWindow(nullptr),
      metalLayer(nullptr),
      window(nullptr)
{
  // disable GLFW's default behavior of creating an OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  //glfwWindow = glfwCreateWindow(800, 600, "Abstract Window", nullptr, nullptr);
  glfwWindow = glfwCreateWindow(600, 600, "Abstract Window", nullptr, nullptr);

  if (!glfwWindow)
    std::runtime_error("Failed to create window");

  metalLayer = CA::MetalLayer::layer()->retain();
  metalLayer->setDevice(MTL::CreateSystemDefaultDevice());
  metalLayer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
  metalLayer->setFramebufferOnly(true);

  window = get_ns_window(glfwWindow, metalLayer)->retain();
}

Window::~Window()
{
  if (metalLayer) {
    metalLayer->release();
   metalLayer = nullptr;
  }

  if (window) {
    window->release();
    window = nullptr;
  }

  glfwDestroyWindow(glfwWindow);
  glfwTerminate();
}

GLFWwindow *Window::getGLFWWindow() const
{
  if (glfwWindow)
    return glfwWindow;
  return nullptr;
}

CA::MetalLayer *Window::getMetalLayer() const
{
  if (metalLayer)
    return metalLayer;
  return nullptr;
}
