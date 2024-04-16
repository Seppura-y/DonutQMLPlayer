#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;

uniform float u_tiling_factor;


void main()
{
    vec4 texColor = v_Color;
	color = texColor;
}