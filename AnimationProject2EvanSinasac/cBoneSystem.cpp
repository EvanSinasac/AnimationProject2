#include "cBoneSystem.h"
#include "GLMCommon.h"
#include "globalThings.h"

// Mostly just what we did in class

void cBoneSystem::Process(const std::vector<cBoneEntity*>& entities, float dt)
{
	BoneHierarchy* boneHierarchy;

	for (unsigned int index = 0; index != entities.size(); index++)
	{
		boneHierarchy = entities[index]->p_mBoneHierarchy;

		if (boneHierarchy == 0)
			continue;

		boneHierarchy->time += dt;

		if (boneHierarchy->time >= 5.0f)
		{
			boneHierarchy->time = 0.0f;
		}

		UpdateHierarchy(boneHierarchy);
	}
}

void cBoneSystem::UpdateHierarchy(BoneHierarchy* boneHierarchy)
{
	// 0 -> 90 over 5 seconds
	// quat(1, 0, 0, 0) -> quat(0.7071, 0.0f, 0.0f, 0.7071) over 5 seconds
	// 
	//glm::quat start = glm::quat(1.f, 0.f, 0.f, 0.f);
	glm::quat start = glm::quat(1.f, 0.f, 0.f, 0.f);
	glm::quat end = glm::quat(1.0f, 0.f, 0.f, 1.0f);

	float rotationFraction = boneHierarchy->time / 5.f;
	glm::quat currentRotation = glm::slerp(start, end, rotationFraction);

	glm::vec3 startPos = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 endPos = glm::vec3(-5.0f, 1.0f, 0.0f);
	float positionFraction = boneHierarchy->time / 5.0f;
	//glm::mat4 currentPos = glm::translate(startPos, endPos, positionFraction);
	//glm::mat4 currentPos = glm::translate(glm::mat4(1.0f), endPos);
	glm::vec3 currentPos= startPos + (endPos * positionFraction);
	//currentPos /= positionFraction;

	glm::vec3 startScale = glm::vec3(1.0f);
	glm::vec3 endScale = glm::vec3(1.2f);
	float scaleFraction = boneHierarchy->time / 5.0f;
	//glm::mat4 currentScale = glm::scale(glm::mat4(1.0f), endScale);
	glm::vec3 currentScale = startScale + (endScale * scaleFraction);
	//currentScale /= scaleFraction;

	for (int i = 0; i < boneHierarchy->currentTransforms.size(); ++i)
	{
		//boneHierarchy->currentTransforms[i].value = glm::toMat4(currentRotation);
		//boneHierarchy->currentTransforms[i].value = currentPos;
		//boneHierarchy->bones[i]->SetRotation(currentRotation);
		//boneHierarchy->currentTransforms[i].value = currentScale;

		//boneHierarchy->currentTransforms[i].position = currentPos;
		//boneHierarchy->currentTransforms[i].rotation = currentRotation;
		//boneHierarchy->currentTransforms[i].scale = currentScale;
		if (::g_BoneAnimation == 1)
		{	// rotation animation
			boneHierarchy->currentTransforms[i].position = glm::vec3(0.0f, 3.0f * i, 0.0f);
			boneHierarchy->currentTransforms[i].rotation = currentRotation;
			boneHierarchy->currentTransforms[i].scale = glm::vec3(1.0f);
		}
		else if (::g_BoneAnimation == 2)
		{	// position animation
			boneHierarchy->currentTransforms[i].position = currentPos;
			boneHierarchy->currentTransforms[i].rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			boneHierarchy->currentTransforms[i].scale = glm::vec3(1.0f);
		}
		else if (::g_BoneAnimation == 3)
		{	// scale animation
			boneHierarchy->currentTransforms[i].position = glm::vec3(0.0f, 3.0f * i, 0.0f);
			boneHierarchy->currentTransforms[i].rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			boneHierarchy->currentTransforms[i].scale = currentScale;
		}
		else if (::g_BoneAnimation == 4)
		{	// position & rotation
			boneHierarchy->currentTransforms[i].position = currentPos;
			boneHierarchy->currentTransforms[i].rotation = currentRotation;
			boneHierarchy->currentTransforms[i].scale = glm::vec3(1.0f);
		}
		else if (::g_BoneAnimation == 5)
		{	// all three
			boneHierarchy->currentTransforms[i].position = currentPos;
			boneHierarchy->currentTransforms[i].rotation = currentRotation;
			boneHierarchy->currentTransforms[i].scale = currentScale;
		}
		else
		{
			boneHierarchy->currentTransforms[i].position = glm::vec3(0.0f, 3.0f * i, 0.0f);
			boneHierarchy->currentTransforms[i].rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			boneHierarchy->currentTransforms[i].scale = glm::vec3(1.0f);
		}

	}

	UpdateBone(boneHierarchy, boneHierarchy->rootBone, glm::mat4(1.f));
}

void cBoneSystem::UpdateBone(BoneHierarchy* boneHierarchy, Bone* bone,
	const glm::mat4 parentModelMatrix)
{
	Transform& transform = boneHierarchy->currentTransforms[bone->index];

	bone->SetPosition(transform.position);
	bone->SetScale(transform.scale);
	bone->SetRotation(transform.rotation);

	glm::mat4 TranslationMatrix = glm::translate(parentModelMatrix, transform.position);
	glm::mat4 RotationMatrix = glm::mat4_cast(transform.rotation);
	glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), transform.scale);

	glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;

	//glm::mat4 modelMatrix = parentModelMatrix * transform.value;
	//glm::mat4 modelMatrix = parentModelMatrix * ModelMatrix;
	//bone->SetModelMatrix(modelMatrix);
	bone->SetModelMatrix(ModelMatrix);

	std::vector<Bone*>& children = bone->GetChildren();
	for (int i = 0; i < children.size(); ++i)
	{
		UpdateBone(boneHierarchy, children[i], ModelMatrix);
	}
}