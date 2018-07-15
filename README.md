# School-Projects

## Building
* Requires CMake 3.0+
* C++11 compiler
* OpenGL 4.0+

1. Run `CMake ..` from *lab-env/build* folder
2. Build project with your favorite C++11 compiler
3. Executables are added to *lab-env/bin* (Set *lab-env/bin* as working directory if running from a debugger)


## Catmull-Clark Subdivision
Half-edge mesh structure and Catmull-Clark Subdivision.

![Cube subdivided 3 times, normal map as texture](https://github.com/TotalHansson/School-Projects/blob/master/images/half-edge_mesh_CCSubdivision.png)

## Physics
Euler, mid point, and RK4 derivation to simulate physics.  
Line sweep to check for AABB overlap, GJK to check for object overlap, and EPA to get the collision depth and normal.

![Physics collision between two Go stones, AABB and OBB](https://github.com/TotalHansson/School-Projects/blob/master/images/s0009d_fysik.png)

## Skeleton animation and Skinning
A skeleton is loaded from file and each of the joints are linked to its parent.  
The animation is loaded and can be applied to each of the joints.  
The mesh is loaded and each vertex is linked (using weights) with 4 joints.  
Diffuse, normal, and specular texture maps are loaded from DDS files.

Controls include:
* Swap between different animation clips (walk, charge, attack, ...)
* Animation speed
* Run/pause the animation
* Toggle bind pose
* Step through the frames of the selected animation
* Swap between rendering all textures and rendering just one (diffuse, normal, or specular)
* Light options (position, intensity, color)

![Footman in begining of attack animation](https://github.com/TotalHansson/School-Projects/blob/master/images/footman_attack_animation.png)