#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 color;

layout (std140) uniform TransformBlock
{
	mat4 mvMatrix;
	mat4 pMatrix;
	mat3 normalMatrix;
};

out Fragment
{
	vec2 texCoord;
	vec3 normal;
	vec3 position;
	vec4 color;
} fragment;

void main()
{
	fragment.color = color;
	fragment.texCoord = texCoord;
	fragment.normal = normalize(normalMatrix * normal);
	fragment.position = vec3(mvMatrix * vec4(position.xyz, 1));
	gl_Position = pMatrix * mvMatrix * vec4(position.xyz, 1);
}