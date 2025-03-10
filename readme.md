# vGizmo3D / virtualGizmo3D &nbsp;v4.0 beta
**vGizmo3D** / **virtualGizmo3D** is an 3D GIZMO manipulator: like a trackball it provides a way to rotate, move and scale a model, with mouse, also with dolly and pan features
You can also define a way to rotate the model around any single axis.
It use mouse movement on screen, mouse buttons and (eventually) key modifiers, like *Shift/Ctrl/Alt/Super*, that you define

It uses **quaternions** algebra, internally, to manage rotations, but you can also only pass your model matrix and gat back a transformation matrix with rotation, translation and scale, inside.

![alt text](https://raw.githubusercontent.com/BrutPitt/virtualGizmo3D/master/screenshots/oglGizmo.gif)

**vGizmo3D** / **virtualGizmo3D** is an *header only* tool (`vGizmo3D.h`) and **is not bound to frameworks or render engines**, is written in C++ (C++17) and uses [**vgMath**](https://github.com/BrutPitt/vgMath) a compact (my *single file header only*) vectors/matrices/quaternions tool/lib that makes **virtualGizmo3D** standalone.\
**Alternatively you can use [**glm**](https://github.com/g-truc/glm) with a simple define: read [about glm and vgMath](#about-glm-and-vgmath)*

**No other files or external libraries are required**

In this way you can use it with any engine, like: *OpenGL, DirectX, Vulkan, Metal, etc.* and/or  with any framework, like: *GLFW, SDL, GLUT, Native O.S. calls, etc.*

You can use [**vgMath**](https://github.com/BrutPitt/vgMath) also externally, for your purposes: it contains classes to manipulate **vec**tors (with 2/3/4 components), **quat**ernions, square **mat**ricies (3x3 and 4x4), both as *simple* single precision `float` **classes** (*Default*) or, enabling **template classes** (*simply adding a* `#define`), as both `float` and `double` data types. It contains also 4 helper functions to define Model/View matrix: **perspective**, **frustum**, **lookAt**, **ortho**

If need a larger/complete library, as alternative to **virtualGizmo3D**, is also possible to interface **imGuIZMO.quat** with [**glm** mathematics library](https://github.com/g-truc/glm) (*simply adding a* `#define`)

==>&nbsp; **Please, read [**Configure virtualGizmo3D**](#Configure-virtualGizmo3D) section.*



### Live WebGL2 / WebGPU examples
You can run/test an emscripten WebGL 2 example of **virtualGismo3D** from following link:
- [vGizmo3D v2.0 WebGL2](https://www.michelemorrone.eu/emsExamples/oglGizmo.html)
- [vGizmo3D v3.99 WebGL2 - easy_cube example](https://brutpitt.github.io/myRepos/vGizmo3D/wglCubeExamples.html)
- [vGizmo3D v3.99 WebGL2 - LighedCube example](https://brutpitt.github.io/myRepos/vGizmo3D/wglLightCube.html)

**is necessary to use a browser with **WebGl2** capabilities:: test if your browser supports **WebGL2**, here: [WebGL2 Report](http://webglreport.com/?v=2)*

- [vGizmo3D v3.99 **WGPU / WebGPU** - easy_cube example](https://brutpitt.github.io/myRepos/vGizmo3D/wgpuCube/vGizmo3D_wgpuCube.html)

**is necessary to use a browser with **WebGPU** capabilities: e.g. Chrome-Canary, FireFox Nightly, Safari Technology Preview ...*




<p>&nbsp;<br></p>

## How to use vGizmo3D in your code


- The following code is taken from the new [easy_examples](https://github.com/BrutPitt/virtualGizmo3D/tree/master/easy_examples)

To use **vGizmo3D** need to include `vGizmo3D.h` or `vGizmo.h` file in your code and declare an object of type `vGizmo3D`, global, static or as member of class


## Univocal initialization / framework-independent 

```cpp                       
#include <vGizmo3D.h> // or "vGizmo3D/vGizmo3D.h" ... or where it is 

/// vGizmo3D : declare global/static/member/..
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vg::vGizmo3D track;     // using vGizmo3D 

// vGizmo3D initialize: 
// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initVGizmo3D()     // Settings to control vGizmo3D
{
    // Initialization is necessary to associate your preferences to vGizmo3D
    
    // These are also the DEFAULT values, so if you want to maintain these combinations you can omit they
    // and to override only the associations that you want modify
    
    // Main rotation
        track.setGizmoRotControl         (vg::evButton1  /* or vg::evLeftButton */, 0 /* vg::evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        track.setGizmoRotXControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evShiftModifier);
        track.setGizmoRotYControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evControlModifier);
        track.setGizmoRotZControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evAltModifier | vg::evSuperModifier);
    
    // Set vGizmo3D control for secondary rotation (from v3.1)
        track.setGizmoSecondaryRotControl(vg::evButton2  /* or vg::evRightButton */, 0 /* vg::evNoModifier */ );
        
    // Pan and Dolly/Zoom: mouse button and key modifier
        track.setDollyControl            (vg::evButton2 /* or vg::evRightButton */, vg::evControlModifier);
        track.setPanControl              (vg::evButton2 /* or vg::evRightButton */, vg::evShiftModifier);
    
    // N.B. vg::enums are ONLY mnemonic: select and pass specific vg::enum to framework (that can have also different IDs)

    // passing the screen sizes calibrate drag rotation and auto-set the mouse sensitivity 
        track.viewportSize(width, height);      // is necessary also to call when resize window/surface: re-calibrate drag rotation & auto-set mouse sensitivity
}
```



Now is necessary to control some mouse and key *event* funtions:

## SDL environment
Get Key-Modifier state and return appropriate `vg::enum`
```cpp
/// vGizmo3D: Check key modifier currently pressed (GLFW version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int getModifier(SDL_Window* window = nullptr) {
    SDL_Keymod keyMod = SDL_GetModState();
    if     (keyMod & KMOD_CTRL)     return vg::evControlModifier;
    else if(keyMod & KMOD_SHIFT)    return vg::evShiftModifier;
    else if(keyMod & KMOD_ALT)      return vg::evAltModifier;
    else if(keyMod & KMOD_GUI)      return vg::evSuperModifier;
    else return vg::evNoModifier;
}


```
In main rendering loop intercept mouse button pression/release:
```cpp                     
    // vGizmo3D: check changing button state to activate/deactivate drag movements
    //           (pressing together left/right activate/deactivate both)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        static int leftPress = 0, rightPress = 0;
        int x, y;
        int mouseState = SDL_GetMouseState(&x, &y);
        if(leftPress != (mouseState & SDL_BUTTON_LMASK)) {              // check if leftButton state is changed
            leftPress = mouseState & SDL_BUTTON_LMASK ;                 // set new (different!) state
            track.mouse(vg::evLeftButton, getModifier(sdlWindow),       // send communication to vGizmo3D...
                                          leftPress, x, y);             // ... checking if a key modifier currently is pressed
        }
        if(rightPress != (mouseState & SDL_BUTTON_RMASK)) {             // check if rightButton state is changed
            rightPress = mouseState & SDL_BUTTON_RMASK;                 // set new (different!) state
            track.mouse(vg::evRightButton, getModifier(sdlWindow),      // send communication to vGizmo3D...
                                           rightPress, x, y);           // ... checking if a key modifier currently is pressed
        }
    // if "drag" was activated, "motion" call updates internal rotations (primary and secondary) 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.motion(x,y);

    // Not mandatory: call it every rendering loop only if you want a (possible) continue idle rotation until you do not click on screen
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.idle();   // set continuous rotation on Idle: the slow rotation depends on speed of last mouse movement, (currently) only on primary quaternion / rotation
                        // It can be adjusted from setIdleRotSpeed(1.0) > more speed/sensibility, < less
                        // It can be stopped by click on screen (without mouse movement)

```

## GLFW environment
Get Key-Modifier state and return appropriate `vg::enum`
```cpp
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
```

In main rendering loop intercept mouse button pression/release:
```cpp
    // vGizmo3D: check changing button state to activate/deactivate drag movements
    //           (pressing together left/right activate/deactivate both)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        static int leftPress = 0, rightPress = 0;
        double x, y;
        glfwGetCursorPos(glfwWindow, &x, &y);
        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) != leftPress) {   // check if leftButton state is changed
            leftPress = leftPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;        // set new (different!) state
            track.mouse(vg::evLeftButton, getModifier(glfwWindow),                  // send communication to vGizmo3D...
                                          leftPress, x, y);                         // ... checking if a key modifier currently is pressed
        }
        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) != rightPress) { // same thing for rightButton
            rightPress = rightPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;
            track.mouse(vg::evRightButton, getModifier(glfwWindow),
                                           rightPress, x, y);
        }
    // vGizmo3D: if "drag" active update internal rotations (primary and secondary)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.motion(x,y);
        
    // vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.idle();   // set continuous rotation on Idle: the slow rotation depends on speed of last mouse movement
                        // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                        // It can be stopped by click on screen (without mouse movement)
```   

### GLFW Callbacks
Using GLFW mouse callbacks (in alternative)
```cpp
// onMouseButton callback: any button pressed/released
static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    double x,y;
    glfwGetCursorPos(window, &x, &y);

    int myButton;

    switch(button) {
        case GLFW_MOUSE_BUTTON_1    : // GLFW_MOUSE_BUTTON_LEFT
            myButton = vg::evButton1; // or vg::evButtonLeft
            break;
        case GLFW_MOUSE_BUTTON_2    : // GLFW_MOUSE_BUTTON_RIGHT
            myButton = vg::evButton2; // or vg::evButtonRight
            break;
        case GLFW_MOUSE_BUTTON_3    : // GLFW_MOUSE_BUTTON_MIDDLE
            myButton = vg::evButton3; // or vg::evButtonMiddle
            break;
        default :
            myButton = -1;
    }
    // in GLFW "CURRENTLY" button ID & vg::enums coincide, so all the switch/case statement can be also omitted and pass directly "button" instead of "myButton"
    // in "mouse" call, but is good rule always to check framework IDs (in case of future changes) and select preferred vg::enum
    if(myButton>=0)
        track.mouse((vgButtons) myButton, (vgModifiers) getModifier(glfwWindow), action == GLFW_PRESS, (int)x, (int)y);
}

// onMouseMotion callback: mouse cursor position changed
static void glfwMousePosCallback(GLFWwindow* window, double x, double y)
{
    track.motion(x, y);
}      

// wheel rotation callback: available only via callback (no event are sent by GLFW)
static void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
    track.wheel(x, y);  // now we can use wheel to Dolly/Zoom
}

```

## Build MVP matrix

And finally, before to call the *draw* function, transfer the rotations to build MVP matrix:
```cpp
    // transferring the rotation to cube model matrix cubeObj...
        mat4 modelMatrix = cubeObj * mat4_cast(track.getRotation());

    // Build a "translation" matrix with Pan & Dolly position
        mat4 translationMatrix = translate(mat4(1), track.getPosition());  // add translations (pan/dolly) to an identity matrix

    // build MVPs matrices to pass to shader
        mvpMatrix   = projMatrix * viewMatrix * compensateView * translationMatrix * modelMatrix;
        lightMatrix = projMatrix * viewMatrix * compensateView * translationMatrix * mat4_cast(track.getSecondRot()) * lightObj /* lightModelMatrix */; 
```
*The whole code is taken from the new [easy_examples](https://github.com/BrutPitt/virtualGizmo3D/tree/master/easy_examples)*

<p>&nbsp;<br></p>

### Other useful stuff
Settings functions to override the default values

```cpp
    // other settings if you need it (public member of vGizmo3D) 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    setGizmoFeeling(1.0f)              // set feeling/sensitivity for the rotations (primary and secondary): less < 1.0 < more 
    setDollyScale(1.0f);               // Dolly mouse drag sensitivity: less < 1.0 < more
    setPanScale(1.0f);                 // Pan mouse drag sensitivity: less < 1.0 < more
    setWheelScale(1.0f);               // wheel sensitivity:  less < 1.0 < more
    setIdleRotSpeed(1.0)               // If used Idle() feature (continue rotation on Idle) it set the sensitivity of that speed: less < 1.0 < more
    setDollyPosition(/* your pos */);  // input: float/double or vec3... in vec3 only Z is acquired
    setPanPosition(/* your pos */);    // vec3 ==> only X and Y are acquired
    setPosition(/* your pos */);       // input vec3 is equivalent to call: setDollyPosition(/* your pos */) and setPanPosition(/* your pos */);
    setRotationCenter(/* vec3 */);     // new rotation center, default vec3(0.0)
    
    // there are (obviously) the corresponding all GET functions that return current value 
```

Just a "trick": Using MIDDLE button to rotate two objects togethers (e.g. objModel + lightSpot): simulating a double press (left+right button)   
    
**SDL**
```cpp
        // Simulating a double press (left+right button) using MIDDLE button,
        // sending two "consecutive" activation/deactivation calls to rotate obj-model and light spot together
        if(middlePress != (mouseState & SDL_BUTTON_MMASK)) {             // check if middleButton state is changed
            middlePress = mouseState & SDL_BUTTON_MMASK;                 // set new (different!) middle button state
            track.mouse(vg::evRightButton, getModifier(sdlWindow), middlePress, x, y);  // call Right activation/deactivation with same "middleStatus"
            track.mouse(vg::evLeftButton,  getModifier(sdlWindow), middlePress, x, y);  // call Left  activation/deactivation with same "middleStatus"
        }
        // To put together to other mouse button checks, and before of `track.motion(x,y);` call
 
```
**GLFW**
```cpp
        // Simulating a double press (left+right button) using MIDDLE button,
        // sending two "consecutive" activation/deactivation calls to rotate obj-model and light spot together
        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE) != middlePress) {   // check if middleButton state is changed
            middlePress = middlePress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;        // set new (different!) middle button state
            track.mouse(vg::evLeftButton, getModifier(glfwWindow),  middlePress, x, y); // call Left activation/deactivation with same "middleStatus"
            track.mouse(vg::evRightButton, getModifier(glfwWindow), middlePress, x, y); // call Right activation/deactivation with same "middleStatus"
        }
        // To put together to other mouse button checks, and before of `track.motion(x,y);` call
        // A same method can be used also for the callbacks 
```
*The whole code is contained in the new [easy_examples](https://github.com/BrutPitt/virtualGizmo3D/tree/master/easy_examples)*

**Class declaration**

The include file `vGizmo3D.h` or `vGizmo.h` contains two classes:
- `virtualGizmoClass` simple rotation manipulator, currently **deprecated**, well be removed on next release
- `virtualGizmo3DClass` manipulator with dolly/zoom and pan/shift
- Template classes are also available if configured. &nbsp; **(read below)*

Helper `typedef` are also defined:
```cpp
    using vGizmo    = virtualGizmoClass;    // deprecated
    using vGizmo3D  = virtualGizmo3DClass;
```

#### Ortho projection
- *For the `Ortho` projection example view this [issue: how to use](https://github.com/BrutPitt/virtualGizmo3D/issues/4#issuecomment-2721634222)*

<p>&nbsp;<br></p>

## Configure virtualGizmo3D
**virtalGizmo3D** uses [**vgMath**](https://github.com/BrutPitt/vgMath) tool, it contains a group of vector/matrices/quaternion classes, operators, and principal functions. It uses the "glsl" convention for types and function names so is compatible with **glm** types and function calls: [**vgMath**](https://github.com/BrutPitt/vgMath) is a subset of [**glm** mathematics library](https://github.com/g-truc/glm) and so you can use first or upgrade to second via a simple `#define`. However **vgMath** does not want replicate **glm**, is only intended to make **vGizmo3D** (and [**imguizmo_quat**](https://github.com/BrutPitt/imGuIZMO.quat)) standalone, and avoid use of **template classes** in the cases of low resources or embedded systems.


The file `vgConfig.h` allows to configure internal math used form **virtalGizmo3D**. In particular is possible select between:
 - static **float** classes (*Default*) / template classes 
 - internal **vgMath** tool (*Default*) / **glm** mathematics library
 - **Right** (*Default*) / **Left** handed coordinate system (*lookAt, perspective, ortho, frustum - functions*)
 - **enable** (*Default*) / **disable** automatic entry of `using namespace vgm;` at end of `vgMath.h` (it influences only your external use of `vgMath.h`)
- Add additional HLSL types name convention
 
You can do this simply by commenting / uncommenting a line in `vgConfig.h` or adding related "define" to your project, you can:
       
- Using [template internal vgMath/vGizmo3D classes/types](https://github.com/BrutPitt/virtualGizmo3D/blob/ec31006140293886aa373c0eaa736254c33e1a9f/vGizmo3D/vgConfig.h#L27C1-L42C28)
- Selecting [preferred math library between **glm** or internal **vgMath**](https://github.com/BrutPitt/virtualGizmo3D/blob/ec31006140293886aa373c0eaa736254c33e1a9f/vGizmo3D/vgConfig.h#L60-L73)
- Disabling [*auto-namespace*](https://github.com/BrutPitt/virtualGizmo3D/blob/ec31006140293886aa373c0eaa736254c33e1a9f/vGizmo3D/vgConfig.h#L60-L73), used to switch between glm (glm::) and vgMath (vgm::) 
- Setting [LeftHanded / RightHanded system](https://github.com/BrutPitt/virtualGizmo3D/blob/ec31006140293886aa373c0eaa736254c33e1a9f/vGizmo3D/vgConfig.h#L90-L109) (as default)
- Setting [Z-Buffer range [-1, 1] (OpenGL) or [0, 1] (Vulkan)](https://github.com/BrutPitt/virtualGizmo3D/blob/ec31006140293886aa373c0eaa736254c33e1a9f/vGizmo3D/vgConfig.h#L111-L128) (as default)

<p>&nbsp;<br></p>

#### About glm and vgMath:
- *If your project grows you can switch to **glm**, in any moment: [how to switch to glm](https://github.com/BrutPitt/virtualGizmo3D/blob/ec31006140293886aa373c0eaa736254c33e1a9f/vGizmo3D/vgConfig.h#L60-L73)*
- *My [**glChAoS.P**](https://github.com/BrutPitt/glChAoS.P) project can switch from internal **vgMath** (`VGIZMO_USES_TEMPLATE`) to **glm** (`VGIZMO_USES_GLM`), and vice versa, only changing defines: you can examine it as example*
- *Can also use an optional CMake statement, like this:*
```cmake                                                          
#OFF: use vgMath, ON: force GLM anyway
option(IMGUIZMO_USES_GLM "Use GLM instead of internal vgMath" OFF)

find_package(glm CONFIG)  

# if GLM package was found, then uses GLM (also if option = OFF)
if(glm_FOUND OR IMGUIZMO_USES_GLM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVGIZMO_USES_GLM")
endif()
```


<p>&nbsp;<br></p>

## Building Example - *Still must be updated to version 3.1*

The source code example shown in the animated gif screenshot, is provided.

In  example I use **GLFW** or **SDL2** (via `#define GLAPP_USE_SDL`) with **OpenGL**, but it is simple to change if you use Vulkan/DirectX/etc, other frameworks (like GLUT) or native OS access.

To use SDL framework instead of GLFW, uncomment `#define GLAPP_USE_SDL` in `glApp.h` file, or pass `-DGLAPP_USE_SDL` directly to compiler.
**CMake** users can pass command line `-DUSE_SDL:BOOL=TRUE` (or use relative GUI flag) to enable **SDL** framework instead of **GLFW**.

To build it you can use CMake (3.10 or higher) or the Visual Studio solution project (for VS 2017) in Windows.
You need to have [**GLFW**](https://www.glfw.org/) (or [**SDL**](https://libsdl.org/)) in your compiler search path (LIB/INCLUDE).

The CMake file is able to build also an [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) version, obviously you need to have installed EMSCRIPTEN SDK on your computer (1.38.10 or higher): look at or use the helper batch/script files, in main example folder, to pass appropriate defines/parameters to CMake command line.

To build the EMSCRIPTEN version, in Windows, with CMake, need to have **mingw32-make.exe** in your computer and search PATH (only the make utility is enough): it is a condition of EMSDK tool to build with CMake.

**For windows users that use vs2017 project solution:**
* To build **SDL** or **GLFW**, select appropriate build configuration
* If you have **GLFW** and/or **SDL** headers/library directory paths added to `INCLUDE` and `LIB` environment vars, the compiler find them.
* The current VisualStudio project solution refers to my environment variable RAMDISK (`R:`), and subsequent VS intrinsic variables to generate binary output:
`$(RAMDISK)\$(MSBuildProjectDirectoryNoRoot)\$(DefaultPlatformToolset)\$(Platform)\$(Configuration)\`, so without a RAMDISK variable, executable and binary files are outputted in base to the values of these VS variables, starting from root of current drive. &nbsp;&nbsp; *(you find built binary here... or change it)*
