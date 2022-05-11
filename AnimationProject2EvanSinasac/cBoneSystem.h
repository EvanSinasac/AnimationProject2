#pragma once

#include "GLMCommon.h"
#include "cBoneEntity.h"
#include "Animation.h"

//class Animation;	// ?
//class BoneHierarchy;
//class Bone;

class cBoneSystem //: System
{
public:
	virtual void Process(const std::vector<cBoneEntity*>& entities, float dt);
private:
	void UpdateHierarchy(BoneHierarchy* boneHierarchy);
	void UpdateBone(BoneHierarchy* boneHierarchy, Bone* bone, 
		const glm::mat4 parentModelMatrix);
};