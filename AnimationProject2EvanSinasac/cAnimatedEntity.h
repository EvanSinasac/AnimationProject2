#pragma once

#include "cEntity.h"
#include "Animation.h"
#include "cMesh.h"
#include <vector>
#include <string>

// Animated Entity inherits from Entity (which just has an update function)
// contains a vector of animations that can be played and changed using various controls
// the entity contains all it's relevant information and passes this information to it's mesh
// when update is called.  Update also goes through children and calls update on them as well
class cAnimatedEntity : public cEntity
{
public:
	cAnimatedEntity();

	virtual ~cAnimatedEntity();
	virtual void Update();

	void SetParent(cAnimatedEntity* entity);
	void RemoveChild(cAnimatedEntity* entity);
	void AddChild(cAnimatedEntity* entity);
	std::vector<cAnimatedEntity*>& GetChildren(void);

	unsigned int id;	//friendly entity
	std::string name;	//friendly name
	bool isActive;
	cAnimatedEntity* currentParent;

	// Animation stuff
	// lukgus has this all templated as components, but I don't think I want to handle positions and stuff like that
	// but idk, it's probably a better system for future stuff
	std::vector<Animation*> vecAnimations;
	cMesh* m_Mesh;
	Animation* AddAnimation();
	int animationIndex;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec3 colour;
protected:

private:
	std::vector<cAnimatedEntity*> m_Children;

};