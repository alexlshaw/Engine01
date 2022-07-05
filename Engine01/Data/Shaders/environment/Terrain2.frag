#version 450

precision highp float;

in Fragment
{
	vec2 texCoords;
	vec3 normal;
	vec4 splat;
} fragment;

out vec4 finalColor;

uniform sampler2D dirtTex;
uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform sampler2D sandTex;
uniform sampler2D snowTex;
uniform vec3 lightIntensity;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 lightPos;

void main(void)
{
	vec3 light = lightIntensity * (ambient + diffuse * max(dot(fragment.normal, lightPos), 0.0));
	//do our texture lookups (we store as vec3s because we know they all have an alpha of 1)
	vec3 rock = texture(rockTex, fragment.texCoords).xyz;
	vec3 sand = texture(sandTex, fragment.texCoords).xyz;
	vec3 dirt = texture(dirtTex, fragment.texCoords).xyz;
	vec3 grass = texture(grassTex, fragment.texCoords).xyz;
	vec3 snow = texture(snowTex, fragment.texCoords).xyz;

	float sandBlend = fragment.splat.r;
	float dirtBlend = fragment.splat.g;
	float grassBlend = fragment.splat.b;
	float snowBlend = fragment.splat.a;

	vec3 color = mix(mix(mix(mix(rock, sand, sandBlend), dirt, dirtBlend), grass, grassBlend), snow, snowBlend);
	finalColor = vec4((light * color), 1);
}