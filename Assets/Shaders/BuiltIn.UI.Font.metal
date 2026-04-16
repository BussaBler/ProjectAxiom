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

struct FontResource {
    metal::texture2d<float> fontTexture [[id(0)]];
    metal::sampler fontSampler [[id(1)]];
};

vertex Varyings vertexMain(VertexIn in [[stage_in]], constant PushConstants& pushConstants [[buffer(0)]]) {
    Varyings out;
    out.position = pushConstants.proj * metal::float4(in.aPos, 0.0, 1.0);
    out.vUv = in.aUv;
    out.vColor = in.aColor;

    return out;
}

float median(float r, float g, float b) {
    return metal::max(metal::min(r, g), metal::min(metal::max(r, g), b));
}

fragment metal::float4 fragmentMain(Varyings in [[stage_in]], constant FontResource& resources [[buffer(8)]]) {
    metal::float3 msd = resources.fontTexture.sample(resources.fontSampler, in.vUv).rgb;
    float sd = median(msd.r, msd.g, msd.b) - 0.5;

    metal::float2 texSize = metal::float2(resources.fontTexture.get_width(), resources.fontTexture.get_height());
    metal::float2 dx = metal::dfdx(in.vUv) * texSize;
    metal::float2 dy = metal::dfdy(in.vUv) * texSize;

    float toPixels = 4.0 * metal::rsqrt(0.5 * (metal::dot(dx, dx) + metal::dot(dy, dy)));
    float screenPxDistance = toPixels * sd;

    float alpha = metal::saturate(screenPxDistance + 0.5);
    return in.vColor * alpha;
}