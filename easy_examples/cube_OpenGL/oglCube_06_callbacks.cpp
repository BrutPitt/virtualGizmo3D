//------------------------------------------------------------------------------
//  Copyright (c) 2018-2025 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include <cstdlib>
#include <cfloat>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "oglDebug.h"
#include "../commons/shadersAndModel.h"

/////////////////////////////////////////////////////////////////////////////
// vGizmo3D:
#include <vGizmo3D.h> // now also vGizmo3D.h from v3.1

int width = 1280, height = 800;
GLFWwindow *glfwWindow;

const int nElemVtx = 4;
const size_t nVertex = sizeof(coloredCubeData)/(sizeof(float)*2*nElemVtx);

int getModifier(GLFWwindow* window);

// Shaders & Vertex attributes
GLuint program, vao, vaoBuffer;
enum loc { vtxIdx = 0, colIdx, mvpIdx, lightIdx};     // shader locations

mat4 mvpMatrix, viewMatrix, projMatrix;
mat4 lightObj, lightMatrix, cubeObj;

/// vGizmo3D : declare global/static/member/..
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vg::vGizmo3D track;     // using vGizmo3D global/static/member instead of specifics variables...
                        // have rotations & Pan/Dolly position variables inside to use with imGuIZMO.quat
                        // And it's necessary if you want use also direct-screen manipulator

mat4 compensateView; // compensate rotation of viewMatrix lookAt Matrix

void draw()
{
    glUseProgram(program);

    glProgramUniformMatrix4fv(program, loc::mvpIdx,   1, false, value_ptr(mvpMatrix)  );  // vgMath permits cast to mat4*
    glProgramUniformMatrix4fv(program, loc::lightIdx, 1, false, value_ptr(lightMatrix));  // using value_ptr maintains GLM compatibility

    glBindVertexArray(vao);
    //glDrawArrays(GL_TRIANGLES, 0, nVertex);
    glDrawArraysInstanced(GL_TRIANGLES, 0, nVertex, 2);   // now using instanced draw to "simulate" light

    glUseProgram(0);
}

void setPerspective()
{
    float aspectRatio = float(height) / float(width);       // Set "camera" position and perspective
    float fov = radians( 45.0f ) * aspectRatio;
    projMatrix = perspective( fov, 1/aspectRatio, 0.1f, 100.0f );
}

void setScene()
{
    viewMatrix = lookAt( vec3( 10.0f, 10.0f, 10.0f ),   // From / EyePos
                         vec3(  0.0f,  0.0f,  0.0f ),   // To   /
                         vec3(  3.0f,  1.0f,   .0f));   // Up

    // Now scale cube to better view light position
    cubeObj = mat4(1); // nothing to do ... scale( vec3(.5));

/// imGuIZMO / vGizmo3D
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //track.setRotation(quat(1,0,0,0));                     // vGizmo3D with NO initial rotation (default initialization)
    //track.setRotation(eulerAngleXYZ(vec3(radians(45),
    //                                     radians( 0),
    //                                     radians( 0))));  // vGizmo3D with rotation of 45 degrees on X axis
    track.setRotation(viewMatrix);                          // vGizmo3D with ViewMatrix (lookAt) rotation

    // for Pan & Dolly always bounded on screen coords (x = left/right, y = up/douw, z = in/out) we remove viewMatrix rotation
    // otherwise Pan & Dolly have as reference the Cartesian axes
    compensateView = inverse(mat4_cast(quat(viewMatrix)));


    // light model
    vec3 lightPos(2, 2.5, 3);        // Light Position

    // acquiring rotation for the light pos
    const float len = length(lightPos);
     //if(len<1.0 && len>= FLT_EPSILON) { normalize(lightPos); len = 1.0; }  // controls are not necessary: lightPos is known
     //else if(len > FLT_EPSILON)
        quat q = angleAxis(acosf(-lightPos.x/len), normalize(vec3(FLT_EPSILON, lightPos.z, -lightPos.y)));
    track.setSecondRot(q);          // store secondary rotation for the Light

    lightObj = translate(mat4(1), lightPos);
    lightObj = scale(lightObj, vec3(.1));       // using same cube vertex but with 10% size

    lightObj = inverse(static_cast<mat4>(track.getSecondRot())) * lightObj ;

    setPerspective();
}

void glfwWindowSizeCallback(GLFWwindow* window, int w, int h)
{
    width = w; height = h;
    setPerspective();
    glViewport(0, 0, width, height);

    track.viewportSize(w, h);   // call it on resize window to re-adjust mouse sensitivity

    draw();
}

void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
    track.wheel(x, y);  // now we can use wheel to zoom
}

void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    double x,y;
    glfwGetCursorPos(window, &x, &y);

    int myButton;

    switch(button) {
        case GLFW_MOUSE_BUTTON_1    : // GLFW_MOUSE_BUTTON_LEFT
            myButton = vg::evButton1; // vg::evButtonLeft
            break;
        case GLFW_MOUSE_BUTTON_2    : // GLFW_MOUSE_BUTTON_RIGHT
            myButton = vg::evButton2; // || vg::evButtonRight
            break;
        case GLFW_MOUSE_BUTTON_3    : // GLFW_MOUSE_BUTTON_MIDDLE
            myButton = vg::evButton3; // || vg::evButtonMiddle
            break;
        default :
            myButton = -1;
    }
    // in GLFW "CURRENTLY" button ID & vg::enums coincide, so all the switch/case statement can be also omitted and pass directly "button"
    // in "mouse" call, but is good rule always to check framework IDs (in case of future changes) and select preferred vg::enum
    if(myButton>=0)
        track.mouse((vgButtons) myButton, (vgModifiers) getModifier(glfwWindow), action == GLFW_PRESS, (int)x, (int)y);
}

static void glfwMousePosCallback(GLFWwindow* window, double x, double y)
{
    track.motion(x, y);
}

void initGL()
{
    enableDebugCallback();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_instanced, NULL);
    glCompileShader(vertex);
    checkShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code, NULL);
    glCompileShader(fragment);
    checkShader(fragment);

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vaoBuffer);
    glNamedBufferStorage(vaoBuffer, sizeof(coloredCubeData), coloredCubeData, 0);

    glVertexArrayAttribBinding(vao,loc::vtxIdx, 0);
    glVertexArrayAttribFormat(vao, loc::vtxIdx, nElemVtx, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vao, loc::vtxIdx);

    glVertexArrayAttribBinding(vao,loc::colIdx, 0);
    glVertexArrayAttribFormat(vao, loc::colIdx, nElemVtx, GL_FLOAT, GL_FALSE, nElemVtx*sizeof(float));
    glEnableVertexArrayAttrib(vao, loc::colIdx);

    glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, 2*nElemVtx*sizeof(float));

    glViewport(0, 0, width, height);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CW);

    glDepthRange(-1.0, 1.0);
    setScene();
}

void initFramework()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

    glfwWindow = glfwCreateWindow(width, height, "glCube", NULL, NULL);
    if (!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfwWindow);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);  //get OpenGL extensions

    glfwSetWindowSizeCallback(glfwWindow, glfwWindowSizeCallback);

// vGizmo3D: Using Callbacks... more elegant method
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    glfwSetScrollCallback(glfwWindow, glfwScrollCallback);
    glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
    glfwSetCursorPosCallback(glfwWindow, glfwMousePosCallback);


    glfwSwapInterval(1); // 0 vSync off - 1 vSync on
}


/// vGizmo3D initialize: <br>
/// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities <br><br>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initVGizmo3D()     // Settings to control vGizmo3D
{
    // Initialization is necessary to associate your preferences to vGizmo3D
    // These are also the DEFAULT values, so if you want to maintain these combinations you can omit they
    // and to override only the associations that you want modify
        track.setGizmoRotControl         (vg::evButton1  /* or vg::evLeftButton */, 0 /* vg::evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        track.setGizmoRotXControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evShiftModifier);
        track.setGizmoRotYControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evControlModifier);
        track.setGizmoRotZControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evAltModifier | vg::evSuperModifier);
    // Set vGizmo3D control for secondary rotation
        track.setGizmoSecondaryRotControl(vg::evButton2  /* or vg::evRightButton */, 0 /* vg::evNoModifier */ );
    // Pan and Dolly/Zoom: mouse button and key modifier
        track.setDollyControl            (vg::evButton2 /* or vg::evRightButton */, vg::evControlModifier);
        track.setPanControl              (vg::evButton2 /* or vg::evRightButton */, vg::evShiftModifier);
    // N.B. vg::enums are ONLY mnemonic: select and pass specific vg::enum to framework (that can have also different IDs)

    // passing the screen sizes auto-set the mouse sensitivity
        track.viewportSize(width, height);      // but if you need to more feeling with the mouse use:
    // track.setGizmoFeeling(1.0);              // 1.0 default,  > 1.0 more sensible, < 1.0 less sensible

    // setIdleRotSpeed(1.0)                     // If used Idle() feature (continue rotation on Idle) it set that speed: more speed > 1.0 ,  less < 1.0

    // other settings if you need it
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // track.setDollyScale(1.0f);               // > 1.0 more sensible, < 1.0 less sensible
    // track.setDollyPosition(/* your pos */);  // input: float/double or vec3... in vec3 only Z is acquired
    // track.setPanScale(1.0f);                 // > 1.0 more sensible, < 1.0 less sensible
    // track.setPanyPosition(/* your pos */);   // vec3 ==> only X and Y are acquired
    // track.setPosition(/* your pos */);       // input vec3 is equivalent to call: track.setDollyPosition(/* your pos */); and track.setPanyPosition(/* your pos */);
    // track.setRotationCenter(/* vec3 */);     // new rotation center
    //
    // Watch vGizmo.h for more functionalities
}

/// vGizmo3D: Check key modifier currently pressed (GLFW version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int getModifier(GLFWwindow* window) {
    if((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)    == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return vg::evControlModifier;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS))
            return vg::evShiftModifier;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_ALT)   == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS))
            return vg::evAltModifier;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SUPER)   == GLFW_PRESS))
            return vg::evSuperModifier;
    else return vg::evNoModifier;
}

int main(int /* argc */, char ** /* argv */)    // necessary for SDLmain in Windows
{
    initFramework();         // initialize GLFW framework
    initGL();           // init OpenGL building vaoBuffer and shader program (compile and link vtx/frag shaders)

    // vGizmo3D: initialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    initVGizmo3D();

    // other OpenGL settings... used locally
    vec4 bgColor = vec4(0.0f);
    GLfloat f=1.0f;


    // main render/draw loop
    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();
        glClearBufferfv(GL_DEPTH, 0, &f);
        glClearBufferfv(GL_COLOR, 0, value_ptr(bgColor));

    // vGizmo3D: Now we use Callbacks... more elegant method
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.idle();   // set continuous rotation on Idle: the smooth rotation depends on speed of last mouse movements
                        // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                        // It can be stopped by click on screen (without mouse movement)


    // transferring the rotation to cube model matrix...
        mat4 modelMatrix = cubeObj * mat4_cast(track.getRotation());

    // Build a "translation" matrix
        mat4 translationMatrix = translate(mat4(1), track.getPosition());      // add translations (pan/dolly) to an identity matrix

    // build MVPs matrices to pass to shader
        mvpMatrix   = projMatrix * viewMatrix * compensateView * translationMatrix * modelMatrix;
        lightMatrix = projMatrix * viewMatrix * compensateView * translationMatrix * (static_cast<mat4>(track.getSecondRot())) * lightObj;

    // draw the cube, passing MVP matrix to the vtx shader
        draw();

        glfwSwapBuffers(glfwWindow);
    }


    // Cleanup OpenGL
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vaoBuffer);
    glDeleteProgram(program);

    // Cleanup Framework
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    return EXIT_SUCCESS;
}