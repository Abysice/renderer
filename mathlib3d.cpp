/*
 * Adam Bysice - 0753709
 * File: mathlib2d.cpp
 * --------------------
 * This file contains the implementation
 * for the mathlib2D classes.
 */

#define _USE_MATH_DEFINES 
#include <math.h>
#include "mathlib3d.h"

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))


point3D::point3D (float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

point3D::point3D() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

point2Di::point2Di (int x, int y) {
	this->x = x;
	this->y = y;
}

point2Di:: point2Di () {
	this->x = 0;
	this->y = 0;
}

vec3D::vec3D (float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->magnitude = sqrt( (this->x*this->x) + (this->y*this->y) + (this->z*this->z));
}

vec3D::vec3D () {
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->magnitude = 0;
}

float vec3D::length() {
	this->magnitude = sqrt((x*x) + (y*y) + (z*z));
	return magnitude;
}

// returns cross product of a vector and an argument
vec3D vec3D::cross(const vec3D& v) const
{
    float ax = y * v.z - z * v.y;
    float ay = z * v.x - x * v.z;
    float az = x * v.y - y * v.x;

    return vec3D(ax, ay, az);
}

// normalizes a vector
vec3D& vec3D::normalize()
{
    this->length();
    this->x /= magnitude;
    this->y /= magnitude;
    this->z /= magnitude;

    return *this;
}

void vec3D::Rotate(float Angle, const vec3D& Axe)
{
    const float SinHalfAngle = sinf(ToRadian(Angle/2));
    const float CosHalfAngle = cosf(ToRadian(Angle/2));

    const float Rx = Axe.x * SinHalfAngle;
    const float Ry = Axe.y * SinHalfAngle;
    const float Rz = Axe.z * SinHalfAngle;
    const float Rw = CosHalfAngle;
    Quaternion RotationQ(Rx, Ry, Rz, Rw);

    Quaternion ConjugateQ = RotationQ.Conjugate();
    Quaternion W = RotationQ * (*this) * ConjugateQ;

    x = W.x;
    y = W.y;
    z = W.z;
}

colour3D::colour3D(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->alpha = a;
}

colour3D::colour3D() {
	this->r = 0;
	this->g = 0;
	this->b = 0;
	this->alpha = 1;
}

float gdistance(point3D p1, point3D p2) {
	float d = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) + (p2.z - p1.z)*(p2.z - p1.z));
	return d;
}

//same as distance, except without the square root, usefull for relative distance
float fdistance(point3D p1, point3D p2) {
	float d = (p2.x - p1.x)*(p2.x - p1.x) + (p2.x - p1.x)*(p2.x - p1.x + (p2.z - p1.z)*(p2.z - p1.z));
	return d;
}

//takes two points, p1 = (x1, y1) and p2 = (x2, y2), and returns the vector
vec3D createVec(point3D p1, point3D p2) {
	vec3D someVec(p2.x - p1.x,  p2.y - p1.y, p2.z - p1.z);
	return someVec;
}

// cross product function
vec3D crossVec(vec3D v1, vec3D v2) {
	vec3D cross(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
	return cross;

}
// dot product function
float dotVec(vec3D v1, vec3D v2) {
	float dot = (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
	return dot;
}


//takes a point, p, and a vector, v, and returns the point (p + v)
point3D movePoint(point3D p1, vec3D v1) {
	point3D vecMov(p1.x + (v1.x*v1.magnitude), p1.y + (v1.y*v1.magnitude), p1.z + (v1.z*v1.magnitude)); 
	return vecMov;
}

point3D rotateX(point3D p1, float angle) {
	float radangle = ToRadian(angle);
	point3D newPoint(1*p1.x + 0*p1.y + 0*p1.z,
					0*p1.x + cos(radangle)*p1.y + -sin(radangle)*p1.z, 
					0*p1.x + sin(radangle)*p1.y + cos(radangle)*p1.z );
	return newPoint;
}

point3D rotateY(point3D p1, float angle) {
	float radangle = ToRadian(angle);
	point3D newPoint(cos(radangle)*p1.x + 0*p1.y + sin(radangle)*p1.z, 
					0*p1.x + 1*p1.y + 0*p1.z ,
					-sin(radangle)*p1.x + 0*p1.y + cos(radangle)*p1.z);
	return newPoint;
}

point3D rotateZ(point3D p1, float angle) {
	float radangle = ToRadian(angle);
	point3D newPoint(cos(radangle)*p1.x + -sin(radangle)*p1.y + 0*p1.z, 
					sin(radangle)*p1.x + cos(radangle)*p1.y + 0*p1.z ,
					0*p1.x + 0*p1.y + 1*p1.z);
	return newPoint;
}

// quaternion class constructor
Quaternion::Quaternion(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

// normalize the quaternion
void Quaternion::Normalize()
{
    float Length = sqrtf(x * x + y * y + z * z + w * w);

    x /= Length;
    y /= Length;
    z /= Length;
    w /= Length;
}

// return the conjugate of the quaternion
Quaternion Quaternion::Conjugate()
{
    Quaternion ret(-x, -y, -z, w);
    return ret;
}

// operator implementation
Quaternion operator*(Quaternion& l, Quaternion& r)
{
    const float w = (l.w * r.w) - (l.x * r.x) - (l.y * r.y) - (l.z * r.z);
    const float x = (l.x * r.w) + (l.w * r.x) + (l.y * r.z) - (l.z * r.y);
    const float y = (l.y * r.w) + (l.w * r.y) + (l.z * r.x) - (l.x * r.z);
    const float z = (l.z * r.w) + (l.w * r.z) + (l.x * r.y) - (l.y * r.x);

    Quaternion ret(x, y, z, w);

    return ret;
}

Quaternion operator*(Quaternion& q,vec3D& v)
{
    const float w = - (q.x * v.x) - (q.y * v.y) - (q.z * v.z);
    const float x =   (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
    const float y =   (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
    const float z =   (q.w * v.z) + (q.x * v.y) - (q.y * v.x);

    Quaternion ret(x, y, z, w);

    return ret;
}

