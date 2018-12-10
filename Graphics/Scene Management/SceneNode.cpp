#include "SceneNode.h"

#include "../../Gameplay/GameObject.h"

SceneNode::SceneNode(string meshFile, NCLVector4 colour)
{
	this->mesh = new Mesh(meshFile, 1);
	this->colour = colour;
	parent = NULL;

	boundingRadius = 10.0f;
	this->mesh->radius = boundingRadius;
	distanceFromCamera = 0.0f;
}

SceneNode::SceneNode(Mesh * mesh, NCLVector4 colour)
{
	this->mesh = mesh;
	this->colour = colour;
	parent = NULL;

	boundingRadius = mesh->getRadius();
	distanceFromCamera = 0.0f;
}

SceneNode::~SceneNode(void)
{
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

NCLVector4 SceneNode::getColour()
{
	return colour;
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
}

void SceneNode::RemoveChild(SceneNode* s) {
	children.erase(std::remove(children.begin(),
		children.end(), s), children.end());
}

/*
* Advanced rendering can be performed here per SceneNode
* Such as disabling depth testing or reuploading data
*/
void SceneNode::Draw(Shader& shader) {
	if (mesh) {
		for (SubMesh* submesh : mesh->meshes)
		{
			submesh->Draw(shader, worldTransform);
		}
	}
}

void SceneNode::DrawShadow(Shader& shader)
{
	if (mesh) {
		for (SubMesh* submesh : mesh->meshes)
		{
			submesh->DrawShadow(shader, worldTransform);
		}
	}
}

void SceneNode::takeLocalCopyOfMeshAnimations()
{
	boneInfo = mesh->boneInfo;
	std::vector<const aiScene*>& importedAnimations = mesh->importedAnimations;
	for (int i = 0; i < importedAnimations.size(); ++i)
	{
		AnimationPlayer::getAnimationService()->addAnimation(mesh->animationFiles[i], parentObject->getName(), mesh,
			importedAnimations[i]->mAnimations[0], mesh->meshScene->mRootNode, mesh->globalInverseTransform, &boneInfo);
	}
}

std::vector<SceneNode*> SceneNode::getChildren()
{
	return children;
}

const float SceneNode::getRadius()
{
	return boundingRadius;
}

void SceneNode::Update(float msec) {
	if (parent) {
		worldTransform = parent->worldTransform * transform;
	}
	else {
		worldTransform = transform;
	}

	mesh->SetTransformForAllSubMeshes(worldTransform);

	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(msec);
	}
}
