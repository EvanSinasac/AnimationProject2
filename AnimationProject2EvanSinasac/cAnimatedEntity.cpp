#include "cAnimatedEntity.h"

cAnimatedEntity::cAnimatedEntity()
	: isActive(true)
	, currentParent(0)
{
	animationIndex = 0;
	// I think I'll initialize the mesh in whatever function I make to build the entity
}

cAnimatedEntity::~cAnimatedEntity()
{

}

// update the mesh and children
void cAnimatedEntity::Update()
{
	this->m_Mesh->positionXYZ = position;
	this->m_Mesh->orientationXYZ = rotation;
	this->m_Mesh->scale = scale;
	this->m_Mesh->wholeObjectDiffuseRGBA = glm::vec4(colour, 1.0f);
	for (unsigned int index = 0; index != m_Children.size(); index++)
	{
		m_Children[index]->Update();
	}
}

std::vector<cAnimatedEntity*>& cAnimatedEntity::GetChildren()
{
	return m_Children;
}

void cAnimatedEntity::SetParent(cAnimatedEntity* parent)
{
	if (currentParent != 0)
	{
		currentParent->RemoveChild(this);
		currentParent = 0;
	}

	currentParent = parent;
	parent->AddChild(this);
}

void cAnimatedEntity::AddChild(cAnimatedEntity* child)
{
	std::vector<cAnimatedEntity*>::iterator itChildren = std::find(m_Children.begin(), m_Children.end(), child);
	if (itChildren != m_Children.end())
		return;
	
	m_Children.push_back(child);
	this->m_Mesh->vec_pChildMeshes.push_back(child->m_Mesh);	// important for drawing the entities mesh
}

void cAnimatedEntity::RemoveChild(cAnimatedEntity* child)
{
	std::vector<cAnimatedEntity*>::iterator itChildren = std::find(m_Children.begin(), m_Children.end(), child);
	if (itChildren != m_Children.end())
		return;
	std::remove(m_Children.begin(), m_Children.end(), child);
	std::remove(m_Mesh->vec_pChildMeshes.begin(), m_Mesh->vec_pChildMeshes.end(), child->m_Mesh);
}

Animation* cAnimatedEntity::AddAnimation()
{
	Animation* newAnimation = new Animation();

	vecAnimations.push_back(newAnimation);

	return newAnimation;
}