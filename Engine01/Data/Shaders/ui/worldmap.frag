#version 450

precision highp float;

in Fragment
{
	vec2 tex_coord;
} fragment;

uniform sampler2D tex;

layout(location = 0) out vec4 final_color;

void main(void)
{
	final_color = texture(tex, fragment.tex_coord);
}