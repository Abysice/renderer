#ifndef _object_h
#define _object_h

#include "mathlib3d.h"
#include "textures.h"

//particle superclass, uses other classes to represent particles
class object3D {
public:
	//constructors
	object3D();
	object3D(float x, float y, float z);
	object3D(point3D pos, vec3D size, vec3D rotation, point3D corner, vec3D boxSize);

	// internal variables
	point3D position; //relative position for object
	vec3D size;	// depth, breadth and height of hitbox
	vec3D scale; //scale modifier
	vec3D rotation; //rotation for the mesh, not the object
	mesh objmesh;
	material mat;
	int textures;
};


#endif
