#type vertex
#version 450 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4   u_viewProjectionMatrix;
out vec2   o_TexCoord;

uniform vec2 u_viewportSize;
uniform vec2 u_VideoSize;

void main(void)
{
    float viewportAspectRatio = viewportSize.x / viewportSize.y;
    float videoAspectRatio = videoSize.x / videoSize.y;

    float scale = min(viewportAspectRatio / videoAspectRation, 1.0);
    vec2 scaledVideoSize = videoSize * scale;
    vec2 translation = (viewportSize - scaledVideoSize) / 2.0;

    vec4 scaledPosition = a_Position * scale + vec4(translation, 0.0f, 0.0f);


    o_TexCoord = a_TexCoord;
    gl_Position = u_viewProjectionMatrix * scaledPosition;
}

#type fragment
#version 450 core

precision highp float;

uniform sampler2D u_TextureY;
uniform sampler2D u_TextureU;
uniform sampler2D u_TextureV;

in vec2 o_TexCoord;
out vec4 fragColor;

void main(void)
{
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture(u_TextureY, o_TexCoord).r;
    yuv.y = texture(u_TextureU, o_TexCoord).r - 0.5;
    yuv.z = texture(u_TextureV, o_TexCoord).r - 0.5;
    rgb = mat3( 1,1,1, 0,-0.39465,2.03211,1.13983,-0.58060,0) * yuv;
    fragColor = vec4(rgb, 1);
}
