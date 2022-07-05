#version 330

precision highp float;

in Fragment
{
	vec2 texCoords;
	vec4 color;
} fragment;

out vec4 finalColor;

uniform sampler2D tex;

void main(void)
{
	vec4 texel = texture(tex, fragment.texCoords) * fragment.color;
	if (texel.a < 0.1)
	{
		discard;
	}
	finalColor = texel;
}