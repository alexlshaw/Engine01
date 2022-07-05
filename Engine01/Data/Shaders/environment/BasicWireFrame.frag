#version 330

precision highp float;

in Fragment
{
	vec3 dist;
} fragment;

out vec4 finalColor;

uniform vec3 wireColor;
uniform vec3 fillColor;

void main(void)
{

	// Undo perspective correction.
	vec3 dist_vec = fragment.dist * gl_FragCoord.w;

	// Compute the shortest distance to the edge
	float d = min(dist_vec[0], min(dist_vec[1], dist_vec[2]));

	// Compute line intensity and then fragment color
	float I = exp2(-2.0 * d * d);
	finalColor.a = 1.0;
	finalColor.xyz = I * wireColor + (1.0 - I) * fillColor;
}