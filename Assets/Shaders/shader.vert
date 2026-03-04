#version 460
#pragma shader_stage(vertex)

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;


void main() {
	gl_Position = vec4(aPos.xyz, 1.0);
}