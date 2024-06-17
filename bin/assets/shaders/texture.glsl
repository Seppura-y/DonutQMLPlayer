#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_viewProjectionMatrix;
};

//uniform mat4 u_viewProjectionMatrix;
uniform mat4 u_transformMatrix;

out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	v_TexIndex = a_TexIndex;
	gl_Position = u_viewProjectionMatrix * a_Position;
	//gl_Position = u_viewProjectionMatrix * u_transformMatrix * a_Position;
	//gl_Position = a_Position;
}

#type fragment
#version 450 core

out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;

layout(binding = 0) uniform sampler2D u_textures[32];
//layout(binding = 1) uniform sampler2D u_tex;

void main()
{
	//color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	color = texture(u_textures[int(v_TexIndex)], v_TexCoord);
}