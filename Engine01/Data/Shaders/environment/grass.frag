#version 450

precision highp float;

in Fragment
{
	vec2 texCoords;
} fragment;

out vec4 finalColor;

uniform sampler2D textures;

void main(void)
{
	vec4 texel = texture(textures, fragment.texCoords) * vec4(0.75, 0.75, 0.75, 1.0);
	if (texel.a < 0.5)
	{
		discard;
	}
	finalColor = texel;
}