#version 330

layout (location = 0) in vec2 vPos;

uniform mat4 orthoMatrix;
uniform float minSize = 5;
uniform float sizeIncrement;

void main(void) 
{
	gl_PointSize = minSize + sizeIncrement;
	gl_Position = orthoMatrix * vec4(vPos, 0.5, 1.0);
}
