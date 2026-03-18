#version 460
#pragma shader_stage(fragment)

layout (location = 0) in vec2 vUv;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D texSampler0;

void main() {
	outColor = texture(texSampler0, vUv);
}