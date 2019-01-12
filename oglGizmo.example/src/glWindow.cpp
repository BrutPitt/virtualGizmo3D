////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tools/oglAxes.h"
#include "glWindow.h"

#include "tools/virtualGizmo.h"


// Global variable or member class
vfGizmo3DClass gizmo; 
vfGizmo3DClass &getGizmo() { return gizmo; }


glWindow::glWindow() 
{   

}


glWindow::~glWindow()
{

}

// Init OGL app
/////////////////////////////////////////////////
void glWindow::onInit()
{

    axes = new oglAxes(true);
    axes->setBackgroundColor(glm::vec4(.25));

#ifdef __EMSCRIPTEN__
        const char *vtxDefs = "#version 300 es\nprecision mediump float;\n";
        const char *fragDefs = "#version 300 es\nprecision mediump float;\n";
#else
    #ifdef GLAPP_NO_GLSL_PIPELINE
        const char *vtxDefs = "#version 430\n";
    #else
        const char *vtxDefs = "#version 410\n#define GLAPP_USE_PIPELINE\n";
    #endif
        const char *fragDefs = "#version 410\n";
#endif

    axes->initShaders(vtxDefs, fragDefs); 



    glViewport(0,0,theApp->GetWidth(), theApp->GetHeight());

    //If do not using GLFW, simply use your ID defines 
    getGizmo().setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    getGizmo().setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
    getGizmo().setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
    getGizmo().setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SUPER);

    getGizmo().setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0);
    getGizmo().setPanControl(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL|GLFW_MOD_SHIFT);
    //getGizmo().setPanControls(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_SHIFT);

    // viewportSize  is need to set mouse sensitivity for rotation
    // You nedd to call it in your "reshape" function: when resize the window
    // look below
    getGizmo().viewportSize(theApp->GetWidth(), theApp->GetHeight());

    // If you need to more feeling with the mouse use:
    // getGizmo().setGizmoFeeling(1.0);
    // 1.0 default,  > 1.0 more sensible, < 1.0 less sensible
    

    // other settings if you need it
    /////////////////////////////////////////////


    //getGizmo().setDollyScale(1.0f);
    //getGizmo().setDollyPosition(5.0f);
    //getGizmo().setRotationCenter(vec3(0.0));


}



//
/////////////////////////////////////////////////
void glWindow::onExit()
{
    delete axes;
   
}

//
/////////////////////////////////////////////////
void glWindow::onRender()
{
    
    
    glm::mat4 m(1.0f);                          // Identity matrix

    // virtualGizmo transformations
    getGizmo().applyTransform(m);           // apply transform to Matrix
    
    axes->getTransforms()->setModelMatrix(m);  // Model Matrix
    axes->getTransforms()->build_MV_MVP();     // Build ModelView and ModelViewProjetion matrix 

                                                // I have an inside helper function for all this 
    //gizmo->getTransforms()->applyTransforms(getGizmo());

    axes->render();                            // render axes

}



//
/////////////////////////////////////////////////
void glWindow::onIdle()
{
    // call it every rendering if want an idle continue rotation until you do not click on screen
    // look at glApp.cpp : "mainLoop" ("newFrame") functions
    getGizmo().idle();
}


//
/////////////////////////////////////////////////
void glWindow::onReshape(GLint w, GLint h)
{
    glViewport(0,0,w,h);
    theApp->SetWidth(w); theApp->SetHeight(h);
    
    axes->getTransforms()->setPerspective((h == 0 | w == 0) ? 1.0 : float(w)/(float(h)));

    // call it on resize window to re-align mouse sensitivity
    getGizmo().viewportSize(w, h);

}

//
/////////////////////////////////////////////////
void glWindow::onKeyUp(unsigned char key, int x, int y)
{

}


//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyDown(int key, int x, int y)
{


}


//
/////////////////////////////////////////////////
void glWindow::onKeyDown(unsigned char key, int x, int y)
{



}



//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyUp(int key, int x, int y)
{



}


//
/////////////////////////////////////////////////
void glWindow::onMouseButton(int button, int upOrDown, int x, int y)
{

    //  Call on mouse button event
    //      button:  your mouse button
    //      mod:     your modifier key -> CTRL, SHIFT, ALT, SUPER
    //      pressed: if button is pressed (TRUE) or released (FALSE)
    //      x, y:    mouse coordinates
    getGizmo().mouse((vgButtons) (button),
                         (vgModifiers) theApp->getModifier(),
                          upOrDown==GLFW_PRESS, x, y);

}

//
/////////////////////////////////////////////////
void glWindow::onMouseWheel(int wheel, int direction, int x, int y)
{

}

//
/////////////////////////////////////////////////
void glWindow::onMotion(int x, int y)
{
    //  Call on motion event to communicate the position
    getGizmo().motion(x, y);
}

//
/////////////////////////////////////////////////
void glWindow::onPassiveMotion(int x, int y)
{

}
