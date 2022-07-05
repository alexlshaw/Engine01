#version 450

precision highp float;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;

uniform mat4 transform;
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;

uniform sampler2D heightMapTexture[9];
uniform sampler2D splatMapTexture[9];

out Fragment
{
	vec2 texCoords;
} fragment;

//determine which heightmap texture the given point belongs to
int getIndex(vec3 p)
{
	float xOffset = transform[3].x - (int(transform[3].x) % 1024) - 1024;
	float zOffset = transform[3].z - (int(transform[3].z) % 1024) - 1024;
	int xIndex = int(floor((p.x - xOffset) / 1024));
	int zIndex = int(floor((p.z - zOffset) / 1024));
	return xIndex + 3 * zIndex;
}

void main(void)
{
	vec3 p = (transform * vec4(position, 1)).xyz;													//transform modified position
	const int index = getIndex(p);
	float hmf = texture(heightMapTexture[index], p.zx / 1024).r;	//look at the heightmap to get the position of the grass
	float gf = texture(splatMapTexture[index], p.zx / 1024).b;		// look at the splatmap to determine if the grass should be there at all

	float off = 0.5 * round(fract(hmf));	//off stores either 0.5 or 0

	float newY = p.y - 2.0 + (2.0 * gf) + hmf;
	vec4 newPos = vec4(p.x, newY, p.z, 1.0);
	fragment.texCoords = vec2(texcoord.x + off, texcoord.y);
	gl_Position = projection_matrix * modelview_matrix * newPos;
}