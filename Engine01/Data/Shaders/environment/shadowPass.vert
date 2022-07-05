#version 330 core
 
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 depthVP;
uniform mat4 modelMatrix;

void main()
{
	vec4 newPosition = modelMatrix * vec4(vertexPosition_modelspace.xyz, 1);
	gl_Position =  depthVP * newPosition;
}