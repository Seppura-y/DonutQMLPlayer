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
    yuv.y = texture(u_textures[u_index], o_TexCoord).r - 0.5;
    yuv.z = texture(u_textures[v_index], o_TexCoord).r - 0.5;
    
    rgb = mat3(
        1.0,  1.0, 1.0,
        0.0, -0.344, 1.772,
        1.402, -0.714, 0.0
    ) * yuv;
    
    fragColor = vec4(rgb, 1.0);
}

