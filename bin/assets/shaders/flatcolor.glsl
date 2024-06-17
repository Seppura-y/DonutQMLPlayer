#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;

//uniform mat4 u_viewProjectionMatrix;
//uniform mat4 u_transformMatrix;

void main()
{
	//gl_Position = u_viewProjectionMatrix * u_transformMatrix * vec4(a_Position, 1.0);
	gl_Position = a_Position, 1.0;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;

//uniform vec4 u_color;

void main()
{
	color = vec4(1.0, 1.0, 1.0, 1.0);
}