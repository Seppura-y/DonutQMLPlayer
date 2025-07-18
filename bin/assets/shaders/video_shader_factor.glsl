
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

uniform int pixel_format; // 0: YUV420P, 1: YUVJ420P, 2: YUV420P10LE
uniform int color_space;  // 0: BT.601, 1: BT.709

void main(void)
{
    vec3 yuv;
    
    yuv.x = texture(u_textures[y_index], o_TexCoord).r;
    yuv.y = texture(u_textures[u_index], o_TexCoord).r - 0.5;
    yuv.z = texture(u_textures[v_index], o_TexCoord).r - 0.5;
    // 处理像素格式
    // YUV420P10LE
    if (pixel_format == 2)
    { 
        yuv *= (65535.0 / 1023.0); // 恢复 10 位数据
        yuv.x = (yuv.x * 1023.0 - 64.0) / (940.0 - 64.0); // Y: [64, 940] -> [0, 1]
        yuv.y = (yuv.y * 1023.0) / (960.0 - 64.0);        // U: [64, 960] -> [-0.5, 0.5]
        yuv.z = (yuv.z * 1023.0) / (960.0 - 64.0);        // V: [64, 960] -> [-0.5, 0.5]
    }
    // YUVJ420P (全范围)
    else if (pixel_format == 1)
    {
        // Y: [0, 1], U/V: [-0.5, 0.5]
    }
    // YUV420P (视频范围)
    else
    {
        yuv.x = (yuv.x * 255.0 - 16.0) / (235.0 - 16.0); // Y: [16, 235] -> [0, 1]
        yuv.y = (yuv.y * 255.0) / (240.0 - 16.0);        // U: [16, 240] -> [-0.5, 0.5]
        yuv.z = (yuv.z * 255.0) / (240.0 - 16.0);        // V: [16, 240] -> [-0.5, 0.5]
    }
    mat3 yuv2rgb;
    // BT.709
    if (color_space == 1)
    {
        yuv2rgb = mat3(
            1.0, 1.0, 1.0,
            0.0, -0.1873, 1.8556,
            1.5748, -0.4681, 0.0
        );
    }
    // BT.601
    else
    { 
        yuv2rgb = mat3(
            1.0, 1.0, 1.0,
            0.0, -0.344, 1.772,
            1.402, -0.714, 0.0
        );
    }
    vec3 rgb = yuv2rgb * yuv;
    fragColor = vec4(rgb, 1.0);
    fragColor = vec4(vec3(yuv.x), 1.0);
}