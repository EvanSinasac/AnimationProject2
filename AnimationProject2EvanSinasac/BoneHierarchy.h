#pragma once
#pragma once

#include "../Bone.h"
//#include <glm/mat4x4.hpp>
#include "GLMCommon.h"

struct Transform
{
	//glm::mat4 value;
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;
};

struct BoneHierarchy
{
	Bone* rootBone;
	std::vector<Bone*> bones;
	std::vector<Transform> currentTransforms;
	float time;
};
