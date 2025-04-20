# 📐 Coordinate Spaces in Metal-C++

This project is a learning sandbox where I explore **coordinate spaces** in graphics programming using **Metal-C++**.

The goal is to develop a clear, hands-on understanding of the different spaces used in a modern 3D rendering pipeline — from **object space** to **screen space** — and how to transform data between them using Metal shaders and math libraries like Eigen.

---

## 🧠 Concepts Covered

- Coordinate spaces:
  - Object (Model) Space
  - World Space
  - View (Camera) Space
  - Clip Space
  - NDC (Normalized Device Coordinates)
  - Screen Space
- Transform matrices:
  - Model matrix
  - View matrix (look-at)
  - Projection matrix (perspective/orthographic)
- Matrix composition and ordering
- Applying transforms in Metal shaders
- Sending vertex data and matrices from C++ to Metal shaders
- Visualizing coordinate transformations (e.g., axes, grids, camera frustum)

---

## 🛠 Tech Stack

- **Metal-C++**: Apple's Metal shading language in C++
- **CMake**: Cross-platform build system
- **GLFW**: Window and input management
- **Eigen**: Linear algebra library for matrix/vector math
- **CLion / VSCode**: Development environments

---

## 🚀 Goals

- Build a minimal renderer that clearly shows how objects transform through the pipeline.
- Write my own transform functions (no hidden magic).
- Learn how Metal handles coordinate systems compared to OpenGL/DirectX/Vulkan.
- Log or visualize each space as vertices are transformed through the pipeline.

---

## ✅ Progress Checklist

- [x] Setup Metal-C++ project with GLFW and CMake  
- [ ] Implement and test object → world transform  
- [ ] Implement camera/view transform  
- [ ] Implement perspective projection  
- [ ] Visualize coordinate space transitions  
- [ ] Document insights with visuals and math

---

## 🧪 How to Build

```bash
mkdir build && cd build
cmake ..
make
./CoordinateSpaces
```

