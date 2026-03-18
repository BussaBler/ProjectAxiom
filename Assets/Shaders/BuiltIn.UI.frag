#version 460
#pragma shader_stage(fragment)

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 oColor;

void main() {
    oColor = vColor;
}