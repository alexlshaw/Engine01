#version 330

out vec4 vFragColor;

uniform vec4 color = vec4(1,1,1,1);

uniform sampler2D  pointSpriteTex;

void main(void)
{ 
	vec4 texel = texture(pointSpriteTex, gl_PointCoord);
	if (texel.r < 0.2)
	{
		discard;
	}
	vFragColor = texel * color;
}