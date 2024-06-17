#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_viewProjectionMatrix;
uniform mat4 u_transformMatrix;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_viewProjectionMatrix * u_transformMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_texture;
uniform vec4 u_color;
uniform float u_tiling_factor;

void main()
{
	color = texture(u_texture, v_TexCoord * u_tiling_factor) * u_color;
}