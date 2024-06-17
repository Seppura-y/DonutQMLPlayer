#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_viewProjectionMatrix;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;


out vec2 v_TexCoord;
out vec4 v_Color;


void main()
{
	//v_TexCoord = a_TexCoord;
	v_Color = a_Color;

	//gl_Position = u_viewProjectionMatrix * vec4(a_Position, 1.0);
	gl_Position = u_projection * u_view * u_model * vec4(a_Position, 1.0);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 color;

//in vec2 v_TexCoord;
in vec4 v_Color;

uniform float u_tiling_factor;


void main()
{
    //vec4 texColor = v_Color;
	color = texColor;
}