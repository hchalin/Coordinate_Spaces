#pragma once
// Include Apple's framework headers for Metal and related APIs
#include <Metal/Metal.hpp>
#include <simd/simd.h>

#include "window.h"
#include "./Primitive/primitive.h"


#include <iostream>

class Renderer
{
public:
  explicit Renderer(Window &window);
  ~Renderer();

  // Getter
  MTL::Device *getDevice();

  // Render method
  void render();

private:
  void logFPS();
  MTL::Device *device;
  MTL::CommandQueue *commandQueue;
  Window &window;

  // Scene objects
  //Triangle *triangle;
  //Quad* quad;

  // New objs using abc
  Primitive* triangle1;    // Base ptr
  Primitive* triangle2;    // Base ptr
  Primitive* quad1;
  Primitive* quad2;

  std::chrono::high_resolution_clock::time_point previousTime;
  double totalTime;
  int lastPrintedSecond;
  int frames;
};
