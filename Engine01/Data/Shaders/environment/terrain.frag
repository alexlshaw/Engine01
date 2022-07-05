#version 330

layout (std140) uniform SunBlock
{
	vec3 wLightPos;
	vec3 lightIntensity;
	vec3 Ka;
	vec3 Kd;
};

layout (std140) uniform FogBlock
{
	vec3 skyColor;
	vec3 fogColor;
	float fogEnd;
	float fogScale;
};

uniform sampler2D terrainTex;

in Fragment
{
	vec2 texCoord;
	vec3 normal;
	vec3 position;
	vec4 color;
} fragment;

layout(location = 0) out vec4 FragColor;

vec3 ads()
{
	vec3 n = normalize(fragment.normal);
	vec3 s = normalize(wLightPos - fragment.position);
	vec3 v = normalize(vec3(-fragment.position));
	return lightIntensity * (Ka + Kd * max(dot(s,n), 0.0));
}

void main()
{
	vec3 color = ads() * texture(terrainTex, fragment.texCoord).xyz;
	float dist = length(fragment.position);
	float fogFactor = (fogEnd - dist) * fogScale;
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	color = mix(fogColor, color, fogFactor);
	FragColor = vec4(color, 1.0);// * fragment.color;
}