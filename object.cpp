#include "object.h"
#include "mathlib3d.h"
#include "textures.h"


object3D::object3D() {
	this->position = point3D();
	this->size = vec3D();
	this->rotation = vec3D();
	this->objmesh = mesh();
}

object3D::object3D(float x, float y, float z) {
	this->position = point3D(x,y,z);
	this->size = vec3D(1,1,1);
	this->scale = vec3D(1,1,1);
	this->rotation = vec3D(0,0,0);
	this->objmesh = mesh();
}

object3D::object3D(point3D pos, vec3D size, vec3D rotation,  point3D corner, vec3D boxSize ) {
	this->position = pos;
	this->size = size;
	this->rotation = rotation;
	this->objmesh = mesh();
}