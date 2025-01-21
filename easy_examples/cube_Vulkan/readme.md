# vGizmo3D Vulkan easy examples
These *easy* examples was written using `vulkan.hpp` / vulkan-hpp      

*Page work in progres: other example coming soon

They are similar to OpenGL examples and to [vGizmo3D v3.1 WebGL2 - easy_cube example](https://brutpitt.github.io/myRepos/vGizmo3D/wglCubeExamples.html)

<p>&nbsp;<br></p>
In the OpenGL version of the examples, it was opted to separate the GLFW and SDL versions in distinct files.

In Vulkan, given the greater complexity of the graphical interface and the integration of the framwork with the creation of the surface, there is a single example with CMake compiler options for GLFW / SDL2 / SDL3.
- GLFW is the DEFAULT option
- `cmake -DUSE_SDL2=ON` builds for SDL2
- `cmake -DUSE_SDL3=ON` builds for SDL3
*obviously is necessary to have GLFW/SDL2/SDL3 (devel package) installed*

They were encapsulated in a `framwork` class and its subclasses (files `framework.cpp` and `framework.h`)

Here is the initialization of vGizmo3D (not dependent on framework: `frameworkBase class`) and the management of the mouse for each specific framework.

The `vkCube` code is divided into (main) 2 parts:
- `vkAppBase class` the bulk of Vulkan initialization 
- `vkApp class` draw and manipulate scene objects


## Different representation of the 3D space
Despite a different representation of the 3D space between Vulkan and OpenGL/Webgl, there are no changes in the use of vGizmo3D 

Obviously the representation of the model changes and you have to choose a different Projection Matrix for Vulkan.
I invite to read:

[Setting Up a Proper Projection Matrix for Vulkan](https://johannesugb.github.io/gpu-programming/setting-up-a-proper-vulkan-projection-matrix/) 


In `vkCube` example I have used a *clip-Matrix*, like a clip-space of previous link.

Here Nvidia uses a similar technique, but have [only `y` inverted and "half" `z` ](https://github.com/KhronosGroup/Vulkan-Hpp/blob/ce439adae242d54d2e10425ce103bad895dde291/samples/utils/math.cpp#L43-L47)


In the In `vkCube_invZ` ... coming soon