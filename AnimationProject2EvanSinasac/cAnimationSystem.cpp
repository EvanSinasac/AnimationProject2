#include "cAnimationSystem.h"

#include "Animation.h"

#include "GLMCommon.h"
#include "globalThings.h"
#include <glm/gtx/easing.hpp>

// this is more or less the same animation system we built in class but the entities are not templated and do
// not have the Components system, so it only affects and can be used to process the AnimatedEntities
void AnimationSystem::Process(const std::vector<cAnimatedEntity*>& entities, float dt)
{
	// pointers
	Animation* animationPtr;
	glm::vec3 positionPtr;
	glm::quat rotationPtr;
	glm::vec3 scalePtr;
	cAnimatedEntity* currentEntityPtr;

	for (unsigned int index = 0; index < entities.size(); index++)
	{
		currentEntityPtr = entities[index];

		animationPtr = currentEntityPtr->vecAnimations[currentEntityPtr->animationIndex];
		positionPtr = currentEntityPtr->position;
		rotationPtr = currentEntityPtr->rotation;
		scalePtr = currentEntityPtr->scale;

		if (animationPtr == 0)
			continue;

		if (!animationPtr->playing)
			continue;

		animationPtr->currentTime += dt * animationPtr->speed * ::g_animationSpeed;


		if (animationPtr->currentTime > animationPtr->duration)
		{
			animationPtr->currentTime = animationPtr->duration;
			if (animationPtr->repeat == true)
			{
				animationPtr->currentTime = 0;
			}
			else
			{
				animationPtr->playing = false;
			}
		}

		// Find active KeyFramePositionPosition
		if (animationPtr->keyFramePositions.size() == 0
			|| animationPtr->keyFrameScales.size() == 0
			|| animationPtr->keyFrameRotations.size() == 0)
		{
			printf("Contains an empty keyframe vector");
			continue;
		}


		// Currently we are using only the current KeyFramePositionPosition,
		// but we need to smoothly animate between two KeyFramePositions
		// If the current time is at 1.5, and two KeyFramePositions are at
		// 1, and 2, then we must find a point between the two 
		// positions
		// 
		//KeyFramePositionPosition KeyFramePositionPosition = FindCurrentKeyFramePositionPosition(animationPtr, animationPtr->currentTime);
		//positionPtr->value = KeyFramePositionPosition.position;

		// If there is only 1 KeyFramePositionPosition in the animation, only use that
		if (animationPtr->keyFramePositions.size() == 1)
		{
			// We only need to set the position of the only KeyFramePositionPosition.
			positionPtr/*->value*/ = animationPtr->keyFramePositions[0].position;
			return;
		}

		int currPosFrameidx = FindKeyFramePositionIndex(animationPtr, animationPtr->currentTime);

		// If we are at the last KeyFramePositionPosition, only use that KeyFramePositionPosition.
		if (currPosFrameidx == animationPtr->keyFramePositions.size() - 1)
		{
			// We only need to set the position of the last KeyFramePositionPosition.
			positionPtr/*->value*/ = animationPtr->keyFramePositions[currPosFrameidx].position;
			return;
		}

		// Find a position between the current KeyFramePositionPosition, and the next KeyFramePositionPosition
		int nextPosFrameidx = currPosFrameidx + 1;

		const KeyFramePosition& keyFramePosition1 = animationPtr->keyFramePositions[currPosFrameidx];
		const KeyFramePosition& keyFramePosition2 = animationPtr->keyFramePositions[nextPosFrameidx];

		// Look into glm's easing functions
		// 1. Ease-In
		// 2. Ease-Out
		// 3. Ease-InOut
		// glm::sineEaseIn()


		// As the animation time 
		// How would I change this to implement glm::sineEaseIn(x)?
		float positionFraction = (animationPtr->currentTime - keyFramePosition1.time) / (keyFramePosition2.time - keyFramePosition1.time);

		switch (keyFramePosition2.easingType)
		{
		case EaseIn:
			positionFraction = glm::sineEaseIn(positionFraction);
			currentEntityPtr->colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case EaseOut:
			positionFraction = glm::sineEaseOut(positionFraction);
			currentEntityPtr->colour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case EaseInOut:
			positionFraction = glm::sineEaseInOut(positionFraction);
			currentEntityPtr->colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		default:
			currentEntityPtr->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
		positionPtr/*->value*/ = keyFramePosition1.position + (keyFramePosition2.position - keyFramePosition1.position) * positionFraction;
		// End of position animation



		/* UPDATE SCALES*/

		if (animationPtr->keyFrameScales.size() == 1)
		{
			// We only need to set the position of the only KeyFrame.
			scalePtr/*->value*/ = animationPtr->keyFrameScales[0].scale;
			return;
		}

		int currScaleFrameidx = FindKeyFrameScaleIndex(animationPtr, animationPtr->currentTime);

		// If we are at the last KeyFrame, only use that KeyFrame.
		if (currScaleFrameidx == animationPtr->keyFrameScales.size() - 1)
		{
			// We only need to set the position of the last KeyFrame.
			scalePtr/*->value*/ = animationPtr->keyFrameScales[currScaleFrameidx].scale;
			return;
		}

		int nextScaleFrameidx = currScaleFrameidx + 1;

		const KeyFrameScale& keyFrameScale1 = animationPtr->keyFrameScales[currScaleFrameidx];
		const KeyFrameScale& keyFrameScale2 = animationPtr->keyFrameScales[nextScaleFrameidx];

		float scaleFraction = (animationPtr->currentTime - keyFrameScale1.time) / (keyFrameScale2.time - keyFrameScale1.time);

		switch (keyFrameScale2.easingType)
		{
		case EaseIn:
			scaleFraction = glm::sineEaseIn(scaleFraction);
			currentEntityPtr->colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case EaseOut:
			scaleFraction = glm::sineEaseOut(scaleFraction);
			currentEntityPtr->colour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case EaseInOut:
			scaleFraction = glm::sineEaseInOut(scaleFraction);
			currentEntityPtr->colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		default:
			currentEntityPtr->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}
		scalePtr/*->value*/ = keyFrameScale1.scale + (keyFrameScale2.scale - keyFrameScale1.scale) * scaleFraction;



		/* UPDATE ROTATIONS*/

		if (animationPtr->keyFrameRotations.size() == 1)
		{
			// We only need to set the position of the only KeyFrame.
			rotationPtr/*->value*/ = animationPtr->keyFrameRotations[0].rotation;
			return;
		}

		int currRotFrameidx = FindKeyFrameRotationIndex(animationPtr, animationPtr->currentTime);

		// If we are at the last KeyFrame, only use that KeyFrame.
		if (currRotFrameidx == animationPtr->keyFrameRotations.size() - 1)
		{
			// We only need to set the position of the last KeyFrame.
			rotationPtr/*->value*/ = animationPtr->keyFrameRotations[currRotFrameidx].rotation;
			return;
		}

		int nextRotFrameidx = currRotFrameidx + 1;

		const KeyFrameRotation& keyFrameRot1 = animationPtr->keyFrameRotations[currRotFrameidx];
		const KeyFrameRotation& keyFrameRot2 = animationPtr->keyFrameRotations[nextRotFrameidx];

		float rotationFraction = (animationPtr->currentTime - keyFrameRot1.time) / (keyFrameRot2.time - keyFrameRot1.time);

		switch (keyFrameRot2.easingType)
		{
		case EaseIn:
			rotationFraction = glm::sineEaseIn(rotationFraction);
			currentEntityPtr->colour = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case EaseOut:
			rotationFraction = glm::sineEaseInOut(rotationFraction);
			currentEntityPtr->colour = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case EaseInOut:
			rotationFraction = glm::sineEaseInOut(rotationFraction);
			currentEntityPtr->colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		default:
			currentEntityPtr->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		}

		if (keyFrameRot2.InterpolationType == 1)
		{
			rotationPtr/*->value*/ = glm::lerp(keyFrameRot1.rotation, keyFrameRot2.rotation, rotationFraction);
		}
		if (keyFrameRot2.InterpolationType == 2)
		{
			rotationPtr/*->value*/ = glm::slerp(keyFrameRot1.rotation, keyFrameRot2.rotation, rotationFraction);
		}
		else
		{
			rotationPtr/*->value*/ = keyFrameRot1.rotation + (keyFrameRot2.rotation - keyFrameRot1.rotation) * rotationFraction;
		}

		currentEntityPtr->position = positionPtr;
		currentEntityPtr->rotation = glm::eulerAngles(rotationPtr);
		currentEntityPtr->scale = scalePtr;

		for (unsigned int index = 0; index != currentEntityPtr->GetChildren().size(); index++)
		{
			::g_pAnimationSystem->Process(currentEntityPtr->GetChildren(), dt);
		}

	}
}


int AnimationSystem::FindKeyFramePositionIndex(Animation* animation, float time)
{
	int keyFramePositionIndex = 1;
	for (; keyFramePositionIndex < animation->keyFramePositions.size(); ++keyFramePositionIndex)
	{
		if (animation->keyFramePositions[keyFramePositionIndex].time > time)
			return keyFramePositionIndex - 1;
	}

	return animation->keyFramePositions.size() - 1;
}

int AnimationSystem::FindKeyFrameScaleIndex(Animation* animation, float time)
{
	int keyFrameIndex = 1;
	for (; keyFrameIndex < animation->keyFrameScales.size(); ++keyFrameIndex)
	{
		if (animation->keyFrameScales[keyFrameIndex].time > time)
			return keyFrameIndex - 1;
	}

	return animation->keyFrameScales.size() - 1;
}

int AnimationSystem::FindKeyFrameRotationIndex(Animation* animation, float time)
{
	int keyFrameIndex = 1;
	for (; keyFrameIndex < animation->keyFrameRotations.size(); ++keyFrameIndex)
	{
		if (animation->keyFrameRotations[keyFrameIndex].time > time)
			return keyFrameIndex - 1;
	}

	return animation->keyFrameRotations.size() - 1;
}