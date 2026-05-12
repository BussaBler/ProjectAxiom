#type vertex
#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

layout(set = 0, binding = 0) uniform GlobalData {
    mat4 uProjection;
    mat4 uView;
    vec4 uCameraPosition;

    vec4 uAmbientColor;
    vec4 uDirectionalLightDir;
    vec4 uDirectionalLightColor;
} globalData;

struct MeshInstance {
    mat4 model;
};

layout(std430, set = 1, binding = 0) readonly buffer InstanceBuffer {
    MeshInstance instances[];
};

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec2 vTexCoord;

void main() {
    MeshInstance instance = instances[gl_InstanceIndex];

    gl_Position = globalData.uProjection * globalData.uView * instance.model * vec4(aPosition, 1.0);
    vNormal = mat3(transpose(inverse(instance.model))) * aNormal;
    vTexCoord = aTexCoord;
}

#type fragment
#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vTexCoord;

layout(set = 0, binding = 0) uniform GlobalData {
    mat4 uProjection;
    mat4 uView;
    vec4 uCameraPosition;

    vec4 uAmbientColor;
    vec4 uDirectionalLightDir;
    vec4 uDirectionalLightColor;
} globalData;

layout(location = 0) out vec4 oColor;

void main() {
    vec3 abledo = vec3(1.0, 0.0, 0.0);

    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(-globalData.uDirectionalLightDir.xyz);

    float diffuse = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = diffuse * globalData.uDirectionalLightColor.xyz;

    vec3 ambientColor = globalData.uAmbientColor.xyz;

    vec3 finalColor = ambientColor + diffuseColor;

    oColor = vec4(finalColor * abledo, 1.0);
}