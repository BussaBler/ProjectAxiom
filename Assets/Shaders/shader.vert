#version 460
#pragma shader_stage(vertex)

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec4 aUv;

layout (location = 0) out vec2 vUv;

void main() {
	gl_Position = vec4(aPos.xyz, 1.0);
	vUv = aUv.xy;
}