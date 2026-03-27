#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec4 vColor;

layout(binding = 0) uniform sampler2D uFontTexture;

layout(location = 0) out vec4 oColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 msd = texture(uFontTexture, vUV).rgb;
    float sd = median(msd.r, msd.g, msd.b) - 0.5;
    
    vec2 texSize = vec2(textureSize(uFontTexture, 0));
    vec2 dx = dFdx(vUV) * texSize;
    vec2 dy = dFdy(vUV) * texSize;
    
    float toPixels = 4.0 * inversesqrt(0.5 * (dot(dx, dx) + dot(dy, dy)));
    float screenPxDistance = toPixels * sd;
    
    float alpha = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    
    oColor = vec4(vColor.rgb * alpha, vColor.a * alpha);
}