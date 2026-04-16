#include <metal_stdlib>

struct PushConstants {
    metal::float4x4 proj;
};

struct VertexIn {
    metal::float2 aPos   [[attribute(0)]];
    metal::float2 aUv    [[attribute(1)]];
    metal::float4 aColor [[attribute(2)]];
    metal::float4 aSize  [[attribute(3)]];
    metal::float4 aRadii [[attribute(4)]];
};

struct Varyings {
    metal::float4 position [[position]];
    metal::float2 vUv;
    metal::float4 vColor;
    metal::float4 vSize;
    metal::float4 vRadii;
};

vertex Varyings vertexMain(VertexIn in [[stage_in]], constant PushConstants& pushConstants [[buffer(0)]]) {
    Varyings out;
    out.position = pushConstants.proj * metal::float4(in.aPos, 0.0, 1.0);
    out.vUv = in.aUv;
    out.vColor = in.aColor;
    out.vSize = in.aSize;
    out.vRadii = in.aRadii;
    return out;
}

fragment metal::float4 fragmentMain(Varyings in [[stage_in]]) {
    metal::float2 pixelPos = (in.vUv - 0.5) * in.vSize.xy;
    
    float r;
    if (pixelPos.x < 0.0 && pixelPos.y < 0.0)      r = in.vRadii.x; // Top-Left
    else if (pixelPos.x > 0.0 && pixelPos.y < 0.0) r = in.vRadii.y; // Top-Right
    else if (pixelPos.x > 0.0 && pixelPos.y > 0.0) r = in.vRadii.z; // Bottom-Right
    else                                           r = in.vRadii.w; // Bottom-Left
    
    metal::float2 halfSize = in.vSize.xy * 0.5;
    metal::float2 q = metal::abs(pixelPos) - halfSize + r;
    float dist = metal::length(metal::max(q, 0.0)) + metal::min(metal::max(q.x, q.y), 0.0) - r;
    
    float alpha = 1.0 - metal::smoothstep(0.0, 1.0, dist);

    metal::float4 finalColor = in.vColor;
    finalColor.a *= alpha;

    return finalColor;
}