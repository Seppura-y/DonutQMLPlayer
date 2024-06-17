#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_TextureIndices;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_viewProjectionMatrix;
};

out vec2 o_TexCoord;

flat out int y_index;
flat out int u_index;
flat out int v_index;

void main(void)
{
    o_TexCoord = a_TexCoord;

    y_index = int(a_TextureIndices.x);
    u_index = int(a_TextureIndices.y);
    v_index = int(a_TextureIndices.z);

    gl_Position = u_viewProjectionMatrix * a_Position;
}

#type fragment
#version 450 core

//layout(location = 0) uniform sampler2D u_TextureY;
//layout(location = 1) uniform sampler2D u_TextureU;
//layout(location = 2) uniform sampler2D u_TextureV;

in vec2 o_TexCoord;

flat in int y_index;
flat in int u_index;
flat in int v_index;

out vec4 fragColor;

layout(binding = 0) uniform sampler2D u_textures[32];

void main(void)
{
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture(u_textures[y_index], o_TexCoord).r;
    yuv.z = texture(u_textures[v_index], o_TexCoord).r - 0.5;
    yuv.y = texture(u_textures[u_index], o_TexCoord).r - 0.5;
    
    //yuv.y = 1.0;
    //yuv.z = 1.0;
    rgb = mat3( 1,1,1, 0,-0.39465,2.03211,1.13983,-0.58060,0) * yuv;
    //fragColor = vec4(rgb, 1);

    //if(y_index == 1 && u_index == 2 && v_index == 3)
    //    fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //else
    //    fragColor = vec4(0.1f * y_index, 0.1f * u_index, 0.1f * v_index, 1.0f);

    //fragColor = texture(u_textures[0], o_TexCoord);
    //fragColor = vec4(1.0f , 1.0f, 1.0f, 1.0f);
}
