#version 450

precision highp float;

layout (location = 0) in vec4 position;

uniform mat4 transform;
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform sampler2D heightMapTexture[9];
uniform sampler2D splatMaps[9];

out Fragment
{
	vec2 texCoords;
	vec3 normal;
	vec4 splat;
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

//as an alternative to calculating this on the fly, I could calculate the normal map the same way I do the heightmap, and have it in a side thread as well
vec3 getNormal(vec3 p)
{
	vec3 up = vec3(p.x, 0, p.z + 1);
	vec3 um = vec3(p.x, 0, p.z - 1);
	float u1 = texture(heightMapTexture[getIndex(up)], up.zx / 1024).r;
	float u2 = texture(heightMapTexture[getIndex(um)], um.zx / 1024).r;
	vec3 u = vec3(0, u1 - u2, 2);

	vec3 vp = vec3(p.x + 1, 0, p.z);
	vec3 vm = vec3(p.x - 1, 0, p.z);
	float v1 = texture(heightMapTexture[getIndex(vp)], vp.zx / 1024).r;
	float v2 = texture(heightMapTexture[getIndex(vm)], vm.zx / 1024).r;
	vec3 v = vec3(2, v1 - v2, 0);

	return normalize(cross(u, v));
}

void main(void)
{
	vec3 p = (transform * position).xyz;													//transform modified position
	const int index = getIndex(p);
	vec4 newPos = vec4(p.x, texture(heightMapTexture[index], p.zx / 1024).r, p.z, 1);		//heightmap modified position
	fragment.normal = getNormal(p);
	fragment.texCoords = p.xz / 4;
	fragment.splat = texture(splatMaps[index], p.zx / 1024);
	gl_Position = projection_matrix * modelview_matrix * newPos;
}

//we use p.zx rather than p.xz for heightmap lookups because the orientation at which the textures are generated is different to the orientation openGL uses for texture coords