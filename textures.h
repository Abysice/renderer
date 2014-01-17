#ifndef _textures_h
#define _textures_h

#include <stdlib.h>
#include "mathlib3d.h"
#include <vector>
using namespace std;

//material class
class material {
public:
	material();

	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shinyness;

};

//mesh class
class mesh {
public:
	mesh();
	void Vadd(float *p);
	void Nadd(float *c);
	void UVadd(float *uv);
	void Eadd(int *n);
	void UVEadd(int *u);
	void ENadd(int *n);
	void Vdump();
	
	float* vertexArray;
	float* colourArray;
	float* normalArray;
	float* UVArray;
	int meshsize;

private:
	vector<float*> vertices; 
	vector<float*> colour;
	vector<float*> normals;
	vector<float*> texcoords;
	vector<int*> UVedge; // uv cord vertex indices
	vector<int*> edges;
	vector<int*> Nedges;

};

#endif