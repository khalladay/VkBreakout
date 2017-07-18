#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform MVP { mat4 data; } mvp;
layout(binding = 1) uniform COLOR { vec4 col; } col;


layout(location = 0) in vec3 vertex;
layout(location = 0) out vec4 fragColor;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() 
{
    gl_Position = vec4(vertex, 1.0);// * mvp.data;
    fragColor = vec4(1,0,0,1);//col.col;
}
