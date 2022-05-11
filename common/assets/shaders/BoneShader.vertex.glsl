#version 400

#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_UV_x2;
layout(location=2) in vec4 in_Normal;
layout(location=3) in vec4 in_BoneWeights;
layout(location=4) in vec4 in_BoneIDs;

out vec4 ex_Position;			
out vec4 ex_PositionWorld;
out vec4 ex_Normal;
out vec4 ex_UV_x2;

uniform mat4 ModelMatrix;	
uniform mat4 RotationMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

//uniform int NumberOfBones;
uniform mat4 BoneMatrices[48];
//uniform mat4 BoneRotationMatrices[4];

void main(void)
{
	mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	
	// Positions
	vec4 position = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	position += BoneMatrices[int(in_BoneIDs[0])] * in_Position;

	// Normals
	//vec4 normal = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	//normal += normalize(position);
	
	gl_Position = MVP * position;

	ex_Position = position;
	ex_PositionWorld = ModelMatrix * position;
	ex_Normal = RotationMatrix * normalize(in_Normal);
	ex_UV_x2 = in_UV_x2;

	return;
}
