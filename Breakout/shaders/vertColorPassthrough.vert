#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PER_OBJECT 
{ 
	mat4 mvp; 
	vec4 col;
} obj;

layout(location = 0) in vec3 vertex;
layout(location = 0) out vec4 fragColor;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
    gl_Position = obj.mvp * vec4(vertex, 1.0);
    fragColor = obj.col;
}
