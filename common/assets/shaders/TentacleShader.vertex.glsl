#version 400

//layout(location=0) in vec4 in_Position; 
//layout(location=2) in vec4 in_Normal;
//layout(location=1) in vec4 in_UV_x2;
layout(location=3) in vec4 in_BoneWeights_x4;
layout(location=3) in vec4 in_BoneIDs_x4;

in vec4 vColour;
in vec4 vPosition;
in vec4 vNormal;				// Vertex normal X,Y,Z (W ignored)
in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1

out vec4 ex_Position;			
out vec4 ex_PositionWorld;
out vec4 ex_Normal;
out vec4 ex_UV_x2;

uniform mat4 RotationMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform int NumberOfBones;
//uniform mat4 BoneMatrices[512];
uniform mat4 BoneMatrices[48];
uniform mat4 BoneRotationMatrices[48];
//uniform mat4 BoneRotationMatrices[512];

void main(void)
{
// Calculate MVP Matrix
	mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

// Calculate PositionWorld	
	vec4 position = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	//position += BoneMatrices[int(in_BoneIDs_x4[0])] * in_Position * in_BoneWeights_x4[0];
	position += BoneMatrices[int(in_BoneIDs_x4[0])] * vPosition * in_BoneWeights_x4[0];
	//position += BoneMatrices[int(in_BoneIDs_x4[1])] * in_Position * in_BoneWeights_x4[1];
	position += BoneMatrices[int(in_BoneIDs_x4[1])] * vPosition * in_BoneWeights_x4[1];
	//position += BoneMatrices[int(in_BoneIDs_x4[2])] * in_Position * in_BoneWeights_x4[2];
	position += BoneMatrices[int(in_BoneIDs_x4[2])] * vPosition * in_BoneWeights_x4[2];
	//position += BoneMatrices[int(in_BoneIDs_x4[3])] * in_Position * in_BoneWeights_x4[3];
	
// Calculate Normal	
	vec4 normal = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	//normal += BoneRotationMatrices[int(in_BoneIDs_x4[0])] * normalize(in_Position) * in_BoneWeights_x4[0];
	normal += BoneRotationMatrices[int(in_BoneIDs_x4[0])] * normalize(vPosition) * in_BoneWeights_x4[0];
	//normal += BoneRotationMatrices[int(in_BoneIDs_x4[1])] * normalize(in_Position) * in_BoneWeights_x4[1];
	normal += BoneRotationMatrices[int(in_BoneIDs_x4[1])] * normalize(vPosition) * in_BoneWeights_x4[1];
	//normal += BoneRotationMatrices[int(in_BoneIDs_x4[2])] * normalize(in_Position) * in_BoneWeights_x4[2];
	normal += BoneRotationMatrices[int(in_BoneIDs_x4[2])] * normalize(vPosition) * in_BoneWeights_x4[2];
	//normal += BoneRotationMatrices[int(in_BoneIDs_x4[3])] * normalize(in_Position) * in_BoneWeights_x4[3];
	
	gl_Position = MVP * position;
	ex_Normal = RotationMatrix * normalize(normal);
	ex_Position = position;
	ex_PositionWorld = ModelMatrix * position;
	ex_UV_x2 = vUVx2;
	//ex_UV_x2 = in_UV_x2;
	
	return;
	
	
	/*
	mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	gl_Position = MVP * in_Position;
	
	ex_Position = in_Position;
	ex_PositionWorld = ModelMatrix * in_Position;
	ex_Normal = RotationMatrix * normalize(in_Normal);
	ex_UV_x2 = in_UV_x2;
	*/
}
