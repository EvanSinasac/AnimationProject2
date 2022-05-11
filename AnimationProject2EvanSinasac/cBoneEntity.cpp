#include "cBoneEntity.h"

cBoneEntity::cBoneEntity()
	: cEntity()
    , isActive(true)
	, currentParent(0)
{
	this->p_mBoneHierarchy = new BoneHierarchy();
	this->p_mBoneHierarchy->time = 0;
}

cBoneEntity::~cBoneEntity()
{

}

void cBoneEntity::Update()
{
	this->m_Mesh->matModel = this->p_mBoneHierarchy->rootBone->GetModelMatrix();
    this->m_Mesh->positionXYZ = this->p_mBoneHierarchy->rootBone->GetPosition();
    this->m_Mesh->orientationXYZ = glm::eulerAngles(this->p_mBoneHierarchy->rootBone->GetRotation());
    this->m_Mesh->scale = this->p_mBoneHierarchy->rootBone->GetScale();
	// honestly, idk what to actually update here, but it's virtual so I need something to be safe
	for (unsigned int index = 0; index != this->m_Children.size(); index++)
	{
		this->m_Children[index]->Update();	// at least call update on the children, (again nothing happening tho)
	}
}

std::vector<cBoneEntity*>& cBoneEntity::GetChildren()
{
	return this->m_Children;
}

void cBoneEntity::SetParent(cBoneEntity* parent)
{
	if (currentParent != 0)
	{
		currentParent->RemoveChild(this);
		currentParent = 0;
	}

	currentParent = parent;
	parent->AddChild(this);
}

void cBoneEntity::AddChild(cBoneEntity* child)
{
	std::vector<cBoneEntity*>::iterator itChildren = std::find(m_Children.begin(), m_Children.end(), child);
	if (itChildren != m_Children.end())
		return;

	m_Children.push_back(child);
	this->m_Mesh->vec_pChildMeshes.push_back(this->m_Mesh);
}

void cBoneEntity::RemoveChild(cBoneEntity* child)
{
	std::vector<cBoneEntity*>::iterator itChildren = std::find(m_Children.begin(), m_Children.end(), child);
	if (itChildren != m_Children.end())
		return;

	std::remove(m_Children.begin(), m_Children.end(), child);
	std::remove(m_Mesh->vec_pChildMeshes.begin(), m_Mesh->vec_pChildMeshes.end(), child->m_Mesh);
}