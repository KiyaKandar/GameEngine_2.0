#include "ProfilerModule.h"

#include "../../GraphicsCommon.h"
#include "../Resource Management/Database/Database.h"
#include "../../../Profiler/ProfilerDisplay.h"
#include "../../../Profiler/Profiler.h"
#include "../../../Profiler/ProfilerService.h"

const float PROFILER_INDENT;
const float PROFILER_NEWLINE;

ProfilerModule::ProfilerModule(const std::string identifier, const Matrix4 projMatrix, const Vector2 resolution,
	Database* database) : GraphicsModule(identifier, projMatrix, resolution)
{
	font = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	ProfilerTextShader = new Shader(SHADERDIR"UITextVertex.glsl", SHADERDIR"UITextFrag.glsl");

	this->database = database;
}

ProfilerModule::~ProfilerModule()
{
}

void ProfilerModule::initialise()
{
}

void ProfilerModule::apply()
{
	ProfilerObjects = ProfilerDisplay::getInterface()->getAllTextInProfiler();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	viewMatrix.toIdentity();
	textureMatrix.toIdentity();


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	setCurrentShader(ProfilerTextShader);
	updateShaderMatrices();

	Vector2 offsetPosition = startPosition;
	for each (ProfilerText profilerInfo in *ProfilerObjects)
	{
		std::string profilerText = profilerInfo.info + ": " + profilerInfo.value;
		TextMesh profileMesh(profilerText, *profilerInfo.font);
		Vector2 buttonSize = profilerInfo.scale;
		profileMesh.Draw(*currentShader, Matrix4::translation(offsetPosition) * Matrix4::scale({ buttonSize.x,buttonSize.y,1 }));
			if (ProfilerDisplay::getInterface()->getChildrenDisplaying())
			{
				Vector2 childPosition = offsetPosition + Vector2(PROFILER_INDENT, 0);
				for (int i = 0; i < profilerInfo.profilertextChildren.size(); ++i)
				{
					profilerText = profilerInfo.profilertextChildren[i].info + ": " + profilerInfo.profilertextChildren[i].value;
					childPosition = childPosition - Vector2(0, PROFILER_NEWLINE);
					TextMesh childMesh(profilerText, *profilerInfo.font);
					childMesh.Draw(*currentShader, Matrix4::translation(childPosition) * Matrix4::scale({ buttonSize.x,buttonSize.y,1 }));
					offsetPosition = Vector2(startPosition.x, childPosition.y);

					Vector2 grandChildPosition = childPosition + Vector2(PROFILER_INDENT, 0);
					for (int j = 0; j < profilerInfo.profilertextChildren[i].profilertextChildren.size(); ++j)
					{
						profilerText = profilerInfo.profilertextChildren[i].profilertextChildren[j].info + ": " + profilerInfo.profilertextChildren[i].profilertextChildren[j].value;
						grandChildPosition = grandChildPosition - Vector2(0, PROFILER_NEWLINE);
						TextMesh grandChildMesh(profilerText, *profilerInfo.font);
						grandChildMesh.Draw(*currentShader, Matrix4::translation(grandChildPosition) * Matrix4::scale({ buttonSize.x,buttonSize.y,1 }));
						offsetPosition = Vector2(startPosition.x, grandChildPosition.y);
					}
				}
			}
			
		offsetPosition = offsetPosition - Vector2(0, 25);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void ProfilerModule::linkShaders()
{
	ProfilerTextShader->LinkProgram();
}

void ProfilerModule::regenerateShaders()
{
	ProfilerTextShader->Regenerate();
}

void ProfilerModule::locateUniforms()
{
}
