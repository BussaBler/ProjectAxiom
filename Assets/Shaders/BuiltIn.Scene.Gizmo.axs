#type vertex
#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

layout(set = 0, binding = 0) uniform GlobalData {
    mat4 uProjection;
    mat4 uView;
    vec4 uCameraPosition;

    vec4 uAmbientColor;
    vec4 uDirectionalLightDir;
    vec4 uDirectionalLightColor;
} globalData;

layout(location = 0) out vec4 vColor;

void main() {
    vColor = aColor;
    gl_Position = globalData.uProjection * globalData.uView * vec4(aPosition, 1.0);
}

#type fragment
#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec4 vColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vColor;
}