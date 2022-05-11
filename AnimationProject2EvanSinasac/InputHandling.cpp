//Stuff done in class with Michael Feeney
#include "GLCommon.h" 
#include "globalThings.h"

#include <sstream>
#include <iostream>
//#include "rbSphereEntity.h"

// Handle async IO (keyboard, joystick, mouse, etc.)


// This is so the "fly camera" won't pay attention to the mouse if it's 
// not directly over the window. 
bool g_MouseIsInsideWindow = false;



void handleAsyncKeyboard(GLFWwindow* pWindow, double deltaTime)
{
   // float cameraMoveSpeed = 10.0f * deltaTime;   //::g_pFlyCamera->movementSpeed;
    float cameraMoveSpeed = 50.0f * deltaTime;   //::g_pFlyCamera->movementSpeed;
    //float cannonSpeed = 1.5f * deltaTime;
    float cameraMaxDistance = 80.0f;
    float cameraMaxDistanceSqr = cameraMaxDistance * cameraMaxDistance;
    float cameraMinDistance = 15.0f;
    float cameraMinDistanceSqr = cameraMinDistance * cameraMinDistance;

    //float sphereSpeedModifier = 10.0f;// *deltaTime;


    if (cGFLWKeyboardModifiers::areAllModsUp(pWindow))
    {
        // Use "fly" camera (keyboard for movement, mouse for aim)
        if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
        {
            ::cameraEye.x += ::cameraTarget.x * cameraMoveSpeed;
            ::cameraEye.z += ::cameraTarget.z * cameraMoveSpeed;
            //::g_pFlyCamera->MoveForward_Z(+cameraMoveSpeed);
        }
        if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)	// "backwards"
        {
            ::cameraEye.x -= ::cameraTarget.x * cameraMoveSpeed;
            ::cameraEye.z -= ::cameraTarget.z * cameraMoveSpeed;
        }
        if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)	// "left"
        {
            ::cameraEye -= glm::normalize(glm::cross(::cameraTarget, ::upVector)) * cameraMoveSpeed;
        }
        if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)	// "right"
        {
            ::cameraEye += glm::normalize(glm::cross(::cameraTarget, ::upVector)) * cameraMoveSpeed;
        }
        if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS)	// "up"
        {
            ::cameraEye += cameraMoveSpeed * ::upVector;
        }
        if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS)	// "down"
        {
            ::cameraEye -= cameraMoveSpeed * ::upVector;
        }

        // Nothing (for this project)
        if (glfwGetKey(pWindow, GLFW_KEY_UP) == GLFW_PRESS)
        {

        }
        if (glfwGetKey(pWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
        {

        }
        if (glfwGetKey(pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {

        }
        if (glfwGetKey(pWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        {

        }



        std::stringstream strTitle;
        // std::cout << 
        //glm::vec3 cameraEye = ::g_pFlyCamera->getEye();
        //strTitle << "Physics & Configuration 2: Project 1!  Rigidbody Sandbox.  Camera Eye: "
        //    << ::cameraEye.x << ", "
        //    << ::cameraEye.y << ", "
        //    << ::cameraEye.z << "  Camera Target: "
        //    << ::cameraTarget.x << ", "
        //    << ::cameraTarget.y << ", "
        //    << ::cameraTarget.z << "  Camera Eye + Target: "
        //    << cameraEye.x + cameraTarget.x << ", "
        //    << cameraEye.y + cameraTarget.y << ", "
        //    << cameraEye.z + cameraTarget.z;

        strTitle << "Animation Project 2: Bone Animation.  Camera Eye: "
            << ::cameraEye.x << ", "
            << ::cameraEye.y << ", "
            << ::cameraEye.z << std::endl;

        ::g_TitleText = strTitle.str();

    }//if ( cGFLWKeyboardModifiers::areAllModsUp(pWindow) )


    // If JUST the shift is down, move the "selected" object
    if (cGFLWKeyboardModifiers::isModifierDown(pWindow, true, false, false))
    {

    }//if ( cGFLWKeyboardModifiers::...


    // If JUST the ALT is down, move the "selected" light
    if (cGFLWKeyboardModifiers::isModifierDown(pWindow, false, true, false))
    {

    }//if ( cGFLWKeyboardModifiers::...

    return;
}


// We call these every frame
void handleAsyncMouse(GLFWwindow* window, double deltaTime)
{

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    //::g_pFlyCamera->setMouseXY(x, y);

    const float MOUSE_SENSITIVITY = 2.0f;


    // Mouse left (primary?) button pressed? 
    // AND the mouse is inside the window...
    //if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    //    && ::g_MouseIsInsideWindow)
    //{
    //    // Mouse button is down so turn the camera
    //    ::g_pFlyCamera->Yaw_LeftRight(::g_pFlyCamera->getDeltaMouseX() * MOUSE_SENSITIVITY, deltaTime);

    //    ::g_pFlyCamera->Pitch_up1Down(-::g_pFlyCamera->getDeltaMouseY() * MOUSE_SENSITIVITY, deltaTime);

    //}

    // Adjust the mouse speed
    if (::g_MouseIsInsideWindow)
    {
        const float MOUSE_WHEEL_SENSITIVITY = 0.1f;

        // Adjust the movement speed based on the wheel position
        //::g_pFlyCamera->movementSpeed -= (::g_pFlyCamera->getMouseWheel() * MOUSE_WHEEL_SENSITIVITY);

        //// Clear the mouse wheel delta (or it will increase constantly)
        //::g_pFlyCamera->clearMouseWheelValue();


        //if (::g_pFlyCamera->movementSpeed <= 0.0f)
        //{
        //    ::g_pFlyCamera->movementSpeed = 0.0f;
        //}
    }


    return;
}

void GLFW_cursor_enter_callback(GLFWwindow* window, int entered)
{
    if (entered)
    {
        //std::cout << "Mouse cursor is over the window" << std::endl;
        ::g_MouseIsInsideWindow = true;
    }
    else
    {
        //std::cout << "Mouse cursor is no longer over the window" << std::endl;
        ::g_MouseIsInsideWindow = false;
    }
    return;
}

// Called when the mouse scroll wheel is moved
void GLFW_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float mouseScrollWheelSensitivity = 0.1f;

    //::g_pFlyCamera->setMouseWheelDelta(yoffset * mouseScrollWheelSensitivity);

    return;
}

void GLFW_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

//#ifdef YO_NERDS_WE_USING_WINDOWS_CONTEXT_MENUS_IN_THIS_THANG
//    // Right button is pop-up
//    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
//    {
//        ShowWindowsContextMenu(window, button, action, mods);
//    }
//#endif
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {



        // No more firing but I want to keep this around in case I need it for reference later
        // Launch a laser projectile, cLaser is misbehaving, so we're just using cParticle with the laser properties, since Integrate doesn't change anyways
        //nPhysics::cParticle* shot = new nPhysics::cParticle(::g_config.GetLaserMass(), ::cameraEye + (::cameraTarget * 5.0f));
        //glm::vec3 velocityDir = /*::cameraEye + */::cameraTarget;
        //velocityDir = glm::normalize(velocityDir);
        //velocityDir *= ::g_config.GetLaserVelocity();
        //shot->SetVelocity(velocityDir);
        //shot->SetAcceleration(glm::vec3(0.0f, -3.8f, 0.0f));
        //shot->SetDamping(::g_config.GetLaserDamping());
        //shot->SetRadius(::g_config.GetLaserSize());
        //shot->SetUseTimer(true);
        //shot->SetTimer(::g_config.GetLaserLifeSpan());
        //::g_world->AddParticle(shot);
        //::g_vec_pProjectiles.push_back(shot);

        //cMesh* newShotMesh = new cMesh();
        //newShotMesh->meshName = "ISO_Sphere_flat_4div_xyz_n_rgba_uv.ply";
        //newShotMesh->positionXYZ = shot->GetPosition();
        ////newShot->orientationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
        //newShotMesh->scale = glm::vec3(shot->GetRadius());// 1.0f;
        //newShotMesh->bUseWholeObjectDiffuseColour = true;
        //newShotMesh->wholeObjectDiffuseRGBA = glm::vec4(0.75f, 1.0f, 0.0f, 1.0f);
        //newShotMesh->bUseObjectDebugColour = true;
        //newShotMesh->objectDebugColourRGBA = glm::vec4(0.75f, 1.0f, 0.0f, 1.0f);
        //newShotMesh->bDontLight = true;
        //newShotMesh->clearTextureRatiosToZero();
        //newShotMesh->textureNames[1] = "2k_jupiter.bmp";
        //newShotMesh->textureRatios[1] = 1.0f;

        //::g_vec_pMeshes.push_back(newShotMesh);
    }

    return;
}


void GLFW_cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    return;
}


