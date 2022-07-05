#version 330 core

in vec2 UV;

out vec4 color;

uniform sampler2D font;

void main()
{
	vec4 modifier = vec4(1,1,1,1);
	vec4 texel = texture2D( font, UV );
	if (texel.a > 0.0 && texel.a < 0.2) {
		modifier = vec4(0,0,0,1);
	}
	color = texel * modifier;
}