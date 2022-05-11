#include "globalThings.h"
//Stuff done in class with Michael Feeney

#include <sstream>
#include <iostream>
#include <physics/random_helpers.h>

//#include "rbSphereEntity.h"

// Turns off the: warning C4005: 'APIENTRY': macro redefinition
#pragma warning( disable: 4005)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

void reverseAnimation(std::vector<cAnimatedEntity*> entities);
void moveAnimationsRight(std::vector<cAnimatedEntity*> entities);
void moveAnimationsLeft(std::vector<cAnimatedEntity*> entities);

/*static*/ void GLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Pause/Play the all the animations
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        //for (unsigned int indexA = 0; indexA != ::vec_pAnimatedEntities.size(); indexA++)
        //{
        //    for (unsigned int indexB = 0; indexB != ::vec_pAnimatedEntities[indexA]->vecAnimations.size(); indexB++)
        //    {
        //        ::vec_pAnimatedEntities[indexA]->vecAnimations[indexB]->playing =
        //            !::vec_pAnimatedEntities[indexA]->vecAnimations[indexB]->playing;
        //    }
        //}
        //::vec_pAnimatedEntities[0]->vecAnimations[::vec_pAnimatedEntities[0]->animationIndex]->playing =
        //    !::vec_pAnimatedEntities[0]->vecAnimations[::vec_pAnimatedEntities[0]->animationIndex]->playing;
    }

    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
    {
        // Press a button to play the animation in reverse. This should control every
        // animation sequence that is active at the sametime.
        // So just the currently playing sequences need to be reversed, not every animation sequence on the entity
        // put it in a function so this isn't so messy looking
        //reverseAnimation(::vec_pAnimatedEntities);
    }

    // change the which animation is playing
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        //::g_animationSpeed = 1;
        ::g_BoneAnimation = 1;
    }
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        //::g_animationSpeed = 2;
        ::g_BoneAnimation = 2;
    }
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        //::g_animationSpeed = 3;
        ::g_BoneAnimation = 3;
    }
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        //::g_animationSpeed = 4;
        ::g_BoneAnimation = 4;
    }
    else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        //::g_animationSpeed = 5;
        ::g_BoneAnimation = 5;
    }
    else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        //::g_animationSpeed = 5;
        ::g_BoneAnimation = 0;
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        // initially just the first entity, but I decided might as well go through all of them
        // first entity should be the only one with multiple animations but I might add more during the recorded demo
       // moveAnimationsRight(::vec_pAnimatedEntities);
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        // initially just the first entity, but I decided might as well go through all of them
        // first entity should be the only one with multiple animations but I might add more during the recorded demo
        //moveAnimationsLeft(::vec_pAnimatedEntities);
    }

    // Basically I need to go back to Feeney's fly camera
    // From the help review session.
    // Move the camera to a location and look somewhere
    //if (key == GLFW_KEY_L && action == GLFW_PRESS)
    //{
    //    // Move the eye
    //    ::g_pFlyCamera->setEye(glm::vec3(8631.0f, -1487.0f, 13010.0f));

    //    // "look at" something.
    //    // Since we are using the fly camera, we can't just set the eye.
    //    //::g_pFlyCamera->setAt(glm::vec3(0.0f, 0.0f, 0.0f));

    //    ::g_pFlyCamera->Yaw_LeftRight(-80.0f);
    //    ::g_pFlyCamera->Pitch_up1Down(-10.0f);

    //}

    float cameraSpeed = 1.0f;
    //float objectMovementSpeed = 1.0f;
    //float lightMovementSpeed = 1.0f;

    bool bShiftDown = false;
    bool bControlDown = false;
    bool bAltDown = false;

    //    // Shift down?
    //    if ( mods == GLFW_MOD_SHIFT )       // 0x0001   0000 0001
    //    {
    //        // ONLY shift is down
    //    }
    //    // Control down?
    //    if ( mods == GLFW_MOD_CONTROL  )    // 0x0002   0000 0010
    //    // Alt down?
    //    if ( mods == GLFW_MOD_ALT   )       // 0x0004   0000 0100

        //   0000 0111 
        // & 0000 0001
        // -----------
        //   0000 0001 --> Same as the shift mask

        // Use bitwise mask to filter out just the shift
    if ((mods & GLFW_MOD_SHIFT) == GLFW_MOD_SHIFT)
    {
        // Shift is down and maybe other things, too
        bShiftDown = true;
    }
    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        // Shift is down and maybe other things, too
        bControlDown = true;
    }
    if ((mods & GLFW_MOD_ALT) == GLFW_MOD_ALT)
    {
        // Shift is down and maybe other things, too
        bAltDown = true;
    }


    //   // If JUST the shift is down, move the "selected" object
       if ( bShiftDown && (!bControlDown) && (!bAltDown) )
       {
           
    
    
       }//if ( bShiftDown && ( ! bControlDown ) && ( ! bAltDown ) )

       // If JUST the ALT is down, move the "selected" light
    if ((!bShiftDown) && (!bControlDown) && bAltDown)
    {
        

    }//if ( bShiftDown && ( ! bControlDown ) && ( ! bAltDown ) )

    return;
}

// goes through the active entities and bubbles the keyframes of the current animation to play them in reverse
void reverseAnimation(std::vector<cAnimatedEntity*> entities)
{
    for (unsigned int entityIndex = 0; entityIndex != /*::vec_pAnimatedEntities*/entities.size(); entityIndex++)
    {
        for (unsigned int keyFrameIndex = 0;
            keyFrameIndex != entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions.size();
            keyFrameIndex++)
        {
            for (unsigned int indexC = 0;
                indexC != entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions.size() - 1 - keyFrameIndex;
                indexC++)
            {   // Swap the parent's/root's keyframes so the animation plays in reverse
                KeyFramePosition tempFrame =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC];

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC] =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC + 1];

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC + 1] =
                    tempFrame;

                float tempFrameTime =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC].time;

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC].time =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC + 1].time;

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFramePositions[indexC + 1].time =
                    tempFrameTime;


                KeyFrameScale tempScale =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC];

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC] =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC + 1];

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC + 1] =
                    tempScale;

                float tempScaleTime =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC].time;

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC].time =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC + 1].time;

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameScales[indexC + 1].time =
                    tempScaleTime;


                KeyFrameRotation tempRotation =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC];

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC] =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC + 1];

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC + 1] =
                    tempRotation;

                float tempRotationTime =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC].time;

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC].time =
                    entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC + 1].time;

                entities[entityIndex]->vecAnimations[entities[entityIndex]->animationIndex]->keyFrameRotations[indexC + 1].time =
                    tempRotationTime;
            } //end of for indexC

        } //end of for keyFrameIndex

        // Oh good lord, the children need to have their animations swapped as well...
        // after swapping all the current entities keyframes, we have to go through the entities children, and then go through their keyframes
        //std::vector<cAnimatedEntity*> children = entities[entityIndex]->GetChildren();
        // nevermind, recursive call
        reverseAnimation(entities[entityIndex]->GetChildren());     // call reverse on the current entity's children
        

    } //end of for entityIndex
} //end of reverseAnimation


void moveAnimationsRight(std::vector<cAnimatedEntity*> entities)
{
    for (unsigned int entityIndex = 0; entityIndex != entities.size(); entityIndex++)
    {
        entities[entityIndex]->animationIndex++;
        if (entities[entityIndex]->animationIndex >= entities[entityIndex]->vecAnimations.size())
        {
            entities[entityIndex]->animationIndex = 0;
        }
        moveAnimationsRight(entities[entityIndex]->GetChildren());
        //for (unsigned int index = 0; index != ::vec_pAnimatedEntities[entityIndex]->GetChildren().size(); index++)
        //{
        //    ::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->animationIndex++;
        //    if (::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->animationIndex >= ::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->vecAnimations.size())
        //    {
        //        ::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->animationIndex = 0;
        //    }
        //}
    }
} //end of moveAnimationsRight
void moveAnimationsLeft(std::vector<cAnimatedEntity*> entities)
{
    for (unsigned int entityIndex = 0; entityIndex != entities.size(); entityIndex++)
    {
        entities[entityIndex]->animationIndex--;
        if (entities[entityIndex]->animationIndex < 0)
        {
            entities[entityIndex]->animationIndex = entities[entityIndex]->vecAnimations.size() - 1;
        }
        moveAnimationsLeft(entities[entityIndex]->GetChildren());
        //for (unsigned int index = 0; index != ::vec_pAnimatedEntities[entityIndex]->GetChildren().size(); index++)
        //{
        //    ::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->animationIndex--;
        //    if (::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->animationIndex < 0)
        //    {
        //        ::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->animationIndex = ::vec_pAnimatedEntities[entityIndex]->GetChildren()[index]->vecAnimations.size() - 1;
        //    }
        //}
    }
} //end of moveAnimationsLeft