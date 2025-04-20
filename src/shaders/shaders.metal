#include <metal_stdlib>
using namespace metal;



struct VertexOut {
    float4 position [[position]]; // Position in clip space
    float4 color;                 // Color to pass to the fragment shader
};

// TODO: decouple vertex attributes into separate buffers using vertexId
    //constant packed_float4 *positions [[buffer(0)]],
vertex VertexOut vertex_main(
    constant float4 *positions [[buffer(0)]],
    constant float4 *color [[buffer(1)]],
    constant float4x4 &matrix [[buffer(11)]],
    uint vertexID [[vertex_id]]
    ) {
    VertexOut out;
    out.position = matrix * positions[vertexID]; // Pass position to clip space
    out.color = color[vertexID];
    // Compute color based on position

    return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]) {
    return in.color; // Use the interpolated color
}
