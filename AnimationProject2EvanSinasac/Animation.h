#pragma once

#include <vector>

#include "KeyFrame.h"

// Animation struct to keep track of and play keyframe animations
struct Animation
{
	bool playing;
	float duration;
	float currentTime;
	float speed;
	bool repeat;
	std::vector<KeyFramePosition> keyFramePositions;
	std::vector<KeyFrameRotation> keyFrameRotations;
	std::vector<KeyFrameScale> keyFrameScales;
};