#import <MetalKit/MetalKit.h>
#import <ModelIO/ModelIO.h>
#import <Metal/Metal.h>


// Use Objective-C Metal API types (id<MTLDevice>, id<MTLBuffer>)
void createMeshBuffers(id<MTLDevice> device, id<MTLBuffer>& vertexBuffer, id<MTLBuffer>& indexBuffer) {
    // Step 1: Create an MTKMeshBufferAllocator
    MTKMeshBufferAllocator* allocator = [[MTKMeshBufferAllocator alloc] initWithDevice:device];

    // Step 2: Create an MDLMesh (sphere)
    MDLMesh* mdlMesh = [[MDLMesh alloc] initSphereWithExtent:vector_float3{0.75, 0.75, 0.75}
                                                    segments:vector_uint2{100, 100}
                                              inwardNormals:NO
                                              geometryType:MDLGeometryTypeTriangles
                                                  allocator:allocator];

    // Step 3: Convert MDLMesh to MTKMesh
    NSError* error = nil;
    MTKMesh* mtkMesh = [[MTKMesh alloc] initWithMesh:mdlMesh device:device error:&error];
    if (error) {
        NSLog(@"Error creating MTKMesh: %@", error);
        return;
    }

    // Step 4: Extract vertex and index buffers
    vertexBuffer = mtkMesh.vertexBuffers[0].buffer;
    indexBuffer = mtkMesh.submeshes[0].indexBuffer.buffer;
}
