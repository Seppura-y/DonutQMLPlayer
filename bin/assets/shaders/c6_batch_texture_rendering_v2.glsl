#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

uniform mat4 u_viewProjectionMatrix;


out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;
out float v_TilingFactor;
out int v_EntityID;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;
	v_EntityID = a_EntityID;
	gl_Position = u_viewProjectionMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;
in float v_TilingFactor;
flat in int v_EntityID;

uniform float u_tiling_factor;
uniform sampler2D u_textures[32];

void main()
{
    vec4 texColor = v_Color;
	switch(int(v_TexIndex))
	{
		case 0: texColor *= texture(u_textures[0], v_TexCoord * v_TilingFactor); break;
		case 1: texColor *= texture(u_textures[1], v_TexCoord * v_TilingFactor); break;
		case 2: texColor *= texture(u_textures[2], v_TexCoord * v_TilingFactor); break;
		case 3: texColor *= texture(u_textures[3], v_TexCoord * v_TilingFactor); break;
		case 5: texColor *= texture(u_textures[5], v_TexCoord * v_TilingFactor); break;
		case 6: texColor *= texture(u_textures[6], v_TexCoord * v_TilingFactor); break;
		case 7: texColor *= texture(u_textures[7], v_TexCoord * v_TilingFactor); break;
		case 8: texColor *= texture(u_textures[8], v_TexCoord * v_TilingFactor); break;
		case 9: texColor *= texture(u_textures[9], v_TexCoord * v_TilingFactor); break;
		case 10: texColor *= texture(u_textures[10], v_TexCoord * v_TilingFactor); break;
		case 11: texColor *= texture(u_textures[11], v_TexCoord * v_TilingFactor); break;
		case 12: texColor *= texture(u_textures[12], v_TexCoord * v_TilingFactor); break;
		case 13: texColor *= texture(u_textures[13], v_TexCoord * v_TilingFactor); break;
		case 14: texColor *= texture(u_textures[14], v_TexCoord * v_TilingFactor); break;
		case 15: texColor *= texture(u_textures[15], v_TexCoord * v_TilingFactor); break;
		case 16: texColor *= texture(u_textures[16], v_TexCoord * v_TilingFactor); break;
		case 17: texColor *= texture(u_textures[17], v_TexCoord * v_TilingFactor); break;
		case 18: texColor *= texture(u_textures[18], v_TexCoord * v_TilingFactor); break;
		case 19: texColor *= texture(u_textures[19], v_TexCoord * v_TilingFactor); break;
		case 20: texColor *= texture(u_textures[20], v_TexCoord * v_TilingFactor); break;
		case 21: texColor *= texture(u_textures[21], v_TexCoord * v_TilingFactor); break;
		case 22: texColor *= texture(u_textures[22], v_TexCoord * v_TilingFactor); break;
		case 23: texColor *= texture(u_textures[23], v_TexCoord * v_TilingFactor); break;
		case 24: texColor *= texture(u_textures[24], v_TexCoord * v_TilingFactor); break;
		case 25: texColor *= texture(u_textures[25], v_TexCoord * v_TilingFactor); break;
		case 26: texColor *= texture(u_textures[26], v_TexCoord * v_TilingFactor); break;
		case 27: texColor *= texture(u_textures[27], v_TexCoord * v_TilingFactor); break;
		case 28: texColor *= texture(u_textures[28], v_TexCoord * v_TilingFactor); break;
		case 29: texColor *= texture(u_textures[29], v_TexCoord * v_TilingFactor); break;
		case 30: texColor *= texture(u_textures[30], v_TexCoord * v_TilingFactor); break;
		case 31: texColor *= texture(u_textures[31], v_TexCoord * v_TilingFactor); break;
	}
	color = texColor;
	color2 = v_EntityID;
}