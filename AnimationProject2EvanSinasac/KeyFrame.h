#pragma once

#include "GLMCommon.h"

// Same as we did in class

enum EasingType
{
	EaseIn,
	EaseOut,
	EaseInOut,
	None
};

#define DIRECT_ROTATION 0
#define LERP_ROTATION 1
#define SLERP_ROTATION 2

struct KeyFramePosition
{
	KeyFramePosition(float time, const glm::vec3& position, EasingType easingType = EasingType::None)
		: time(time), position(position), easingType(easingType) {}
	float time;
	glm::vec3 position;
	EasingType easingType;
};

struct KeyFrameScale
{
	KeyFrameScale(float time, const glm::vec3& scale, EasingType easingType = EasingType::None)
		: time(time), scale(scale), easingType(easingType) {}
	float time;
	glm::vec3 scale;
	EasingType easingType;
};

struct KeyFrameRotation
{
	KeyFrameRotation(float time, const glm::quat& rotation, int interpolationType = 0, EasingType easingType = EasingType::None)
		: time(time), rotation(rotation), InterpolationType(interpolationType), easingType(easingType) {}
	float time;
	glm::quat rotation;
	EasingType easingType;
	int InterpolationType;
};