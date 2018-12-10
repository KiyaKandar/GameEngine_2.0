#pragma once

#include "../Utilities/Maths/Matrix4.h"
#include "../Utilities/Maths/Vector3.h"
#include "../Utilities/Maths/Vector4.h"
#include "../Meshes/Mesh.h"
#include <vector>

class GameObject;

class SceneNode
{
public:
	SceneNode(string meshFile, NCLVector4 colour = NCLVector4(1, 1, 1, 1));
	SceneNode(Mesh* mesh, NCLVector4 colour = NCLVector4(1, 1, 1, 1));
	virtual ~SceneNode(void);

	void setParent(GameObject* parentObject)
	{
		this->parentObject = parentObject;
	}

	GameObject* getParent()
	{
		return parentObject;
	}

	void setPosition(NCLVector3 position)
	{
		transform.setPositionVector(position);
	}

	void  SetTransform(const NCLMatrix4 &matrix)
	{
		transform = matrix;
	}

	void	SetTransform(NCLVector3 pos) 
	{
		transform.setPositionVector(pos);
	}

	Mesh* GetMesh()
	{
		return mesh;
	}

	NCLMatrix4 GetTransform()
	{
		return transform;
	}
	NCLMatrix4 GetWorldTransform() const
	{
		return worldTransform;
	}

	void SetColour(NCLVector4 c)
	{
		this->colour = c;
		mesh->SetbackupColourAttributeForAllSubMeshes(c);
	}

	NCLVector4 getColour();

	void SetModelScale(NCLVector3 s)
	{
		boundingRadius *= s.length();
		transform.setScalingVector(s);
	}

	float GetCameraDistance() const
	{
		return distanceFromCamera;
	}
	void	SetCameraDistance(float f) { distanceFromCamera = f; }

	void AddChild(SceneNode* s);
	void RemoveChild(SceneNode* s);

	virtual void Update(float msec);
	virtual void Draw(Shader& shader);
	virtual void DrawShadow(Shader& shader);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera)
			? true : false;
	}

	void setEnabled(bool isEnabled)
	{
		this->isEnabled = isEnabled;
	}

	void takeLocalCopyOfMeshAnimations();

	std::vector<SceneNode*> getChildren();

	const float getRadius();

	bool isPaintSurface = false;
	int hasTexture = 0;
	bool isEnabled = true;
	NCLVector4 axisAngleRotation;

	bool isReflective = false;
	float reflectiveStrength = 0.0f;

protected:
	SceneNode*	parent;
	Mesh*		mesh;
	NCLMatrix4		worldTransform;
	NCLMatrix4		transform;
	NCLVector4		colour;

	float		distanceFromCamera;
	float		boundingRadius;

	std::vector<SceneNode*> children;
	GameObject* parentObject;

	vector<BoneInfo> boneInfo;
};

