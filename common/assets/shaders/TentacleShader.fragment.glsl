#version 400

// in from vertex shader
in vec4 ex_Position;
in vec4 ex_PositionWorld;
in vec4 ex_Normal;
in vec4 ex_UV_x2;

// Default colour output
layout ( location = 0 ) out vec4 out_Colour;

uniform samplerCube SkyBoxTexture;

uniform int ShaderID;

void main(void)
{
	if ( ShaderID == 1 )
	{
		out_Colour = vec4(ex_Normal.xyz, 1.0f);
		return;
	}
	else if ( ShaderID == 2 )
	{
		out_Colour = texture(SkyBoxTexture, ex_Normal.xyz);
		return;
	}

	return;
}