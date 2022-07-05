#version 330

in vec3 color;
in vec3 wPos;

out vec4 fragColor;

layout (std140) uniform FogBlock
{
	vec3 skyColor;
	vec3 fogColor;
	float fogEnd;
	float fogScale;
};

void main()
{
	float dist = length(wPos);
	float fogFactor = (fogEnd - dist) * fogScale;
	fogFactor = clamp(fogFactor, 0.0, 1.0);
	fragColor = vec4(mix(fogColor, color, fogFactor), 0.5);
}