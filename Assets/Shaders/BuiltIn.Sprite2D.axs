#type vertex
#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUv;

layout(push_constant, std430) uniform PushConstants {
    mat4 uProjectionView;
} pushConstants;

struct SpriteInstance {
    mat4 model;
    vec4 color;
    vec4 data;
};

layout(std430, set = 0, binding = 0) readonly buffer InstanceBuffer {
    SpriteInstance instances[];
};

layout(location = 0) out vec4 vColor;
layout(location = 1) out vec2 vUv;
layout(location = 2) out flat uint vTextureSlotIndex;
layout(location = 3) out flat uint vSamplerIndex;

void main() {
    SpriteInstance instance = instances[gl_InstanceIndex];

    gl_Position = pushConstants.uProjectionView * instance.model * vec4(aPos, 0.0, 1.0);
    vColor = instance.color;
    vUv = aUv;
    vTextureSlotIndex = uint(instance.data.x);
    vSamplerIndex = uint(instance.data.y);
}

#type fragment
#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec4 vColor;
layout(location = 1) in vec2 vUv;
layout(location = 2) in flat uint vTextureSlotIndex;
layout(location = 3) in flat uint vSamplerIndex;

layout(set = 1, binding = 0) uniform texture2D uTextures[16];
layout(set = 1, binding = 1) uniform sampler uSamplers[2];

layout(location = 0) out vec4 oColor;

void main() {
    vec4 texColor = texture(sampler2D(uTextures[vTextureSlotIndex], uSamplers[vSamplerIndex]), vUv);
    oColor = vColor * texColor;
}