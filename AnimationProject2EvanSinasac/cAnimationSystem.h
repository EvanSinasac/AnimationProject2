#pragma once

#include "KeyFrame.h"
#include "Animation.h"
#include "cAnimatedEntity.h"

// Animation system process gets called every frame to update all the entities' animations
class AnimationSystem
{
public:
	virtual void Process(const std::vector<cAnimatedEntity*>& entities, float dt);
private:
	int FindKeyFramePositionIndex(Animation* animation, float time);
	int FindKeyFrameScaleIndex(Animation* animation, float time);
	int FindKeyFrameRotationIndex(Animation* animation, float time);
};