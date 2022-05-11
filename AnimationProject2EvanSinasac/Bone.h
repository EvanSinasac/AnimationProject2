#pragma once

#include <vector>
#include <string>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

class Bone
{
public:
	Bone(const glm::vec3& position, const glm::vec3& scale, const glm::quat& rotation)
		: m_Position(position)
		, m_Scale(scale)
		, m_Rotation(rotation)
		, m_Parent(0)
	{}

	Bone()
		: Bone(glm::vec3(0.f)
			, glm::vec3(1.f)
			, glm::quat(1.f, 0.f, 0.f, 0.f))
	{}

	virtual ~Bone()
	{}

	const glm::vec3& GetPosition() { return m_Position; }
	void SetPosition(const glm::vec3& position) { m_Position = position; }

	const glm::vec3& GetScale() { return m_Scale; }
	void SetScale(const glm::vec3& scale) { m_Scale = scale; }

	const glm::quat& GetRotation() { return m_Rotation; }
	void SetRotation(const glm::quat& rotation) { m_Rotation = rotation; }

	const glm::mat4& GetModelMatrix() { return m_ModelMatrix; }
	void SetModelMatrix(const glm::mat4& modelMatrix) { m_ModelMatrix = modelMatrix; }

	Bone* GetParent() { return m_Parent; }
	void SetParent(Bone* parent)
	{
		// TODO: Make sure the parent is not a child
		m_Parent = parent;
	}

	void AddChild(Bone* bone)
	{
		// TODO: Need to check if the bone is already a child
		// TODO: Need to check if the bone is the parent
		m_Children.push_back(bone);
	}

	std::vector<Bone*>& GetChildren() { return m_Children; }

	std::string name;

	int index;

private:
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::quat m_Rotation;
	glm::mat4 m_ModelMatrix;

	Bone* m_Parent;
	std::vector<Bone*> m_Children;
};