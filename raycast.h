#ifndef _raycast_h
#define _raycast_h

#include "mathlib3d.h"
#include <gl/freeglut.h>
#include <gl/gl.h>

bool rayCast(int x, int y,point3D corner,vec3D boxSize,point3D campos, vec3D rotation);
void Get3DPos(int x, int y, float winz, GLdouble point[3]);
bool rayPlaneTest(vec3D newRayDir,vec3D newRayOrigin, vec3D groundPlaneNorm, float inter[], float &d,point3D corner,vec3D boxSize,vec3D rotation);
bool rayBoxTest(vec3D newRayDir, vec3D newRayOrigin , float inter[], point3D corner, vec3D boxSize, vec3D rotation);

#endif