# Assignment-3-3D Scene Editor
Author : Yichen Xie\
Date: Nov/09/2021

## Assignment Tasks
For the introduction to assignment tasks, see
[Assignment 3: 3D Scene Editor](https://github.com/nyu-cs-cy-6533-fall-2021/base/blob/main/Assignment_3/requirements/Assignment3.md)


## Installation
This project is built by CMAKE. Try:
```shell
> cd build
> cmake ..
> make
```

## Result Generation
You can generate experiment results by running bin file in the project folder:
```shell
{PROJECT_DIR}/build/Assignment3_bin
```

## Instructions

### Add Objects
* The key 1 will add a unit cube in the origin
* The key 2 will import a new copy of "bumpy_cube.off"
* The key 3 will import a new copy of "bunny.off" 

### Object Control
Clicking on a object will select the object, turning its color to bright yellow.

![select](sample/select.png "select")

You can transform the object in multiple ways:

#### Rotation
Press key 'r' will activate the rotation mode. Then use 'w' and 's' to rotate along x axis, 'a' and 'd' along y axis, and 'f' and 'g' along z axis.

#### Translation
Press key 't' will activate the translation mode.Then use 'w' and 's' to translate along x axis, 'a' and 'd' along y axis, and 'f' and 'g' along z axis.

#### Scaling
Using 'q' and 'e' to rescale the object, make it bigger or smaller.

### Camara Control
Press key 'n' to activate the normal mode.

You can change the camara position at any time. Use key 'Up' and 'Down' to move camera along y axis, 'Left' and 'Right' to move along x axis, '-' and '+' to move along z axis.

In the normal mode, you can change the camaratype by 'o' and 'p'. 'o' is for orthographic camera and 'p' is for perspective camera.

Ortho mode:

![ortho](sample/ortho.png "ortho")

Perspective mode:

![perspective](sample/perspective.png "perspective")

You can also press the key 'm' to activate the trackball mode.

Use key 'Up' and 'Down' to move camera vertically, 'Left' and 'Right' to move horizontally, '-' and '+' to zoom in and out.

### Rendering Mode
By pressing 'z' or 'x' or 'c', you can change the rendering mode of selected object into 'Wireframe mode' or 'Flat mode' or 'Phong mode'.

Wireframe mode:

![wire](sample/wire.png "wire")

Flat mode:

![flat](sample/flat.png "flat")

Phong mode:

![phong](sample/phong.png "phong")