#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_viewProjectionMatrix;
};

struct VertexOutput
{
	vec4 color;
	vec2 tex_coord;
	float tiling_factor;
};

layout(location = 0) out VertexOutput v_Output;
layout(location = 3) out flat float v_TexIndex;
layout(location = 4) out flat int v_EntityID;

void main()
{
	v_Output.tex_coord = a_TexCoord;
	v_Output.color = a_Color;
	v_Output.tiling_factor = a_TilingFactor;
	v_TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

	gl_Position = u_viewProjectionMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

struct VertexOutput
{
	vec4 color;
	vec2 tex_coord;
	float tiling_factor;
};

layout(location = 0) in VertexOutput Input;
layout(location = 3) in flat float v_TexIndex;
layout(location = 4) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_textures[32];

void main()
{
    vec4 texColor = Input.color;
	switch(int(v_TexIndex))
	{
		case 0: texColor *= texture(u_textures[0], Input.tex_coord * Input.tiling_factor); break;
		case 1: texColor *= texture(u_textures[1], Input.tex_coord * Input.tiling_factor); break;
		case 2: texColor *= texture(u_textures[2], Input.tex_coord * Input.tiling_factor); break;
		case 3: texColor *= texture(u_textures[3], Input.tex_coord * Input.tiling_factor); break;
		case 4: texColor *= texture(u_textures[4], Input.tex_coord * Input.tiling_factor); break;
		case 5: texColor *= texture(u_textures[5], Input.tex_coord * Input.tiling_factor); break;
		case 6: texColor *= texture(u_textures[6], Input.tex_coord * Input.tiling_factor); break;
		case 7: texColor *= texture(u_textures[7], Input.tex_coord * Input.tiling_factor); break;
		case 8: texColor *= texture(u_textures[8], Input.tex_coord * Input.tiling_factor); break;
		case 9: texColor *= texture(u_textures[9], Input.tex_coord * Input.tiling_factor); break;
		case 10: texColor *= texture(u_textures[10], Input.tex_coord * Input.tiling_factor); break;
		case 11: texColor *= texture(u_textures[11], Input.tex_coord * Input.tiling_factor); break;
		case 12: texColor *= texture(u_textures[12], Input.tex_coord * Input.tiling_factor); break;
		case 13: texColor *= texture(u_textures[13], Input.tex_coord * Input.tiling_factor); break;
		case 14: texColor *= texture(u_textures[14], Input.tex_coord * Input.tiling_factor); break;
		case 15: texColor *= texture(u_textures[15], Input.tex_coord * Input.tiling_factor); break;
		case 16: texColor *= texture(u_textures[16], Input.tex_coord * Input.tiling_factor); break;
		case 17: texColor *= texture(u_textures[17], Input.tex_coord * Input.tiling_factor); break;
		case 18: texColor *= texture(u_textures[18], Input.tex_coord * Input.tiling_factor); break;
		case 19: texColor *= texture(u_textures[19], Input.tex_coord * Input.tiling_factor); break;
		case 20: texColor *= texture(u_textures[20], Input.tex_coord * Input.tiling_factor); break;
		case 21: texColor *= texture(u_textures[21], Input.tex_coord * Input.tiling_factor); break;
		case 22: texColor *= texture(u_textures[22], Input.tex_coord * Input.tiling_factor); break;
		case 23: texColor *= texture(u_textures[23], Input.tex_coord * Input.tiling_factor); break;
		case 24: texColor *= texture(u_textures[24], Input.tex_coord * Input.tiling_factor); break;
		case 25: texColor *= texture(u_textures[25], Input.tex_coord * Input.tiling_factor); break;
		case 26: texColor *= texture(u_textures[26], Input.tex_coord * Input.tiling_factor); break;
		case 27: texColor *= texture(u_textures[27], Input.tex_coord * Input.tiling_factor); break;
		case 28: texColor *= texture(u_textures[28], Input.tex_coord * Input.tiling_factor); break;
		case 29: texColor *= texture(u_textures[29], Input.tex_coord * Input.tiling_factor); break;
		case 30: texColor *= texture(u_textures[30], Input.tex_coord * Input.tiling_factor); break;
		case 31: texColor *= texture(u_textures[31], Input.tex_coord * Input.tiling_factor); break;
	}
	color = texColor;

	color2 = v_EntityID;
}