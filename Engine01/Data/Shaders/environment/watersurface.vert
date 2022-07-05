#version 330

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 vColor;

uniform vec3 offset;

layout (std140) uniform TransformBlock
{
	mat4 mvMatrix;
	mat4 pMatrix;
	mat3 normalMatrix;
};

out vec3 color;
out vec3 wPos;

void main()
{
	wPos = (mvMatrix * vec4(vPos.xyz + offset,1)).xyz;
	gl_Position = pMatrix * mvMatrix * vec4(vPos.xyz + offset, 1);
	color = vColor.rgb;
}