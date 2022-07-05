#version 450

precision highp float;

uniform vec3 v3LightPos;
uniform float g;
uniform float g2;

in vec3 v3Direction;

in Fragment
{
	vec3 color;
	vec3 secondaryColor;
} fragment;

out vec4 finalColor;

void main (void)
{
	finalColor = vec4(1.0, 1.0, 1.0, 1.0);

	float fCos = dot(v3LightPos, v3Direction) / length(v3Direction);
	float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0*g*fCos, 1.5);
	//finalColor = gl_Color + fMiePhase * gl_SecondaryColor;
	finalColor.rgb = fragment.color + (fMiePhase * fragment.secondaryColor);
	finalColor.a = finalColor.b;

}
