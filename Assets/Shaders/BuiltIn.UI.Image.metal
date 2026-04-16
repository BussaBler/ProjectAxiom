#include <metal_stdlib>

struct PushConstants {
    metal::float4x4 proj;
};

struct VertexIn {
    metal::float2 aPos [[attribute(0)]];
    metal::float2 aUv [[attribute(1)]];
    metal::float4 aColor [[attribute(2)]];
};

struct Varyings {
    metal::float4 position [[position]];
    metal::float2 vUv;
    metal::float4 vColor;
};

struct ImageResource {
    metal::texture2d<float> imageTexture [[id(0)]];
    metal::sampler imageSampler [[id(1)]];
};

vertex Varyings vertexMain(VertexIn in [[stage_in]], constant PushConstants& pushConstants [[buffer(0)]]) {
    Varyings out;
    out.position = pushConstants.proj * metal::float4(in.aPos, 0.0, 1.0);
    out.vUv = in.aUv;
    out.vColor = in.aColor;

    return out;
}

fragment metal::float4 fragmentMain(Varyings in [[stage_in]], constant ImageResource& resources [[buffer(8)]]) {
    return in.vColor * resources.imageTexture.sample(resources.imageSampler, in.vUv);
}