//Evan Sinasac - 1081418
//INFO6045 Animation Project 2
//main.cpp description:
//					The main purpose of Project 2 is to demonstrate a model being animated through a bone elements
//					and affecting vertices in the shader




// Particle physics (namespace nPhysics)
#include <physics/cParticleWorld.h>
#include <physics/random_helpers.h>
#include <physics/particle_contact_generators.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "GLCommon.h"
#include "GLMCommon.h"
#include "globalThings.h"


// Animation
#include "cAnimatedEntity.h"

#include <glm/gtx/quaternion.hpp>


//Globals;
float lastX = 600.f;
float lastY = 320.f;
bool firstMouse = true;
float cameraYaw = 90.f;
float cameraPitch = 0.f;

double deltaTime = 0.0;
double lastFrame = 0.0;

int numberOfTransparentObjects = 0;


std::vector<std::string> modelLocations;


//Function signature for DrawObject()
void DrawObject(
	cMesh* pCurrentMesh,
	glm::mat4 matModel,
	GLint matModel_Location,
	GLint matModelInverseTranspose_Location,
	GLuint program,
	cVAOManager* pVAOManager);

void DrawBoneObject(/*cMesh* pCurrentMesh*/cBoneEntity* pCurrentEntity, glm::mat4 matModel,
	GLint matModel_Location,
	GLint matModelInverseTranspose_Location,
	GLuint program,
	cVAOManager* pVAOManager);

void MakeAnimatedEntites();
void MakeBoneAnimationEntities();

//callbacks
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

bool loadWorldFile();
bool loadLightsFile();


int main(int argc, char** argv)
{
	GLFWwindow* pWindow;

	GLuint program = 0;		//0 means no shader program
	//GLuint boneProgram = 0;

	GLint mvp_location = -1;
	std::stringstream ss;


	glfwSetErrorCallback(GLFW_error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	pWindow = glfwCreateWindow(1200, 640, "Animation Project 2", NULL, NULL);

	if (!pWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(pWindow, GLFW_key_callback);

	glfwSetCursorEnterCallback(pWindow, GLFW_cursor_enter_callback);
	glfwSetCursorPosCallback(pWindow, GLFW_cursor_position_callback);
	glfwSetScrollCallback(pWindow, GLFW_scroll_callback);
	glfwSetMouseButtonCallback(pWindow, GLFW_mouse_button_callback);
	glfwSetWindowSizeCallback(pWindow, GLFW_window_size_callback);

	
	glfwSetCursorPosCallback(pWindow, mouse_callback);

	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(pWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


	GLint max_uniform_location = 0;
	GLint* p_max_uniform_location = NULL;
	p_max_uniform_location = &max_uniform_location;
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, p_max_uniform_location);

	std::cout << "GL_MAX_UNIFORM_LOCATIONS: " << max_uniform_location << std::endl;

	// Create global things
	::g_StartUp(pWindow);

	
	// Shaders with textures and lights, probably going to disable lights unless it looks OK

	// Lukas used a different bone shader for the vertex and fragment shaders, if I copy over, replaces these
	// actually, 100%, part of the project description is to assign vertices to a bone when loading the mesh ( probably a change in the model loading code)
	// and to change the shader so that it is able to accept at least 1 bone id per vertex
	// where the shader is able to modify the rotation scale and position of the bone

	// welp, no harm in trying and seeing what happens, right?
	cShaderManager::cShader vertShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\vertShader_01.glsl";
	vertShader.fileName = ss.str();

	cShaderManager::cShader geomShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\geomShader.glsl";
	geomShader.fileName = ss.str();;

	cShaderManager::cShader fragShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\fragShader_01.glsl";
	fragShader.fileName = ss.str();

	if (::g_pShaderManager->createProgramFromFile("Shader#1", vertShader, geomShader, fragShader))
	{
		std::cout << "Shader compiled OK" << std::endl;
		// 
		// Set the "program" variable to the one the Shader Manager used...
		program = ::g_pShaderManager->getIDFromFriendlyName("Shader#1");
	}
	else
	{
		std::cout << "Error making shader program: " << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
	}

	// Animation Project 2, Bone Shader
	//cShaderManager::cShader boneVertShader;
	//ss.str("");
	////ss << SOLUTION_DIR << "common\\assets\\shaders\\BoneShader.vertex.glsl";
	//ss << SOLUTION_DIR << "common\\assets\\shaders\\TentacleShader.vertex.glsl";
	//boneVertShader.fileName = ss.str();

	//cShaderManager::cShader boneFragShader;
	//ss.str("");
	////ss << SOLUTION_DIR << "common\\assets\\shaders\\BoneShader.fragment.glsl";
	//ss << SOLUTION_DIR << "common\\assets\\shaders\\TentacleShader.fragment.glsl";
	//boneFragShader.fileName = ss.str();

	//if (::g_pShaderManager->createProgramFromFile("BoneShader", boneVertShader, boneFragShader))
	//{
	//	std::cout << "Bone Shader compiled OK" << std::endl;
	//	// 
	//	// Set the "program" variable to the one the Shader Manager used...
	//	boneProgram = ::g_pShaderManager->getIDFromFriendlyName("Shader#1");
	//}
	//else
	//{
	//	std::cout << "Error making shader program: " << std::endl;
	//	std::cout << ::g_pShaderManager->getLastError() << std::endl;
	//}
	// ok, now I've got two shaders, gunna use the BoneShader when drawing the bone object

	// Select the shader program we want to use
	// (Note we only have one shader program at this point)
	glUseProgram(program);

	// TODO (Eventually): Go make this a function and set all the uniform locations at once so I never have to look
	// for them again and can just call the pShaderProc whenever I need to set something
	// *******************************************************
	// Now, I'm going to "load up" all the uniform locations
	// (This was to show how a map could be used)
	// Stuff we've started doing in graphics to make things faster 
	cShaderManager::cShaderProgram* pShaderProc = ::g_pShaderManager->pGetShaderProgramFromFriendlyName("Shader#1");
	int theUniformIDLoc = -1;
	theUniformIDLoc = glGetUniformLocation(program, "matModel");

	pShaderProc->mapUniformName_to_UniformLocation["matModel"] = theUniformIDLoc;

	// Or...
	pShaderProc->mapUniformName_to_UniformLocation["matModel"] = glGetUniformLocation(program, "matModel");

	pShaderProc->mapUniformName_to_UniformLocation["matView"] = glGetUniformLocation(program, "matView");
	pShaderProc->mapUniformName_to_UniformLocation["matProjection"] = glGetUniformLocation(program, "matProjection");
	pShaderProc->mapUniformName_to_UniformLocation["matModelInverseTranspose"] = glGetUniformLocation(program, "matModelInverseTranspose");

	// *******************************************************

	//GLint mvp_location = -1;
	mvp_location = glGetUniformLocation(program, "MVP");

	// Get "uniform locations" (aka the registers these are in)
	GLint matModel_Location = glGetUniformLocation(program, "matModel");
	//	GLint matView_Location = glGetUniformLocation(program, "matView");
	GLint matProjection_Location = glGetUniformLocation(program, "matProjection");
	GLint matModelInverseTranspose_Location = glGetUniformLocation(program, "matModelInverseTranspose");

	GLint bDiscardTransparencyWindowsON_LocID = glGetUniformLocation(program, "bDiscardTransparencyWindowsOn");

	// ***************************************************************************************************************
	// Gunna grab the locations we'll need like above
	//cShaderManager::cShaderProgram* pBoneShaderProc = ::g_pShaderManager->pGetShaderProgramFromFriendlyName("BoneShader");
	//int boneUniformID = -1;

	//pBoneShaderProc->mapUniformName_to_UniformLocation["ModelMatrix"] = glGetUniformLocation(boneProgram, "ModelMatrix");
	//pBoneShaderProc->mapUniformName_to_UniformLocation["RotationMatrix"] = glGetUniformLocation(boneProgram, "ModelMatrix");
	//for (unsigned int index = 0; index != 48; index++)
	//{
	//	ss.str("");
	//	ss << "BoneRotationMatrices[" << index << "]";
	//	pBoneShaderProc->mapUniformName_to_UniformLocation[ss.str().c_str()] = glGetUniformLocation(boneProgram, ss.str().c_str());
	//}
	//pBoneShaderProc->mapUniformName_to_UniformLocation["ViewMatrix"] = glGetUniformLocation(boneProgram, "ModelMatrix");
	//pBoneShaderProc->mapUniformName_to_UniformLocation["ProjectionMatrix"] = glGetUniformLocation(boneProgram, "ModelMatrix");

	//int numBones = 48;
	//for (int i = 0; i < numBones; i++)
	//{
	//	ss.str("");
	//	ss << "BoneMatrices[" << i << "]";
	//	pBoneShaderProc->mapUniformName_to_UniformLocation[ss.str()] = glGetUniformLocation(boneProgram, ss.str().c_str());
	//}

	//pBoneShaderProc->LoadUniformLocation("ShaderId"); // ?idk

	// **********************************************************************************************


	//Lights stuff here, this is just how to set light information, I'm keeping this around just so I have reference to it
	// but for the most part, the loadLightsFile will handle it from now on and I'll just make changes to the function
	// and the lights file

	//    	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
//	                // 0 = pointlight
//					// 1 = spot light
//					// 2 = directional light
	//::g_pTheLights->theLights[0].position = glm::vec4(0.0f, 110.0f, 0.0f, 1.0f);
	////::g_pTheLights->theLights[0].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	////::g_pTheLights->theLights[0].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_pTheLights->theLights[0].param1.x = 0.0f;	// Pointlight
	////::g_pTheLights->theLights[0].param1.y = 10.0f;   // Inner
	////::g_pTheLights->theLights[0].param1.z = 12.0f;   // Outer
	////::g_pTheLights->theLights[0].direction = glm::vec4(1.0f, -0.1f, 0.0f, 1.0f);
	//::g_pTheLights->theLights[0].atten.x = 1.0f;
	//::g_pTheLights->theLights[0].atten.y = 0.0000000000001f;
	//::g_pTheLights->theLights[0].atten.z = 0.00000000000001f;
	//::g_pTheLights->TurnOnLight(0);

	// loads all the lights from a file instead of individually setting them here
	if (loadLightsFile())
	{
		std::cout << "loadLightsFile finished ok!" << std::endl;
	}
	else
	{
		std::cout << "loadLightsFile did NOT finish ok!  Aborting!" << std::endl;
		return -1;
	}

	// Get the uniform locations of the light shader values
	::g_pTheLights->SetUpUniformLocations(program);

	// Set the default file path for the models in the scene
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\models\\";

	::g_pVAOManager->setFilePath(ss.str());

	// Load ALL the models
	// Some projects I make a function and change what models are being made there, for this I'm not going to have that
	// many so we're just doing it here
	modelLocations.push_back("SM_Env_Floor_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Floor_04_xyz_n_rgba_uv.ply");
	modelLocations.push_back("Isosphere_Smooth_Normals.ply");
	modelLocations.push_back("ISO_Sphere_flat_4div_xyz_n_rgba_uv.ply");
	modelLocations.push_back("Turret_Simple_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply");
	modelLocations.push_back("4-tier-cone_xyz_n_rgba_uv.ply");									// used for the bone animation, not sure if I need the other models

	unsigned int totalVerticesLoaded = 0;
	unsigned int totalTrianglesLoaded = 0;
	for (std::vector<std::string>::iterator itModel = modelLocations.begin(); itModel != modelLocations.end(); itModel++)
	{
		sModelDrawInfo theModel;
		std::string modelName = *itModel;
		std::cout << "Loading " << modelName << "...";
		if (!::g_pVAOManager->LoadModelIntoVAO(modelName, theModel, program))
		{
			std::cout << "didn't work because: " << std::endl;
			std::cout << ::g_pVAOManager->getLastError(true) << std::endl;
		}
		else
		{
			std::cout << "OK." << std::endl;
			std::cout << "\t" << theModel.numberOfVertices << " vertices and " << theModel.numberOfTriangles << " triangles loaded." << std::endl;
			totalTrianglesLoaded += theModel.numberOfTriangles;
			totalVerticesLoaded += theModel.numberOfVertices;
		}
	} //end of for (std::vector<std::string>::iterator itModel

	std::cout << "Done loading models." << std::endl;
	std::cout << "Total vertices loaded = " << totalVerticesLoaded << std::endl;
	std::cout << "Total triangles loaded = " << totalTrianglesLoaded << std::endl;


	//std::cout << "I don't need to load EVERY model into the boneShader, just the 4-tier cone" << std::endl;
	//{
	//	sModelDrawInfo theModel;
	//	std::string modelName = modelLocations[modelLocations.size() - 1];
	//	std::cout << "Loading " << modelName << "...";
	//	if (!::g_pVAOManager->LoadModelIntoVAO(modelName, theModel, boneProgram))
	//	{
	//		std::cout << "didn't work because: " << std::endl;
	//		std::cout << ::g_pVAOManager->getLastError(true) << std::endl;
	//	}
	//	else
	//	{
	//		std::cout << "OK." << std::endl;
	//		std::cout << "\t" << theModel.numberOfVertices << " vertices and " << theModel.numberOfTriangles << " triangles loaded." << std::endl;
	//	}
	//	std::cout << "Loaded the 4-tier cone into the boneProgram" << std::endl;
	//}

	// Animation Project 1: Make the animated object here and keep it in some way that I can track it
	// Keeping this for future reference/rememberance, also I just don't like deleting code until I'm certain I don't need it
	//MakeAnimatedEntites();

	// Animation Project 2: Build off of project 1 and change to some bone animations
	MakeBoneAnimationEntities();

	//World file stuff here
	if (loadWorldFile())
	{
		std::cout << "loadWorldFile finished OK" << std::endl;
	}
	else
	{
		std::cout << "loadWorldFile did not finish OK, aborting" << std::endl;
		return -1;
	}

	
	// Transparent objects get made last (and sorted before being drawn)


	//Michael Feeney's BMP texture mapping, this is basically forever unchanged until I either learn or implement a different texture manager
	//// Load the textures
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\textures\\";
	::g_pTextureManager->SetBasePath(ss.str());

	if (::g_pTextureManager->Create2DTextureFromBMPFile("SpaceInteriors_Texture.bmp", true))
		//if (::g_pTextureManager->Create2DTextureFromBMPFile("Pebbleswithquarzite.bmp", true))
	{
		std::cout << "Loaded the texture" << std::endl;
	}
	else
	{
		std::cout << "DIDN'T load the texture" << std::endl;
	}

	::g_pTextureManager->Create2DTextureFromBMPFile("SpaceInteriors_Emmision.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("WorldMap.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("2k_jupiter.bmp", true);

	::g_pTextureManager->Create2DTextureFromBMPFile("Wall_Atlas_01_ID.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Wall_Atlas_03_ID.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Wall_Atlas_08_ID.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Wall_Atlas_11_ID.bmp", true);

	// Default don't have a texture
	if (::g_pTextureManager->Create2DTextureFromBMPFile("BrightColouredUVMap.bmp", true))
		//if (::g_pTextureManager->Create2DTextureFromBMPFile("Pebbleswithquarzite.bmp", true))
	{
		std::cout << "Loaded the texture" << std::endl;
	}
	else
	{
		std::cout << "DIDN'T load the texture" << std::endl;
	}

	// Add a skybox texture, same with these, this is just how we do skyboxes until I learn another way
	std::string errorTextString;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\textures\\cubemaps\\";
	::g_pTextureManager->SetBasePath(ss.str());		// update base path to cube texture location

	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("TropicalSunnyDay",
		"TropicalSunnyDayRight2048.bmp",	/* +X */	"TropicalSunnyDayLeft2048.bmp" /* -X */,
		"TropicalSunnyDayUp2048.bmp",		/* +Y */	"TropicalSunnyDayDown2048.bmp" /* -Y */,
		"TropicalSunnyDayFront2048.bmp",	/* +Z */	"TropicalSunnyDayBack2048.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}


	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("Space01",
		"SpaceBox_right1_posX.bmp",		/* +X */	"SpaceBox_left2_negX.bmp" /* -X */,
		"SpaceBox_top3_posY.bmp",		/* +Y */	"SpaceBox_bottom4_negY.bmp" /* -Y */,
		"SpaceBox_front5_posZ.bmp",		/* +Z */	"SpaceBox_back6_negZ.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("City01",
		"city_lf.bmp",		/* +X */	"city_rt.bmp" /* -X */,
		"city_dn.bmp",		/* +Y */	"city_up.bmp" /* -Y */,
		"city_ft.bmp",		/* +Z */	"city_bk.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("Skyrim",
		"Skyrim_Right.bmp",		/* +X */	"Skyrim_Left.bmp" /* -X */,
		"Skyrim_Top.bmp",		/* +Y */	"Skyrim_Bottom.bmp" /* -Y */,
		"Skyrim_Forward.bmp",		/* +Z */	"Skyrim_Back.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	// Make sphere for "skybox" before world file
	cMesh* sphereSky = new cMesh();
	//sphereSky->meshName = "Isosphere_Smooth_Normals.ply";
	// We are using a sphere with INWARD facing normals, so we see the "back" of the sphere
	sphereSky->meshName = "Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply";
	// 2 main ways we can do a skybox:
	//
	// - Make a sphere really big, so everything fits inside
	// (be careful of the far clipping plane)
	//
	// - Typical way is:
	//	- Turn off the depth test
	//	- Turn off the depth function (i.e. writing to the depth buffer)
	//	- Draw the skybox object (which can be really small, since it's not interacting with the depth buffer)
	//	- Once drawn:
	//		- Turn on the depth function
	//		- Turn on the depth test

	sphereSky->positionXYZ = ::cameraEye;	//glm::vec3(100.0f, -250.0f, 100.0f);
	//sphereSky->orientationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	sphereSky->scale = glm::vec3(10000.0f, 10000.0f, 10000.0f);
	//sphereSky->bUseWholeObjectDiffuseColour = false;
	//sphereSky->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	sphereSky->bDontLight = true;
	sphereSky->clearTextureRatiosToZero();
	//sphereSky->textureNames[1] = "2k_jupiter.bmp";
	sphereSky->textureRatios[1] = 1.0f;


	// Not using this camera for Physics 2, Project 1 
	// Also, I don't think this works anymore anyways and need to re do it lol
	// Face camera towards origin
	if (::cameraEye.x > 0 && ::cameraEye.z > 0)
	{
		::cameraYaw = 180.f + (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x > 0 && ::cameraEye.z < 0)
	{
		::cameraYaw = 90.f - (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x < 0 && ::cameraEye.z > 0)
	{
		::cameraYaw = (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x < 0 && ::cameraEye.z < 0)
	{
		::cameraYaw = (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x == 0.f)
	{
		if (::cameraEye.z >= 0.f)
		{
			::cameraYaw = 270.f;
		}
		else
		{
			::cameraYaw = 90.f;
		}
	}
	else if (::cameraEye.z == 0.f)
	{
		if (::cameraEye.x <= 0)
		{
			::cameraYaw = 0.f;
		}
		else
		{
			::cameraYaw = 180.f;
		}
	}
	//anyways, after figuring out the yaw, we set the target at the negative of the xz of the ::camera position and y=0 (this faces the ::camera towards the origin)
	::cameraTarget = glm::vec3(-1.f * ::cameraEye.x, 0, -1.f * ::cameraEye.z);
	glm::normalize(::cameraTarget);

	const double MAX_DELTA_TIME = 0.1;	//100 ms
	double previousTime = glfwGetTime();

	// I know in graphics 2 we started moving some of the start stuff out of the while loop so I'm just putting these here now
	float ratio;
	int width, height;

	while (!glfwWindowShouldClose(pWindow))
	{
		glUseProgram(program);
		// not moving these out cause I want to ensure they're reset and not using last frame's information
		glm::mat4 matModel;				// used to be "m"; Sometimes it's called "world"
		glm::mat4 matProjection;        // used to be "p";
		glm::mat4 matView;              // used to be "v";

		double currentTime = glfwGetTime();
		deltaTime = currentTime - previousTime;
		deltaTime = (deltaTime > MAX_DELTA_TIME ? MAX_DELTA_TIME : deltaTime);
		previousTime = currentTime;

		glfwGetFramebufferSize(pWindow, &width, &height);
		ratio = width / (float)height;


		// ***************************************************
		// Physics
		::g_world->TimeStep((float)deltaTime);

		
		// ***************************************************
		
		


		//Turn on the depth buffer
		glEnable(GL_DEPTH);         // Turns on the depth buffer
		glEnable(GL_DEPTH_TEST);    // Check if the pixel is already closer

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// *******************************************************
		// Screen is cleared and we are ready to draw the scene...
		// *******************************************************

		// Update the title text
		glfwSetWindowTitle(pWindow, ::g_TitleText.c_str());



		// Copy the light information into the shader to draw the scene
		::g_pTheLights->CopyLightInfoToShader();

		/*matProjection = glm::perspective(0.6f,
			ratio,
			0.1f,
			1000.0f);*/

		matProjection = glm::perspective(0.6f,	// FOV variable later
			ratio,
			0.1f,								// Near plane
			1'000'000.0f);						// Far plane



		matView = glm::mat4(1.0f);
		//matView = glm::lookAt(	cameraEye,   // "eye"
		//						cameraAt,    // "at"
		//						cameraUp);


		// For the fly camera I use
		matView = glm::lookAt(::cameraEye,
			::cameraEye + ::cameraTarget,
			upVector);

		//	glUniformMatrix4fv(matView_Location, 1, GL_FALSE, glm::value_ptr(matView));

		glUniformMatrix4fv(pShaderProc->getUniformID_From_Name("matView"),
			1, GL_FALSE, glm::value_ptr(matView));


		glUniformMatrix4fv(matProjection_Location, 1, GL_FALSE, glm::value_ptr(matProjection));


		// Since this is a space game (most of the screen is filled with "sky"),
		// I'll draw the skybox first

		GLint bIsSkyBox_LocID = glGetUniformLocation(program, "bIsSkyBox");
		glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_TRUE);

		// Move the "skybox object" with the camera
		sphereSky->positionXYZ = ::cameraEye;
		DrawObject(sphereSky, glm::mat4(1.0f),
			matModel_Location, matModelInverseTranspose_Location,
			program, ::g_pVAOManager);

		glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_FALSE);

		// If there were transparent objects, this is where I would sort them

		// **********************************************************************
		// Draw the "scene" of all objects.
		// i.e. go through the vector and draw each one...
		// **********************************************************************
		for (unsigned int index = 0; index != ::g_vec_pMeshes.size(); index++)
		{
			// So the code is a little easier...
			cMesh* pCurrentMesh = ::g_vec_pMeshes[index];

			matModel = glm::mat4(1.0f);  // "Identity" ("do nothing", like x1)
			//mat4x4_identity(m);

			if (pCurrentMesh->bHasDiscardTexture)
			{
				// Discard texture
				{
					//GLuint discardTextureNumber = ::g_pTextureManager->getTextureIDFromName("Lisse_mobile_shipyard-mal1.bmp");
					GLuint discardTextureNumber = ::g_pTextureManager->getTextureIDFromName(pCurrentMesh->discardTexture);

					// I'm picking texture unit 30 since it's not in use.
					GLuint textureUnit = 30;
					glActiveTexture(textureUnit + GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, discardTextureNumber);
					GLint discardTexture_LocID = glGetUniformLocation(program, "discardTexture");
					glUniform1i(discardTexture_LocID, textureUnit);


					// Turn it on
					glUniform1f(bDiscardTransparencyWindowsON_LocID, (GLfloat)GL_TRUE);
					DrawObject(pCurrentMesh,
						matModel,
						matModel_Location,
						matModelInverseTranspose_Location,
						program,
						::g_pVAOManager);

					// Turn it off
					glUniform1f(pShaderProc->mapUniformName_to_UniformLocation["bDiscardTransparencyWindowsOn"], (GLfloat)GL_FALSE);
				}
			}
			else
			{
				DrawObject(pCurrentMesh,
					matModel,
					matModel_Location,
					matModelInverseTranspose_Location,
					program,
					::g_pVAOManager);
			}


		}//for (unsigned int index

		

		// ***************************************************
		// Animation
		// Update the animation process and then update the meshes to match the current animation
		// Then draw all the meshes related to the Animated Entities
		// Project 2, probably keeping the same process but using bone animations instead of keyframe (keyframe animations are more for positional whole model animation)
		// But I'll likely need to implement the Process somehow, idk using his code is sometimes hard since he has his whole engine basically finished
		matModel = glm::mat4(1.0f);
		::g_pAnimationSystem->Process(::vec_pAnimatedEntities, deltaTime);
		for (unsigned int index = 0; index != ::vec_pAnimatedEntities.size(); index++)
		{
			::vec_pAnimatedEntities[index]->Update();
			DrawObject(::vec_pAnimatedEntities[index]->m_Mesh,
				matModel,
				matModel_Location,
				matModelInverseTranspose_Location,
				program,
				::g_pVAOManager);
		}

		// Bone Animation
		// void DrawObject(	cMesh* pCurrentMesh, glm::mat4 matModel, 
		//					GLint matModel_Location, GLint matModelInverseTranspose_Location,
		//					GLuint program,	cVAOManager* pVAOManager)
		//							^^^^
		// That program determines what shader we're using, however!
		// there are still a bunch of glGets and things here, so I'm going to re-create the DrawObject
		// for specifically drawing the bone objects like we do in the Render in the SimpleEngine
		//glUseProgram(boneProgram);

		//matProjection = glm::perspective(0.6f,	// FOV variable later
		//	ratio,
		//	0.1f,								// Near plane
		//	1'000'000.0f);						// Far plane



		//matView = glm::mat4(1.0f);
		////matView = glm::lookAt(	cameraEye,   // "eye"
		////						cameraAt,    // "at"
		////						cameraUp);


		//// For the fly camera I use
		//matView = glm::lookAt(::cameraEye,
		//	::cameraEye + ::cameraTarget,
		//	upVector);

		//	glUniformMatrix4fv(matView_Location, 1, GL_FALSE, glm::value_ptr(matView));

		//glUniformMatrix4fv(pBoneShaderProc->getUniformID_From_Name("ViewMatrix"),
		//	1, GL_FALSE, glm::value_ptr(matView));


		//glUniformMatrix4fv(pBoneShaderProc->getUniformID_From_Name("ProjectionMatrix"), 
		//	1, GL_FALSE, glm::value_ptr(matProjection));

		matModel = glm::mat4(1.0f);
		// Run the Bone System Process and then call update on each entity
		::g_pBoneSystem->Process(::g_vec_pBoneEntities, deltaTime);
		for (unsigned int index = 0; index != ::g_vec_pBoneEntities.size(); index++)
		{
			::g_vec_pBoneEntities[index]->Update();
			// New draw
			DrawBoneObject(::g_vec_pBoneEntities[index],
				matModel,
				//pBoneShaderProc->mapUniformName_to_UniformLocation["ModelMatrix"],
				matModel_Location,
				matModelInverseTranspose_Location,
				program,
				::g_pVAOManager);

		}

		// ***************************************************

		// Scene is drawn
		// **********************************************************************

		// "Present" what we've drawn.
		glfwSwapBuffers(pWindow);        // Show what we've drawn

		// Process any events that have happened
		glfwPollEvents();

		// Handle OUR keyboard, mouse stuff
		handleAsyncKeyboard(pWindow, deltaTime);
		handleAsyncMouse(pWindow, deltaTime);
	} //while (!glfwWindowShouldClose(window))

	// All done, so delete things...
	::g_ShutDown(pWindow);


	glfwDestroyWindow(pWindow);

	glfwTerminate();
	exit(EXIT_SUCCESS);

} //end of main

// Modified from previous versions to take texture information
bool loadWorldFile()
{
	std::stringstream ss;
	std::stringstream sFile;


	ss << SOLUTION_DIR << "common\\assets\\worldFile.txt";

	std::ifstream theFile(ss.str());

	if (!theFile.is_open())
	{
		fprintf(stderr, "Could not open worldFile.txt");
		return false;
	}

	std::string nextToken;
	ss.str("");

	//Throwaway text describing the format of the file
	theFile >> nextToken;       //ModelFileName(extension)
	theFile >> nextToken;       //Position(x,y,z)
	theFile >> nextToken;       //Orientation(x,y,z)
	theFile >> nextToken;       //Scale(x,y,z)
	theFile >> nextToken;       //Colour(r,g,b,a)
	theFile >> nextToken;		//Light
	theFile >> nextToken;		//TextureOp
	theFile >> nextToken;		//TextureName1
	theFile >> nextToken;		//Ratio1
	theFile >> nextToken;		//TextureName2
	theFile >> nextToken;		//Ratio2
	theFile >> nextToken;		//TextureName3
	theFile >> nextToken;		//Ratio3
	theFile >> nextToken;		//DiscardTexture

	theFile >> nextToken;		//Camera(x,y,z)
	theFile >> nextToken;		//x position
	::cameraEye.x = std::stof(nextToken);
	theFile >> nextToken;		//y position
	::cameraEye.y = std::stof(nextToken);
	theFile >> nextToken;		//z position
	::cameraEye.z = std::stof(nextToken);

	theFile >> nextToken;		//number of transparent objects
	numberOfTransparentObjects = std::stoi(nextToken);

	//From here modify based on worldFile format
	while (theFile >> nextToken)    //this should always be the name of the model to load or end.  Potential error check, add a check for "ply" in the mdoel name
	{
		cMesh* curMesh = new cMesh;
		if (nextToken == "end")
		{
			break;
		}
		std::cout << nextToken << std::endl;        //Printing model names to console, just making sure we're loading ok.  Can be commented out whenever
		//First is the file name of model
		//ss << SOLUTION_DIR << "common\\assets\\models\\" << nextToken;		// don't need this with the setBasePath function for the loading intot the VAO
		curMesh->meshName = nextToken;
		//Next 3 are the position of the model
		theFile >> nextToken;                                               //x position for the model
		curMesh->positionXYZ.x = std::stof(nextToken);
		theFile >> nextToken;                                               //y position for the model
		curMesh->positionXYZ.y = std::stof(nextToken);
		theFile >> nextToken;                                               //z position for the model
		curMesh->positionXYZ.z = std::stof(nextToken);
		//Next 3 are the orientation of the model
		theFile >> nextToken;                                               //x orientation value
		//curMesh.orientationXYZ.x = std::stof(nextToken);
		curMesh->orientationXYZ.x = glm::radians(std::stof(nextToken));
		theFile >> nextToken;                                               //y orientation value
		//curMesh.orientationXYZ.y = std::stof(nextToken);
		curMesh->orientationXYZ.y = glm::radians(std::stof(nextToken));
		theFile >> nextToken;                                               //z orientation value
		//curMesh.orientationXYZ.z = std::stof(nextToken);
		curMesh->orientationXYZ.z = glm::radians(std::stof(nextToken));
		// TODO: Update this to 3 values
		//Next is the scale to multiply the model by
		theFile >> nextToken;
		curMesh->scale.x = std::stof(nextToken);
		theFile >> nextToken;
		curMesh->scale.y = std::stof(nextToken);
		theFile >> nextToken;
		curMesh->scale.z = std::stof(nextToken);
		//Next 3 are the r, g, b values for the model
		curMesh->bUseWholeObjectDiffuseColour = false;
		theFile >> nextToken;													//RGB red value
		curMesh->wholeObjectDiffuseRGBA.r = std::stof(nextToken) / 255.0f;		//convert to nice shader value (between 0 and 1)
		theFile >> nextToken;													//RGB green value
		curMesh->wholeObjectDiffuseRGBA.g = std::stof(nextToken) / 255.0f;		//convert to nice shader value (between 0 and 1)
		theFile >> nextToken;													//RGB blue value
		curMesh->wholeObjectDiffuseRGBA.b = std::stof(nextToken) / 255.0f;		//convert to nice shader value (between 0 and 1)
		theFile >> nextToken;
		curMesh->wholeObjectDiffuseRGBA.a = std::stof(nextToken);
		curMesh->alphaTransparency = std::stof(nextToken);

		// Light
		theFile >> nextToken;
		curMesh->bDontLight = (nextToken == "0");		// set to true or "0" in the worldFile to see textures without lighting

		// Texture operator
		theFile >> nextToken;
		curMesh->textureOperator = std::stoi(nextToken);

		// texture for mesh is in worldFile.txt now
		curMesh->clearTextureRatiosToZero();

		theFile >> nextToken;
		if (nextToken != "null")
		{
			curMesh->textureNames[1] = nextToken;
			theFile >> nextToken;
			curMesh->textureRatios[1] = std::stof(nextToken);
		}
		else
		{
			theFile >> nextToken;	// throw away the ratio number
		}

		theFile >> nextToken;
		if (nextToken != "null")
		{
			curMesh->textureNames[2] = nextToken;
			theFile >> nextToken;
			curMesh->textureRatios[2] = std::stof(nextToken);
		}
		else
		{
			theFile >> nextToken;	// throw away the ratio number
		}

		theFile >> nextToken;
		if (nextToken != "null")
		{
			curMesh->textureNames[3] = nextToken;
			theFile >> nextToken;
			curMesh->textureRatios[3] = std::stof(nextToken);
		}
		else
		{
			theFile >> nextToken;	// throw away the ratio number
		}

		if (curMesh->textureRatios[1] == 0.0f && curMesh->textureRatios[2] == 0.0f && curMesh->textureRatios[3] == 0.0f)
		{
			curMesh->textureRatios[0] = 1.0f;
		}

		// Discard texture
		theFile >> nextToken;
		if (nextToken != "null")
		{
			curMesh->discardTexture = nextToken;
			curMesh->bHasDiscardTexture = true;
		}

		::g_vec_pMeshes.push_back(curMesh);     //push the model onto our vector of meshes
		ss.str("");                         //reset the stringstream
	} //end of while
	theFile.close();
	return true;
}	//end of load world file

bool loadLightsFile()
{
	std::stringstream ss;
	std::stringstream sFile;


	ss << SOLUTION_DIR << "common\\assets\\lights.txt";

	std::ifstream theFile(ss.str());

	if (!theFile.is_open())
	{
		fprintf(stderr, "Could not open lights.txr");
		return false;
	}

	std::string nextToken;
	ss.str("");

	// Throw away description 
	theFile >> nextToken;		// position(x,y,z)
	theFile >> nextToken;		// diffuse(r,g,b)
	theFile >> nextToken;		// specular(r,g,b)
	theFile >> nextToken;		// atten(x,y,z)
	theFile >> nextToken;		// direction(x,y,z)
	theFile >> nextToken;		// param1(x,y,z)
	theFile >> nextToken;		// param2(x)

	::g_pTheLights->TurnOffLight(0);

	// Let's try 0 again
	unsigned int index = 0;	// can't start at 0 because for some reason the 0 light over writes all other lights

	while (theFile >> nextToken)
	{
		if (nextToken == "end" || index >= cLightManager::NUMBER_OF_LIGHTS)
		{
			break;
		}
		glm::vec3 position;
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 atten;
		glm::vec3 direction;
		glm::vec3 param1;
		float param2;

		// Position
		position.x = std::stof(nextToken);
		theFile >> nextToken;
		position.y = std::stof(nextToken);
		theFile >> nextToken;
		position.z = std::stof(nextToken);

		// Diffuse
		theFile >> nextToken;
		diffuse.x = std::stof(nextToken);
		theFile >> nextToken;
		diffuse.y = std::stof(nextToken);
		theFile >> nextToken;
		diffuse.z = std::stof(nextToken);

		// Specular
		theFile >> nextToken;
		specular.x = std::stof(nextToken);
		theFile >> nextToken;
		specular.y = std::stof(nextToken);
		theFile >> nextToken;
		specular.z = std::stof(nextToken);

		// Atten
		theFile >> nextToken;
		atten.x = std::stof(nextToken);
		theFile >> nextToken;
		atten.y = std::stof(nextToken);
		theFile >> nextToken;
		atten.z = std::stof(nextToken);

		// Direction
		theFile >> nextToken;
		direction.x = std::stof(nextToken);
		theFile >> nextToken;
		direction.y = std::stof(nextToken);
		theFile >> nextToken;
		direction.z = std::stof(nextToken);

		// Param1
		theFile >> nextToken;
		param1.x = std::stof(nextToken);;
		theFile >> nextToken;
		param1.y = std::stof(nextToken);
		theFile >> nextToken;
		param1.z = std::stof(nextToken);

		// Param2
		theFile >> nextToken;
		param2 = std::stof(nextToken);


		// Load everything into the lights
		::g_pTheLights->theLights[index].position = glm::vec4(position, 1.0f);
		::g_pTheLights->theLights[index].diffuse = glm::vec4(diffuse, 1.0f);
		::g_pTheLights->theLights[index].specular = glm::vec4(specular, 1.0f);
		::g_pTheLights->theLights[index].atten = glm::vec4(atten, 1.0f);
		::g_pTheLights->theLights[index].direction = glm::vec4(direction, 1.0f);
		::g_pTheLights->theLights[index].param1 = glm::vec4(param1, 1.0f);
		::g_pTheLights->theLights[index].param2 = glm::vec4(param2, 0.0f, 0.0f, 1.0f);

		index++;

	} //end of while

	theFile.close();
	return true;

} //end of load lights

// Not used for this project, can probably comment out
//Figured out the math for how to do this from https://learnopengl.com/Getting-started/Camera and http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
//Using the mouse position we calculate the direction that the camera will be facing
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//if it's the start of the program this smooths out a potentially glitchy jump
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//find the offset of where the mouse positions have moved
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	//multiply by sensitivity so that it's not potentially crazy fast
	float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	::cameraYaw += xOffset;         // The yaw is the rotation around the ::camera's y-axis (so we want to add the xOffset to it)
	::cameraPitch += yOffset;       // The pitch is the rotation around the ::camera's x-axis (so we want to add the yOffset to it)
	//This limits the pitch so that we can't just spin the ::camera under/over itself
	if (::cameraPitch > 89.0f)
		::cameraPitch = 89.0f;
	if (::cameraPitch < -89.0f)
		::cameraPitch = -89.0f;
	//calculations for the new direction based on the mouse movements
	glm::vec3 direction;
	direction.x = cos(glm::radians(::cameraYaw)) * cos(glm::radians(::cameraPitch));
	direction.y = sin(glm::radians(::cameraPitch));
	direction.z = sin(glm::radians(::cameraYaw)) * cos(glm::radians(::cameraPitch));
	::cameraTarget = glm::normalize(direction);
} //fly ::camera

// Animation Project 1
// The purpose of this function is to contain the animated entity process
// This would later go into a factory and be called by some kind of mediator
// or builder to make entities of specific forms but for this project and for now, this is good enough for rock and roll
void MakeAnimatedEntites()
{
	// Create the entity pointer and set the values
	cAnimatedEntity* root = new cAnimatedEntity();
	root->position = glm::vec3(0.0f, 5.0f, 0.0f);
	root->rotation = glm::vec3(0.0f);
	root->scale = glm::vec3(1.0f);
	root->colour = glm::vec3(0.6f, 0.6f, 0.2f);
	// Make the entity's mesh
	root->m_Mesh = new cMesh();
	root->m_Mesh->meshName = "Isosphere_Smooth_Normals.ply";
	root->m_Mesh->bUseWholeObjectDiffuseColour = true;
	root->m_Mesh->wholeObjectDiffuseRGBA = glm::vec4(root->colour, 1.0f);
	root->m_Mesh->bDontLight = true;
	root->m_Mesh->bIsWireframe = true;
	root->m_Mesh->positionXYZ = root->position;
	root->m_Mesh->orientationXYZ = root->rotation;
	root->m_Mesh->scale = root->scale;

	// This will be a child entity of the root entity and be affected by the root's animations as well as it's own
	cAnimatedEntity* child1 = new cAnimatedEntity();
	child1->position = glm::vec3(0.0f, 2.0f, 0.0f);
	child1->rotation = glm::vec3(0.0f);
	child1->scale = glm::vec3(1.0f);
	child1->colour = glm::vec3(0.6f, 0.6f, 0.2f);
	// Make the entity's mesh
	child1->m_Mesh = new cMesh();
	child1->m_Mesh->meshName = "Isosphere_Smooth_Normals.ply";
	child1->m_Mesh->bUseWholeObjectDiffuseColour = true;
	child1->m_Mesh->wholeObjectDiffuseRGBA = glm::vec4(child1->colour, 1.0f);
	child1->m_Mesh->bDontLight = true;
	child1->m_Mesh->bIsWireframe = true;
	child1->m_Mesh->positionXYZ = child1->position;
	child1->m_Mesh->orientationXYZ = child1->rotation;
	child1->m_Mesh->scale = child1->scale;

	// This will be a child entity of the root entity and be affected by the root's animations as well as it's own
	cAnimatedEntity* child2 = new cAnimatedEntity();
	child2->position = glm::vec3(0.0f, 4.0f, 0.0f);
	child2->rotation = glm::vec3(0.0f);
	child2->scale = glm::vec3(1.0f);
	child2->colour = glm::vec3(0.6f, 0.6f, 0.2f);
	// Make the entity's mesh
	child2->m_Mesh = new cMesh();
	child2->m_Mesh->meshName = "Isosphere_Smooth_Normals.ply";
	child2->m_Mesh->bUseWholeObjectDiffuseColour = true;
	child2->m_Mesh->wholeObjectDiffuseRGBA = glm::vec4(child2->colour, 1.0f);
	child2->m_Mesh->bDontLight = true;
	child2->m_Mesh->bIsWireframe = true;
	child2->m_Mesh->positionXYZ = child2->position;
	child2->m_Mesh->orientationXYZ = child2->rotation;
	child2->m_Mesh->scale = child2->scale;

	// This is a separate entity with it's own animation(s)
	cAnimatedEntity* secondEntity = new cAnimatedEntity();
	secondEntity->position = glm::vec3(7.5f, 5.0f, 0.0f);
	secondEntity->rotation = glm::vec3(0.0f);
	secondEntity->scale = glm::vec3(1.0f);
	secondEntity->colour = glm::vec3(0.6f, 0.6f, 0.2f);
	// Make the entity's mesh
	secondEntity->m_Mesh = new cMesh();
	secondEntity->m_Mesh->meshName = "Turret_Simple_01_xyz_n_rgba_uv.ply";
	secondEntity->m_Mesh->bUseWholeObjectDiffuseColour = true;
	secondEntity->m_Mesh->wholeObjectDiffuseRGBA = glm::vec4(secondEntity->colour, 1.0f);
	secondEntity->m_Mesh->bDontLight = true;
	secondEntity->m_Mesh->bIsWireframe = true;
	secondEntity->m_Mesh->positionXYZ = secondEntity->position;
	secondEntity->m_Mesh->orientationXYZ = secondEntity->rotation;
	secondEntity->m_Mesh->scale = secondEntity->scale;


	{	// Animation 1, root rotates around the origin, child 1 also rotates around the x-axis
		Animation* rootAnimation = root->AddAnimation();
		rootAnimation->currentTime = 0;
		rootAnimation->duration = 5.0f;
		rootAnimation->playing = true;
		rootAnimation->speed = 1.0f;
		rootAnimation->repeat = true;

		rootAnimation->keyFramePositions.push_back(KeyFramePosition(0, root->position, EasingType::EaseIn));
		rootAnimation->keyFramePositions.push_back(KeyFramePosition(5, root->position, EasingType::EaseIn));

		rootAnimation->keyFrameScales.push_back(KeyFrameScale(0, root->scale, EasingType::EaseIn));
		rootAnimation->keyFrameScales.push_back(KeyFrameScale(5, root->scale, EasingType::EaseIn));

		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseIn));
		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(0.7071f, 0.0f, 0.0f, 0.7071f), 0, EasingType::EaseIn));

		// *************************************************************
		Animation* child1Animation = child1->AddAnimation();
		child1Animation->currentTime = 0;
		child1Animation->duration = 5.0f;
		child1Animation->playing = true;
		child1Animation->speed = 1.0f;
		child1Animation->repeat = true;

		child1Animation->keyFramePositions.push_back(KeyFramePosition(0, child1->position, EasingType::EaseIn));
		child1Animation->keyFramePositions.push_back(KeyFramePosition(5, child1->position, EasingType::EaseIn));

		child1Animation->keyFrameScales.push_back(KeyFrameScale(0, child1->scale, EasingType::EaseIn));
		child1Animation->keyFrameScales.push_back(KeyFrameScale(5, child1->scale, EasingType::EaseIn));

		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 2, EasingType::EaseIn));
		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(0.7071, 0.0f, 0.0f, 0.7071f), 2, EasingType::EaseIn));
		
		// *************************************************************
		Animation* child2Animation = child2->AddAnimation();
		child2Animation->currentTime = 0;
		child2Animation->duration = 5.0f;
		child2Animation->playing = true;
		child2Animation->speed = 1.0f;
		child2Animation->repeat = true;

		child2Animation->keyFramePositions.push_back(KeyFramePosition(0, child2->position, EasingType::EaseIn));
		child2Animation->keyFramePositions.push_back(KeyFramePosition(5, child2->position, EasingType::EaseIn));

		child2Animation->keyFrameScales.push_back(KeyFrameScale(0, child2->scale, EasingType::EaseIn));
		child2Animation->keyFrameScales.push_back(KeyFrameScale(5, child2->scale, EasingType::EaseIn));

		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 1, EasingType::EaseIn));
		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(0.7071, 0.0f, 0.0f, 0.7071f), 1, EasingType::EaseIn));

	}

	{	// Animation 2, root moves to the "right", positive X (which is left for my initial camera) and up, child doubles in size
		Animation* rootAnimation = root->AddAnimation();
		rootAnimation->currentTime = 0;
		rootAnimation->duration = 5.0f;
		rootAnimation->playing = true;
		rootAnimation->speed = 1.0f;
		rootAnimation->repeat = true;

		rootAnimation->keyFramePositions.push_back(KeyFramePosition(0, root->position, EasingType::EaseOut));
		rootAnimation->keyFramePositions.push_back(KeyFramePosition(5, glm::vec3(5.0f, 1.0f, 0.0f), EasingType::EaseOut));

		rootAnimation->keyFrameScales.push_back(KeyFrameScale(0, root->scale, EasingType::EaseOut));
		rootAnimation->keyFrameScales.push_back(KeyFrameScale(5, root->scale, EasingType::EaseOut));

		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseOut));
		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseOut));

		// *************************************************************
		Animation* child1Animation = child1->AddAnimation();
		child1Animation->currentTime = 0;
		child1Animation->duration = 5.0f;
		child1Animation->playing = true;
		child1Animation->speed = 1.0f;
		child1Animation->repeat = true;

		child1Animation->keyFramePositions.push_back(KeyFramePosition(0, child1->position, EasingType::EaseOut));
		child1Animation->keyFramePositions.push_back(KeyFramePosition(5, child1->position, EasingType::EaseOut));

		child1Animation->keyFrameScales.push_back(KeyFrameScale(0, child1->scale, EasingType::EaseOut));
		child1Animation->keyFrameScales.push_back(KeyFrameScale(5, child1->scale * 2.0f, EasingType::EaseOut));

		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseOut));
		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseOut));
	
		// *************************************************************
		Animation* child2Animation = child2->AddAnimation();
		child2Animation->currentTime = 0;
		child2Animation->duration = 5.0f;
		child2Animation->playing = true;
		child2Animation->speed = 1.0f;
		child2Animation->repeat = true;

		child2Animation->keyFramePositions.push_back(KeyFramePosition(0, child2->position, EasingType::EaseOut));
		child2Animation->keyFramePositions.push_back(KeyFramePosition(5, child2->position, EasingType::EaseOut));

		child2Animation->keyFrameScales.push_back(KeyFrameScale(0, child2->scale, EasingType::EaseOut));
		child2Animation->keyFrameScales.push_back(KeyFrameScale(5, child2->scale * 2.0f, EasingType::EaseOut));

		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseOut));
		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseOut));


	}
	

	{	// Animation 3, root grows, then moves.  child1 rotates then shrinks
		Animation* rootAnimation = root->AddAnimation();
		rootAnimation->currentTime = 0;
		rootAnimation->duration = 10.0f;
		rootAnimation->playing = true;
		rootAnimation->speed = 1.0f;
		rootAnimation->repeat = true;

		rootAnimation->keyFramePositions.push_back(KeyFramePosition(0, root->position, EasingType::EaseInOut));
		rootAnimation->keyFramePositions.push_back(KeyFramePosition(5, root->position, EasingType::EaseInOut));
		rootAnimation->keyFramePositions.push_back(KeyFramePosition(10, glm::vec3(-7.5f, 5.0f, 2.0f), EasingType::EaseInOut));

		rootAnimation->keyFrameScales.push_back(KeyFrameScale(0, root->scale, EasingType::EaseInOut));
		rootAnimation->keyFrameScales.push_back(KeyFrameScale(5, root->scale * 1.5f, EasingType::EaseInOut));
		rootAnimation->keyFrameScales.push_back(KeyFrameScale(10, root->scale * 1.5f, EasingType::EaseInOut));

		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseInOut));
		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseInOut));
		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(10, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseInOut));

		// *************************************************************
		Animation* child1Animation = child1->AddAnimation();
		child1Animation->currentTime = 0;
		child1Animation->duration = 10.0f;
		child1Animation->playing = true;
		child1Animation->speed = 1.0f;
		child1Animation->repeat = true;

		child1Animation->keyFramePositions.push_back(KeyFramePosition(0, child1->position, EasingType::EaseInOut));
		child1Animation->keyFramePositions.push_back(KeyFramePosition(5, child1->position, EasingType::EaseInOut));
		child1Animation->keyFramePositions.push_back(KeyFramePosition(10, child1->position, EasingType::EaseInOut));

		child1Animation->keyFrameScales.push_back(KeyFrameScale(0, child1->scale, EasingType::EaseInOut));
		child1Animation->keyFrameScales.push_back(KeyFrameScale(5, child1->scale, EasingType::EaseInOut));
		child1Animation->keyFrameScales.push_back(KeyFrameScale(10, child1->scale / 2.0f, EasingType::EaseInOut));

		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseInOut));
		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, -0.65f, 0.0f, 0.0f), 0, EasingType::EaseInOut));
		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(10, glm::quat(1.0f, -0.65f, 0.0f, 0.0f), 0, EasingType::EaseInOut));
	
		// *************************************************************
		Animation* child2Animation = child2->AddAnimation();
		child2Animation->currentTime = 0;
		child2Animation->duration = 10.0f;
		child2Animation->playing = true;
		child2Animation->speed = 1.0f;
		child2Animation->repeat = true;

		child2Animation->keyFramePositions.push_back(KeyFramePosition(0, child2->position, EasingType::EaseInOut));
		child2Animation->keyFramePositions.push_back(KeyFramePosition(5, child2->position, EasingType::EaseInOut));
		child2Animation->keyFramePositions.push_back(KeyFramePosition(10, child2->position, EasingType::EaseInOut));

		child2Animation->keyFrameScales.push_back(KeyFrameScale(0, child2->scale, EasingType::EaseInOut));
		child2Animation->keyFrameScales.push_back(KeyFrameScale(5, child2->scale, EasingType::EaseInOut));
		child2Animation->keyFrameScales.push_back(KeyFrameScale(10, child2->scale / 2.0f, EasingType::EaseInOut));

		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::EaseInOut));
		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, -0.65f, 0.0f, 0.0f), 0, EasingType::EaseInOut));
		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(10, glm::quat(1.0f, -0.65f, 0.0f, 0.0f), 0, EasingType::EaseInOut));


	}


	{	// Animation 4, root shrinks, moves and rotates.  child1 grows (so it should stay the same size), moves and rotates
		Animation* rootAnimation = root->AddAnimation();
		rootAnimation->currentTime = 0;
		rootAnimation->duration = 10.0f;
		rootAnimation->playing = true;
		rootAnimation->speed = 1.0f;
		rootAnimation->repeat = true;

		rootAnimation->keyFramePositions.push_back(KeyFramePosition(0, root->position, EasingType::None));
		rootAnimation->keyFramePositions.push_back(KeyFramePosition(5, glm::vec3(0.0f, -0.5f, -3.2f), EasingType::None));
		rootAnimation->keyFramePositions.push_back(KeyFramePosition(10, glm::vec3(5.0f, 5.0f, 2.0f), EasingType::None));

		rootAnimation->keyFrameScales.push_back(KeyFrameScale(0, root->scale, EasingType::None));
		rootAnimation->keyFrameScales.push_back(KeyFrameScale(5, root->scale / 1.5f, EasingType::None));
		rootAnimation->keyFrameScales.push_back(KeyFrameScale(10, root->scale / 2.0f, EasingType::None));

		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::None));
		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, 0.0f, 0.65f, 1.0f), 0, EasingType::None));
		rootAnimation->keyFrameRotations.push_back(KeyFrameRotation(10, glm::quat(1.0f, -0.33f, 0.75f, 0.0f), 0, EasingType::None));

		// *************************************************************
		Animation* child1Animation = child1->AddAnimation();
		child1Animation->currentTime = 0;
		child1Animation->duration = 10.0f;
		child1Animation->playing = true;
		child1Animation->speed = 1.0f;
		child1Animation->repeat = true;

		child1Animation->keyFramePositions.push_back(KeyFramePosition(0, child1->position, EasingType::None));
		child1Animation->keyFramePositions.push_back(KeyFramePosition(5, child1->position + glm::vec3(0.0f, -0.5f, -3.2f), EasingType::None));
		child1Animation->keyFramePositions.push_back(KeyFramePosition(10, child1->position + glm::vec3(5.0f, 5.0f, 2.0f), EasingType::None));

		child1Animation->keyFrameScales.push_back(KeyFrameScale(0, child1->scale, EasingType::None));
		child1Animation->keyFrameScales.push_back(KeyFrameScale(5, child1->scale * 1.5f, EasingType::None));
		child1Animation->keyFrameScales.push_back(KeyFrameScale(10, child1->scale * 2.0f, EasingType::None));

		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::None));
		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, -0.65f, 0.1f, 0.32f), 0, EasingType::None));
		child1Animation->keyFrameRotations.push_back(KeyFrameRotation(10, glm::quat(1.0f, -0.65f, -0.1f, 0.44f), 0, EasingType::None));
	
		// *************************************************************
		Animation* child2Animation = child2->AddAnimation();
		child2Animation->currentTime = 0;
		child2Animation->duration = 10.0f;
		child2Animation->playing = true;
		child2Animation->speed = 1.0f;
		child2Animation->repeat = true;

		child2Animation->keyFramePositions.push_back(KeyFramePosition(0, child2->position, EasingType::None));
		child2Animation->keyFramePositions.push_back(KeyFramePosition(5, child2->position + glm::vec3(0.0f, -0.5f, -3.2f), EasingType::None));
		child2Animation->keyFramePositions.push_back(KeyFramePosition(10, child2->position + glm::vec3(5.0f, 5.0f, 2.0f), EasingType::None));

		child2Animation->keyFrameScales.push_back(KeyFrameScale(0, child2->scale, EasingType::None));
		child2Animation->keyFrameScales.push_back(KeyFrameScale(5, child2->scale * 1.5f, EasingType::None));
		child2Animation->keyFrameScales.push_back(KeyFrameScale(10, child2->scale * 2.0f, EasingType::None));

		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0, EasingType::None));
		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, -0.65f, 0.1f, 0.32f), 0, EasingType::None));
		child2Animation->keyFrameRotations.push_back(KeyFrameRotation(10, glm::quat(1.0f, -0.65f, -0.1f, 0.44f), 0, EasingType::None));

}
	

	{	// Animation 5, secondEntity's animation, no control over it except reverse and speed
		Animation* secondAnimation = secondEntity->AddAnimation();
		secondAnimation->currentTime = 0;
		secondAnimation->duration = 10.0f;
		secondAnimation->playing = true;
		secondAnimation->speed = 1.5f;
		secondAnimation->repeat = true;

		secondAnimation->keyFramePositions.push_back(KeyFramePosition(0, secondEntity->position, EasingType::EaseInOut));
		secondAnimation->keyFramePositions.push_back(KeyFramePosition(5, secondEntity->position + glm::vec3(2.0f, -0.5f, 2.5f), EasingType::EaseInOut));
		secondAnimation->keyFramePositions.push_back(KeyFramePosition(10, secondEntity->position + glm::vec3(5.0f, 5.0f, 5.0f), EasingType::EaseInOut));

		secondAnimation->keyFrameScales.push_back(KeyFrameScale(0, secondEntity->scale, EasingType::EaseInOut));
		secondAnimation->keyFrameScales.push_back(KeyFrameScale(5, secondEntity->scale / 3.0f, EasingType::EaseInOut));
		secondAnimation->keyFrameScales.push_back(KeyFrameScale(10, secondEntity->scale * 2.0f, EasingType::EaseInOut));

		secondAnimation->keyFrameRotations.push_back(KeyFrameRotation(0, glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 1, EasingType::EaseInOut));
		secondAnimation->keyFrameRotations.push_back(KeyFrameRotation(5, glm::quat(1.0f, -0.78f, 0.5f, 1.0f), 2, EasingType::EaseInOut));
		secondAnimation->keyFrameRotations.push_back(KeyFrameRotation(10, glm::quat(1.0f, 0.0f, 1.0f, -0.5f), 1, EasingType::EaseInOut));

	}
	

	child2->SetParent(child1);
	child1->SetParent(root);
	

	::vec_pAnimatedEntities.push_back(root);
	::vec_pAnimatedEntities.push_back(secondEntity);
} //end of MakeAnimatedEntities

// Animation Project 2
// contain the bone animation entity process (if that's what it ends up being
void MakeBoneAnimationEntities()
{
	// TODO
	// Create the entity pointer and set the values
	cBoneEntity* root = new cBoneEntity();
	//root->position = glm::vec3(0.0f, 5.0f, 0.0f);
	//root->rotation = glm::vec3(0.0f);
	//root->scale = glm::vec3(1.0f);
	//root->colour = glm::vec3(0.6f, 0.6f, 0.2f);
	// Make the entity's mesh
	root->m_Mesh = new cMesh();
	//root->m_Mesh->meshName = "4-tier-cone_xyz_n_rgba_uv.ply";
	root->m_Mesh->meshName = "Isosphere_Smooth_Normals.ply";
	root->m_Mesh->bUseObjectDebugColour = true;
	root->m_Mesh->objectDebugColourRGBA = glm::vec4(1.0f);
	root->m_Mesh->bUseWholeObjectDiffuseColour = true;
	root->m_Mesh->wholeObjectDiffuseRGBA = glm::vec4(1.0f);
	root->m_Mesh->bDontLight = true;
	root->m_Mesh->bIsWireframe = false;

	root->m_Mesh->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	root->m_Mesh->orientationXYZ = glm::vec3(glm::radians(270.0f), 0.0f, 0.0f);
	root->m_Mesh->scale = glm::vec3(1.0f);

	// made in constructor
	//root->p_mBoneHierarchy = new BoneHierarchy;


	
	Bone* rootBone = new Bone();
	rootBone->index = 0;
	rootBone->SetPosition(glm::vec3(0.0f));
	//rootBone->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
	//rootBone->SetPosition(root->m_Mesh->positionXYZ);
	//rootBone->SetRotation(glm::quat(1.0f, -1.0f, 0.0f, 0.0f));
	//rootBone->SetRotation(glm::quat(0.7071, 0.0f, 0.0f, 0.7071));
	//rootBone->SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	rootBone->SetRotation(glm::quat(glm::vec3(root->m_Mesh->orientationXYZ.x, 
		root->m_Mesh->orientationXYZ.y,
		root->m_Mesh->orientationXYZ.z)));
	//rootBone->SetRotation(glm::quat(1.0f, glm::eulerAngleX(90.0f), 0.0f, 0.0f));
	//rootBone->SetScale(glm::vec3(1.0f));
	//rootBone->SetScale(root->m_Mesh->scale);
	rootBone->SetScale(glm::vec3(1.0f));
	glm::mat4 BoneTranslationMatrix = glm::translate(glm::mat4(1.0f), rootBone->GetPosition());
	glm::mat4 BoneRotationMatrix = glm::mat4_cast(rootBone->GetRotation());
	//glm::mat4 BoneRotationMatrix = glm::mat4(1.0f);
	glm::mat4 BoneScaleMatrix = glm::scale(glm::mat4(1.0f), rootBone->GetScale());
	glm::mat4 BoneModelMatrix = BoneTranslationMatrix * BoneRotationMatrix * BoneScaleMatrix;
	rootBone->SetModelMatrix(BoneModelMatrix);
	//rootBone->SetModelMatrix(glm::mat4(1.0f));

	Transform transform;
	//transform.value = rootBone->GetModelMatrix();
	transform.position = rootBone->GetPosition();
	transform.rotation = rootBone->GetRotation();
	transform.scale = rootBone->GetScale();
	root->p_mBoneHierarchy->currentTransforms.push_back(transform);
	
	
	Bone* bone1 = new Bone();
	bone1->index = 1;
	bone1->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	bone1->SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	bone1->SetScale(glm::vec3(1.0f));
	glm::mat4 Bone1TranslationMatrix = glm::translate(glm::mat4(1.0f), bone1->GetPosition());
	//glm::mat4 Bone1RotationMatrix = glm::mat4_cast(bone1->GetRotation());
	glm::mat4 Bone1RotationMatrix = glm::mat4(1.0f);
	glm::mat4 Bone1ScaleMatrix = glm::scale(glm::mat4(1.0f), bone1->GetScale());
	glm::mat4 Bone1ModelMatrix = Bone1TranslationMatrix * Bone1RotationMatrix * Bone1ScaleMatrix;

	//bone1->SetModelMatrix(Bone1ModelMatrix);
	bone1->SetModelMatrix(glm::mat4(1.0f));
	Transform transform1;
	//transform1.value = bone1->GetModelMatrix();
	transform1.position = bone1->GetPosition();
	transform1.rotation = bone1->GetRotation();
	transform1.scale = bone1->GetScale();
	root->p_mBoneHierarchy->currentTransforms.push_back(transform1);

	rootBone->AddChild(bone1);
	bone1->SetParent(rootBone);
	
	Bone* bone2 = new Bone();
	bone2->index = 2;
	bone2->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	bone2->SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	bone2->SetScale(glm::vec3(1.0f));
	glm::mat4 Bone2TranslationMatrix = glm::translate(glm::mat4(1.0f), bone2->GetPosition());
	glm::mat4 Bone2RotationMatrix = glm::mat4_cast(bone2->GetRotation());
	glm::mat4 Bone2ScaleMatrix = glm::scale(glm::mat4(1.0f), bone2->GetScale());
	glm::mat4 Bone2ModelMatrix = Bone2TranslationMatrix * Bone2RotationMatrix * Bone2ScaleMatrix;
	
	//bone2->SetModelMatrix(Bone2ModelMatrix);
	bone2->SetModelMatrix(glm::mat4(1.0f));

	Transform transform2;
	//transform2.value = bone2->GetModelMatrix();
	transform2.position = bone2->GetPosition();
	transform2.rotation = bone2->GetRotation();
	transform2.scale = bone2->GetScale();
	root->p_mBoneHierarchy->currentTransforms.push_back(transform2);
	
	bone1->AddChild(bone2);
	bone2->SetParent(bone1);

	Bone* bone3 = new Bone();
	bone3->index = 3;
	bone3->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	//bone3->SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	//bone3->SetScale(glm::vec3(1.0f));
	glm::mat4 Bone3TranslationMatrix = glm::translate(glm::mat4(1.0f), bone3->GetPosition());
	glm::mat4 Bone3RotationMatrix = glm::mat4_cast(bone3->GetRotation());
	glm::mat4 Bone3ScaleMatrix = glm::scale(glm::mat4(1.0f), bone3->GetScale());
	glm::mat4 Bone3ModelMatrix = Bone3TranslationMatrix * Bone3RotationMatrix * Bone3ScaleMatrix;
		
	//bone3->SetModelMatrix(Bone3ModelMatrix);
	bone3->SetModelMatrix(glm::mat4(1.0f));

	Transform transform3;
	//transform3.value = bone3->GetModelMatrix();
	transform3.position = bone3->GetPosition();
	transform3.rotation = bone3->GetRotation();
	transform3.scale = bone3->GetScale();
	root->p_mBoneHierarchy->currentTransforms.push_back(transform3);

	bone2->AddChild(bone3);
	bone3->SetParent(bone2);

	root->m_Mesh->positionXYZ = rootBone->GetPosition();
	root->m_Mesh->orientationXYZ = glm::eulerAngles(rootBone->GetRotation());
	root->m_Mesh->scale = rootBone->GetScale();

	root->p_mBoneHierarchy->bones.push_back(rootBone);
	root->p_mBoneHierarchy->bones.push_back(bone1);
	root->p_mBoneHierarchy->bones.push_back(bone2);
	root->p_mBoneHierarchy->bones.push_back(bone3);

	//root->p_mBoneHierarchy->currentTransforms.push_back(rootTransform);
	//root->p_mBoneHierarchy->currentTransforms.push_back(bone1Transform);
	//root->p_mBoneHierarchy->currentTransforms.push_back(bone2Transform);
	//root->p_mBoneHierarchy->currentTransforms.push_back(bone3Transform);

	root->p_mBoneHierarchy->rootBone = rootBone;


	//root->m_Mesh->positionXYZ = root->p_mBoneHierarchy->rootBone->GetPosition();
	//root->m_Mesh->orientationXYZ = glm::eulerAngles(root->p_mBoneHierarchy->rootBone->GetRotation());
	//root->m_Mesh->scale = root->p_mBoneHierarchy->rootBone->GetScale();

	::g_vec_pBoneEntities.push_back(root);

	return;
}