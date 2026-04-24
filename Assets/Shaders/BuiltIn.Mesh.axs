#type vertex
#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(push_constant, std430) uniform PushConstants {
    mat4 uProjectionView;
} pushConstants;

struct MeshInstance {
    mat4 model;
};

layout(std430, set = 0, binding = 0) readonly buffer InstanceBuffer {
    MeshInstance instances[];
};

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec2 vTexCoord;

void main() {
    MeshInstance instance = instances[gl_InstanceIndex];

    gl_Position = pushConstants.uProjectionView * instance.model * vec4(aPosition, 1.0);
    vNormal = mat3(transpose(inverse(instance.model))) * aNormal;
    vTexCoord = aTexCoord;
}

#type fragment
#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vTexCoord;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);
}