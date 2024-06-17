#type vertex
#version 450 core

layout(location = 0) in vec4 vertices;

out vec2 coords;
uniform float u_test;

uniform mat4 u_translation;

out float o_test;
void main() {
    //vec4 transformedPosition = u_translation * vertices;
    vec4 transformedPosition = vertices;
	gl_Position = transformedPosition;
	coords = transformedPosition.xy / transformedPosition.w;
	o_test = u_test;
}

#type fragment
#version 450 core

uniform float t;
in vec2 coords;

in float o_test;

out vec4 o_color;
void main() {
    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));
    i = smoothstep(o_test - 0.8, o_test + 0.8, i);
    //i = smoothstep(t - 0.8, t + 0.8, i);
    i = floor(i * 20.) / 20.;
    o_color = vec4(coords * .5 + .5, i, i);

    //o_color = vec4(1.0, 1.0, 1.0, 1.0);
}
