#version 330 core

layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;

uniform vec2 screenVector;	//this should be a vector of x = 0.5 * screenWidth, y = 0.5 * screenHeight

out vec2 UV;

void main()
{
	// Output position of the vertex, in clip space
	// map [0..screenWidth][0..screenHeight] to [-1..1][-1..1]
	vec2 vertexPosition_homoneneousspace = vertexPosition_screenspace - screenVector; // [0..screenWidth][0..screenHeight] -> [-0.5sW..0.5sW][-0.5sH..sH]
	vertexPosition_homoneneousspace /= screenVector;

	gl_Position =  vec4(vertexPosition_homoneneousspace, 0, 1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}