// VERTEX SHADER
#version 330 core

// Create an input 3D vector representing vertex position at location = 0
layout (location = 0) in vec3 aPos;

void main() {
	// Send position data as output
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}