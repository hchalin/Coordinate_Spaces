#include "primitive.h"
#include "../shaders/readShaderFile.h"

/*
-------------------------------------------------------------------
  PRIMATIVE  ---------------------------------------------------------

  Currently there are 2 types:
    Triangle,
    Quad
-------------------------------------------------------------------
*/
Primitive::Primitive(MTL::Device *device) : device(device)
{
    // Transform
    //transform = Transform();
}

/*
    Destructor
*/
Primitive::~Primitive()
{

  if (vertexBuffer)
  {
    vertexBuffer->release();
    vertexBuffer = nullptr;
  }

  if (pipelineState)
  {
    pipelineState->release();
    pipelineState = nullptr;
  }
}

/*
    CREATE VERTEX BUFFER
*/
void Primitive::createVertexBuffer(const std::vector<float4> &vertices)
{
  if (vertices.empty())
    throw std::runtime_error("No vertices defined");

  vertexBuffer = device->newBuffer(vertices.data(), vertices.size() * sizeof(float4), MTL::ResourceStorageModeManaged);

  if (!vertexBuffer)
    throw std::runtime_error("Failed to create vertex buffer");

}

/*
    CREATE COLOR BUFFER
*/
void Primitive::createColorBuffer(const std::vector<float4> &color)
{
  if (color.empty())
    throw std::runtime_error("No color defined");

  colorBuffer = device->newBuffer(color.data(), color.size() * sizeof(float4), MTL::ResourceStorageModeManaged);

  if (!colorBuffer)
    throw std::runtime_error("Failed to create vertex buffer");
}

/*
    CREATE INDEX BUFFER
*/
void Primitive::createIndexBuffer(const std::vector<uint16_t> &indices)
{
  indexBuffer = device->newBuffer(indices.data(), indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeManaged);
  if (indexBuffer)
    std::cout << "Index buffer created" << std::endl;
}

/*
  CREATE RENDER PIPELINE STATE
*/
void Primitive::createRenderPipelineState()
{
  // Uses helper function to load the shaders - using pre-compiled shaders is another approach
  const std::string fileName = "shaders.metal";
  NS::Error *error{nullptr}; // Used to catch errors when creating the library
  MTL::Library *library{nullptr};

  try
  {
    loadShaderFromFile(library, device, fileName);
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error loading shader: " << e.what() << std::endl;
  }

  // Create library
  if (!library)
    throw std::runtime_error("Failed to create triangle shader library");

  // Get both vertex and fragment functions
  MTL::Function *vertexFunction = library->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
  if (!vertexFunction)
  {
    std::cerr << "Vertex function not found" << std::endl;
    return;
  }

  MTL::Function *fragmentFunction = library->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
  if (!fragmentFunction)
  {
    std::cerr << "Fragment function not found" << std::endl;
    return;
  }

  // Create render pipeline descriptor
  MTL::RenderPipelineDescriptor *pipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
  // Set the functions
  pipelineDescriptor->setVertexFunction(vertexFunction);
  pipelineDescriptor->setFragmentFunction(fragmentFunction);

  // Configure color attachment - NOTE: The color attachment represents the output target for the fragment shader.
  MTL::RenderPipelineColorAttachmentDescriptor *colorAttachment = pipelineDescriptor->colorAttachments()->object(0);
  colorAttachment->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
  colorAttachment->setBlendingEnabled(false); // This overwrites the entire color buffer, keep this in mind when rendering fog etc...

  /*
    OLD - no longer need the vertex descriptor
        For anything more complicated than a primitive. Use a vertex descriptor.

  // Create and configure the vertex descriptor
  MTL::VertexDescriptor *vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

  // Position
  vertexDescriptor->layouts()->object(0)->setStride(sizeof(Vertex));                           // Each vertex is a float4 (x, y, z, w)
  vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat4); // float4 for position
  vertexDescriptor->attributes()->object(0)->setOffset(offsetof(Vertex, position));            // Position starts at offset 0
  vertexDescriptor->attributes()->object(0)->setBufferIndex(0);                                // Use buffer index 0

  // Set color attribute
  vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat4); // float4 for position
  vertexDescriptor->attributes()->object(1)->setOffset(offsetof(Vertex, color));
  vertexDescriptor->attributes()->object(1)->setBufferIndex(0); // Use buffer index 0

  pipelineDescriptor->setVertexDescriptor(vertexDescriptor);
  */

  // Now create the renderPipelineState with the descriptor
  pipelineState = device->newRenderPipelineState(pipelineDescriptor, &error);

  if (error)
  {
    std::cerr << "ERROR: " << error->localizedDescription()->utf8String() << std::endl;
    return;
  }

  if (!pipelineState)
  {
    std::cerr << "Invalid pipelineState" << std::endl;
    return;
  }

  // clean up
  pipelineDescriptor->release();
  // vertexDescriptor->release();
  vertexFunction->release();
  fragmentFunction->release();
  library->release();
}

/*
    ENCODE RENDER COMMANDS
*/
void Primitive::encodeRenderCommands(MTL::RenderCommandEncoder *encoder) const
{

  /*
      To encode, you need:
      1: A valid encoder
      2: Complete render pipeline state
      3: Compete Vertex buffer
  */
  if (!pipelineState)
    throw std::runtime_error("No Pipeline State");
  if (!vertexBuffer)
    throw std::runtime_error("No Vertex Buffer");

  if (!encoder)
  {
    std::cerr << "Invalid encoder" << std::endl;
    return;
  }

  // encoder->setTriangleFillMode(MTL::TriangleFillMode::TriangleFillModeLines);
  //  set renderpipeline state using encoder
  encoder->setRenderPipelineState(pipelineState);

  // Set vertex buffer
  encoder->setVertexBuffer(vertexBuffer, 0, 0); // Set vertexBuffer to buffer(0)
  encoder->setVertexBuffer(colorBuffer, 0, 1);  // Set colorBuffer to buffer(1)

    /*
     *  Always send transform matrix to GPU, even if there are not transformations.
     */

    const Eigen::Matrix4f& transformMatrix = transform.getMatrix();
    encoder->setVertexBytes(transformMatrix.data(), sizeof(Eigen::Matrix4f), 11);
}

Transform &Primitive::getTransform() {
    return transform;
}

/*
-------------------------------------------------------------------
    Triangle  ---------------------------------------------------------
-------------------------------------------------------------------
*/
// Standard constructor
Triangle::Triangle(MTL::Device *device) : Primitive(device) {
    createDefaultBuffers();
    createRenderPipelineState();
}
/**
 * @brief Constructs a triangle with custom vertices and colors.
 *
 * Creates a triangle primitive with user-specified vertex positions and colors.
 * The constructor automatically generates the appropriate indices (0,1,2) for the triangle
 * and creates all necessary GPU buffers and render pipeline state.
 *
 * @param device The Metal device used to create buffers and pipeline state
 * @param vertices A vector of float4 values representing the triangle's vertex positions
 * @param color A vector of float4 values representing the color of each vertex
 * @throws std::runtime_error If vertices or color vectors are empty
 * @throws std::runtime_error If buffer creation fails
 */
Triangle::Triangle(MTL::Device *device, const std::vector<float4> &vertices,
                   const std::vector<float4> &color): Primitive(device) {
    if (vertices.empty())
        throw std::runtime_error("No vertices defined");
    if (color.empty())
        throw std::runtime_error("No color defined");
    createVertexBuffer(vertices);
    createColorBuffer(color);
    // define indices
    std::vector<uint16_t> indices = {0, 1, 2};
    Primitive::createIndexBuffer(indices);

    createRenderPipelineState();
}
Triangle::~Triangle()
{

    if (indexBuffer)
    {
        indexBuffer->release();
        indexBuffer = nullptr;
    }
}
/*
      Draw ------
*/
void Triangle::draw(MTL::RenderCommandEncoder *encoder)
{


  encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                 3, // Number of indices
                                 MTL::IndexType::IndexTypeUInt16,
                                 indexBuffer,
                                 0); //
}

void Triangle::createDefaultBuffers()
{
  // Positions
  std::vector<float4> positions = {
      {0.0, 0.5, 0.0, 1.0},
      {-0.5, -0.5, 0.0, 1.0},
      {0.5, -0.5, 0.0, 1.0}};
  Primitive::createVertexBuffer(positions);

  // Colors
  std::vector<float4> color = {
      {0.5, 0.5, 0.5, 1.0}, // Gray color
      {0.5, 0.5, 0.5, 1.0}, // Gray color
      {0.5, 0.5, 0.5, 1.0}}; // Gray color
  Primitive::createColorBuffer(color);

  // Indexing
  std::vector<uint16_t> indices = {0, 1, 2};
  Primitive::createIndexBuffer(indices);

}

/*
-------------------------------------------------------------------
    Quad  ---------------------------------------------------------
-------------------------------------------------------------------
*/

/**
 * @brief Constructs a Quad primitive with default buffers.
 *
 * This constructor initializes a Quad object with default vertex, color, and index buffers.
 * It also creates the render pipeline state required for rendering the Quad.
 *
 * @param device The Metal device used to create buffers and pipeline state.
 */
Quad::Quad(MTL::Device *device) : Primitive(device), indexBuffer(nullptr)
{
    // default
  createDefaultBuffers();
  Primitive::createRenderPipelineState();
}

/**
 * @brief Constructs a Quad with custom vertices and colors.
 *
 * This constructor initializes a Quad object with user-defined vertex positions and colors.
 * It creates the necessary GPU buffers (vertex, color, and index buffers) and sets up the render pipeline state.
 *
 * @param device The Metal device used to create buffers and pipeline state.
 * @param vertices A vector of float4 values representing the positions of the quad's vertices.
 * @param color A vector of float4 values representing the color of each vertex.
 * @throws std::runtime_error If the vertices or color vectors are empty.
 * @throws std::runtime_error If buffer creation fails.
 */
Quad::Quad(MTL::Device * device, const std::vector<float4> &vertices, const std::vector<float4> &color): Primitive(device) {
    // custom
    if (vertices.empty())
        throw std::runtime_error("No vertices defined");
    if (color.empty())
        throw std::runtime_error("No color defined");

    createVertexBuffer(vertices);

    createColorBuffer(color);
  // Indexing
  std::vector<uint16_t> indices = {
      // First tringle
      0, 2, 3,
      // Second triangle
      0, 1, 2};
  //Primative::createIndexBuffer(indices);
    indexBuffer = device->newBuffer(indices.data(), indices.size() * sizeof(float4), MTL::ResourceStorageModeManaged);
    //Primitive::createIndexBuffer(indices);        // FIXME: I cannot call this method with a quad????

    createRenderPipelineState();
}

Quad::~Quad()
{

  if (indexBuffer)
  {
    indexBuffer->release();
    indexBuffer = nullptr;
  }
}

void Quad::createDefaultBuffers()
{
  // Positions
  std::vector<float4> vertices = {
      {-0.5, 0.5, 0.0, 1.0}, // Top Left
      {0.5, 0.5, 0.0, 1.0},  // Top Right
      {0.5, -0.5, 0.0, 1.0}, // Bottom Right
      {-0.5, -0.5, 0.0, 1.0} // Bottom Left
  };
  Primitive::createVertexBuffer(vertices);
  if (!vertexBuffer)
    throw std::runtime_error("Vertex buffer failed to create");

  // Colors
  std::vector<float4> color = {
      {0.0, 0.0, 1.0, 1.0},
      {0.0, 0.0, 1.0, 1.0},
      {0.0, 0.0, 1.0, 1.0},
      {0.0, 0.0, 1.0, 1.0}};

  Primitive::createColorBuffer(color);
  if (!colorBuffer)
    throw std::runtime_error("Color buffer failed to create");

  // Indexing
  std::vector<uint16_t> indices = {
      // First tringle
      0, 2, 3,
      // Second triangle
      0, 1, 2};
  //Primative::createIndexBuffer(indices);
  indexBuffer = device->newBuffer(indices.data(), indices.size() * sizeof(float4), MTL::ResourceStorageModeManaged);
  if (!indexBuffer)
    throw std::runtime_error("Index buffer failed to create");

  // test
  std::cout << "SUCCESS in creating Quad buffers" << std::endl;
}

void Quad::draw(MTL::RenderCommandEncoder *encoder)
{

  if (!indexBuffer)
    throw std::runtime_error("Index buffer failed to create");

  // Draw the quad using the index buffer
  encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                 6, // Number of indices
                                 MTL::IndexType::IndexTypeUInt16,
                                 indexBuffer,
                                 0); //
}

//-------------------------------------------------------------------
//    Circle  ---------------------------------------------------------
//-------------------------------------------------------------------

Circle::Circle(MTL::Device *device): Primitive(device) {
    // Create the vertex buffer for the circle
    createDefaultBuffers();
    Primitive::createRenderPipelineState();
}
/*
    Destructor
*/
Circle::~Circle()
{


    if (vertexBuffer)
    {
        vertexBuffer->release();
        vertexBuffer = nullptr;
    }

    if (pipelineState)
    {
        pipelineState->release();
        pipelineState = nullptr;
    }

}

void Circle::createDefaultBuffers() {
   /*
    * Position
    */
   std::vector<float4> positions;
   unsigned int vertexCount {100};
   float angle = (2 * M_PI) / vertexCount;
   positions.emplace_back(0.0, 0.0, 0.0, 1.0);
   for(int i {1}; i <= vertexCount; ++i){
       float x = radius * cos(i * angle);
       float y = radius * sin(i * angle);
       positions.emplace_back(x, y, 0.0, 1.0);
   }

   Primitive::createVertexBuffer(positions);
   if (!vertexBuffer) {
       throw std::runtime_error("Failed to create circles vertexBuffer");
   }

   /*
    * Color
    */
   std::vector<float4> color;
   for (int i {0}; i <= vertexCount; ++i){
       color.emplace_back(0.4, 0.2, 0.3, 1.0);
   }
   Primitive::createColorBuffer(color);
    if (!colorBuffer)
        throw std::runtime_error("Color buffer failed to create");

    /*
     * Indices
     */
    std::vector<uint16_t> indices;
    const uint16_t centerIdx = 0;
    for (uint16_t i = 1; i <= vertexCount; ++i) {
        // Create two triangles for each segment of the circle
        indices.push_back(centerIdx); // Center vertex
        indices.push_back(i);
        if (i == vertexCount) {
            indices.push_back(1); // Connect back to the first vertex
        } else {
            indices.push_back(i + 1);
        }
    }

    // Add the last triangle to close the circle
    for (uint16_t i = 1; i < vertexCount; ++i) {
        indices.push_back(0); // Center vertex
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    indexBuffer = device->newBuffer(indices.data(), indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeManaged );
    if (!indexBuffer)
        throw std::runtime_error("Index buffer failed to create");
}

void Circle::draw(MTL::RenderCommandEncoder *encoder) {
    // This is the draw call
    std::cout << "Drawing circle" << std::endl;

    encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                   indexBuffer->length() / sizeof(uint16_t), // Number of indices
                                   MTL::IndexType::IndexTypeUInt16,
                                   indexBuffer,
                                   0); //
}

