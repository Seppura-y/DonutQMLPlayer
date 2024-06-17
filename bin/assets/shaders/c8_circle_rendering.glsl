#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;
layout(location = 1) in vec3 a_LocalPosition;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in float a_Thickness;
layout(location = 4) in float a_Fade;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_viewProjectionMatrix;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) out VertexOutput Output;
layout (location = 4) out flat int v_EntityID;

void main()
{
	Output.LocalPosition = a_LocalPosition;
	Output.Color = a_Color;
	Output.Thickness = a_Thickness;
	Output.Fade = a_Fade;

	v_EntityID = a_EntityID;

	gl_Position = u_viewProjectionMatrix * vec4(a_WorldPosition, 1.0);
//	gl_Position = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

#type fragment
#version 450 core
//
layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;

void main()
{
    // Calculate distance and fill circle with white
	// local position相当于uv, 
    float distance_ = 1.0 - length(Input.LocalPosition);
	// param
	// edge0 :	Specifies the value of the lower edge of the Hermite function.
	// edge1 :	Specifies the value of the upper edge of the Hermite function.
	// x :		Specifies the source value for interpolation.
    float circle_ = smoothstep(0.0, Input.Fade, distance_);
    circle_ *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance_);

	// 加入discard无法编译？
	if (circle_ == 0.0f)
	{
//		discard;
	}

    // Set output color
    o_Color = Input.Color;
	o_Color.a *= circle_;

	o_EntityID = v_EntityID;

//	o_Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}