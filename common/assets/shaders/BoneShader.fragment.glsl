#version 400

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_separate_shader_objects : enable

// in from vertex shader
in vec4 ex_Position;
in vec4 ex_PositionWorld;
in vec4 ex_Normal;
in vec4 ex_UV_x2;

// Default colour output
layout ( location = 0 ) out vec4 out_Colour;

void main(void)
{
	out_Colour = ex_Normal;
}