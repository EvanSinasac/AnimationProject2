#pragma once

#include "cEntity.h"
#include "cMesh.h"
#include "BoneHierarchy.h"
#include <vector>
#include <string>

class cBoneEntity : public cEntity
{
public:
	cBoneEntity();

	virtual ~cBoneEntity();
	virtual void Update();

	void SetParent(cBoneEntity* entity);
	void RemoveChild(cBoneEntity* entity);
	void AddChild(cBoneEntity* entity);
	std::vector<cBoneEntity*>& GetChildren(void);

	unsigned int id;	//friendly entity
	std::string name;	//friendly name
	bool isActive;
	cBoneEntity* currentParent;

	// Bone Stuff
	// So this is where I set the bone stuff?
	cMesh* m_Mesh;
	BoneHierarchy* p_mBoneHierarchy;		// Use the Bone Hierarchy from class on the cBoneEntity, and we'll call from this when drawing the entity
protected:

private:
	std::vector<cBoneEntity*> m_Children;

};