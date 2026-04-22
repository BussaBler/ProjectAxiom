#type vertex
#version 460
#pragma shader_stage(vertex)

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;

layout(push_constant, std430) uniform PushConstants {
    mat4 uProjectionView;
} pushConstants;

layout(location = 0) out vec4 vColor;

void main() {
    vColor = aColor;
    gl_Position = pushConstants.uProjectionView * vec4(aPosition, 1.0);
}

#type fragment
#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec4 vColor;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vColor;
}