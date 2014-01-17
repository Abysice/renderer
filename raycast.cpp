#include "mathlib3d.h"
#include <gl/freeglut.h>
#include <gl/gl.h>
#include "raycast.h"



bool rayCast(int x, int y,point3D corner,vec3D boxSize, point3D campos,vec3D rotation) {
	GLdouble pNear[3];
	GLdouble pFar[3]; 
	float inter[3]; // intersection point

	//get 3D position of mouse cursor on near and far clipping planes
	Get3DPos(x, y, 0.0, pNear);
	Get3DPos(x, y, 1.0, pFar);
	
	//create a ray originating at the camera position, and using the vector between the two points for the direction
	//vec3D newRayOrigin(myCam->m_pos.x, myCam->m_pos.y, myCam->m_pos.z);
	vec3D newRayOrigin(campos.x,campos.y,campos.z);

	//ray direction is the vector (pFar - pNear)
	vec3D newRayDir(pFar[0] - newRayOrigin.x,pFar[1] - newRayOrigin.y,pFar[2] - newRayOrigin.z);
	
	//normalize the ray direction
	newRayDir.length();
	newRayDir.normalize();
	//find d (normal of plane coords * point on plane coords * -1
	//test + find the point of intersection
	if (rayBoxTest(newRayDir, newRayOrigin , inter,  corner, boxSize, rotation)) {
		return true; 
	}
	else {
		return false;
	}

}

// takes a defining corners and tests if its in the box
bool rayBoxTest(vec3D newRayDir, vec3D newRayOrigin , float inter[], point3D corner,vec3D boxSize,vec3D rotation) {
	//calculate points from from size + corner point
	point3D p1 = corner;
	point3D p2(corner.x - boxSize.x, corner.y, corner.z);
	point3D p3(corner.x, corner.y - boxSize.y, corner.z);
	point3D p4(corner.x - boxSize.x, corner.y - boxSize.y, corner.z);
	point3D p5(corner.x, corner.y,corner.z- boxSize.z);
	point3D p6(corner.x - boxSize.x, corner.y, corner.z- boxSize.z); 
	point3D p7(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); 
	point3D p8(corner.x- boxSize.x, corner.y - boxSize.y, corner.z-boxSize.z);
	float d;
	
	//calculate vectors and normal for each face
	vec3D v12 = createVec(p1,p2);
	vec3D v13 = createVec(p1,p3);
	vec3D v56 = createVec(p5,p6);
	vec3D v57 = createVec(p5,p7);
	vec3D v15 = createVec(p1,p5);
	vec3D v34 = createVec(p3,p4);
	vec3D v37 = createVec(p3,p7);
	vec3D v68 = createVec(p6,p8);
	vec3D v48 = createVec(p4,p8);
	
	//find the normal for each face + normalize
	vec3D norm = crossVec(v12,v13);		norm.normalize();
	vec3D norm2 = crossVec(v56,v57);	norm2.normalize();
	vec3D norm3 = crossVec(v15, v12);	norm3.normalize();
	vec3D norm4 = crossVec(v34,v37);	norm4.normalize();
	vec3D norm5 = crossVec(v15,v13);	norm5.normalize();
	vec3D norm6 = crossVec(v68, v48);	norm6.normalize();
	
	
	//front face check
	d = -1*(norm.x*p1.x +norm.y*p1.y + norm.z*p1.z);
	if (rayPlaneTest(newRayDir,newRayOrigin, norm, inter, d, corner, boxSize,rotation))
		return true;
	//find the normal for back 
	d = -1*(norm2.x*p5.x + norm2.y*p5.y + norm2.z*p5.z);
	if (rayPlaneTest(newRayDir,newRayOrigin, norm, inter, d, corner, boxSize,rotation))
		return true;
	//top normal
	d = -1*(norm3.x*p1.x +norm3.y*p1.y + norm3.z*p1.z);
	if (rayPlaneTest(newRayDir,newRayOrigin, norm3, inter, d, corner, boxSize,rotation))
		return true;
	//bottom normal
	d = -1*(norm4.x*p3.x +norm4.y*p3.y + norm4.z*p3.z);
	if (rayPlaneTest(newRayDir,newRayOrigin, norm4, inter, d, corner, boxSize,rotation))
		return true;
	//side 1
	d = -1*(norm5.x*p1.x +norm5.y*p1.y + norm5.z*p1.z);
	if (rayPlaneTest(newRayDir,newRayOrigin, norm5, inter, d, corner, boxSize,rotation))
		return true;
	//side 2
	d = -1*(norm6.x*p8.x + norm2.y*p8.y + norm2.z*p8.z);
	if (rayPlaneTest(newRayDir,newRayOrigin, norm6, inter, d, corner, boxSize,rotation))
		return true;
	
	return false;
}

bool rayPlaneTest(vec3D newRayDir, vec3D newRayOrigin ,vec3D groundPlaneNorm,float inter[],float &d, point3D corner,vec3D boxSize, vec3D rotation) {

	//first compute N dot Ray direction, if 0, no intersection because parallel
	if (dotVec(groundPlaneNorm,newRayDir) == 0) {
		inter[0] = 1000;
		inter[1] = 1000;
		inter[2] = 1000;
		return false;
	}

	//since point on the ray = rayorg + t*raydirection
	//solve for t such that POI = rayorg + t*raydirection
	float t = -1*( ( dotVec(groundPlaneNorm,newRayOrigin) + d) / dotVec(groundPlaneNorm, newRayDir) );

	// intersection point = newRayOrigin + t*newRayDir
	vec3D poi = (newRayOrigin + (newRayDir*t));
	point3D fpoi = rotateX(point3D(poi.x,poi.y,poi.z),-rotation.x);
	fpoi = rotateY(fpoi, -rotation.y);
	fpoi = rotateZ(fpoi, -rotation.z);
	inter[0] = fpoi.x;
	inter[1] = fpoi.y;
	inter[2] = fpoi.z;

		//preform a check on that face that POI is within the face
	if ( ((inter[0] <= corner.x && inter[0] >= corner.x - boxSize.x) || (inter[0] <= corner.x - boxSize.x && inter[0] >= corner.x  ))
		&& ((inter[1] <= corner.y && inter[1] >= corner.y - boxSize.y)  || (inter[1] <= corner.y - boxSize.y && inter[1] >= corner.y ))
		&& ((inter[2] <= corner.z && inter[2] >= corner.z - boxSize.z)) || (inter[2] <= corner.z - boxSize.z && inter[2] >= corner.z ))
		{ 
			//then it is in the 1 face
			return true;
		}
	else {
		// not in the face
		return false;
	}
	
}

void Get3DPos(int x, int y, float winz, GLdouble point[3])
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	
	//get the matrices
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );
 
	//"un-project" the 2D point giving the 3D position corresponding to the provided depth (winz)
	gluUnProject( (float)(x), (float)(viewport[3]-y), winz, modelview, projection, viewport, &point[0], &point[1], &point[2]);
}


