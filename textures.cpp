#include "textures.h"


mesh::mesh() {
	vertices.clear();
	colour.clear();
	texcoords.clear();
	edges.clear();
	normals.clear();
	UVedge.clear();

	vertexArray = new float[0];
	normalArray = new float[0];
	colourArray = new float[0];
	UVArray = new float[0];
}

void mesh::Vadd(float *p) {
	vertices.push_back(p);

}
void mesh::Nadd(float *p) {
	normals.push_back(p);

}
void mesh::UVadd(float *uv) {
	texcoords.push_back(uv);

}
void mesh::Eadd(int *p) {
	edges.push_back(p);

}

void mesh::ENadd(int *p) {
	Nedges.push_back(p);

}

void mesh::UVEadd(int *u) {
	UVedge.push_back(u);
}


void mesh::Vdump() {

	meshsize = 0;

	vertexArray = new float[edges.size()*9];
	// dump vertices based on edges
	for(int i = 0; i < edges.size(); i++) {
			vertexArray[i*9] = vertices.at(edges.at(i)[0])[0];
			vertexArray[i*9+1] = vertices.at(edges.at(i)[0])[1];
			vertexArray[i*9+2] = vertices.at(edges.at(i)[0])[2];
			vertexArray[i*9+3] = vertices.at(edges.at(i)[1])[0];
			vertexArray[i*9+4] = vertices.at(edges.at(i)[1])[1];
			vertexArray[i*9+5] = vertices.at(edges.at(i)[1])[2];
			vertexArray[i*9+6] = vertices.at(edges.at(i)[2])[0];
			vertexArray[i*9+7] = vertices.at(edges.at(i)[2])[1];
			vertexArray[i*9+8] = vertices.at(edges.at(i)[2])[2];
			
	}
	normalArray = new float[Nedges.size()*9];
	// dump normals based on edges
	for(int i = 0; i < Nedges.size(); i++) {

			normalArray[i*9] = normals.at(Nedges.at(i)[0])[0];
			normalArray[i*9+1] = normals.at(Nedges.at(i)[0])[1];
			normalArray[i*9+2] = normals.at(Nedges.at(i)[0])[2];
			normalArray[i*9+3] = normals.at(Nedges.at(i)[1])[0];
			normalArray[i*9+4] = normals.at(Nedges.at(i)[1])[1];
			normalArray[i*9+5] = normals.at(Nedges.at(i)[1])[2];
			normalArray[i*9+6] = normals.at(Nedges.at(i)[2])[0];
			normalArray[i*9+7] = normals.at(Nedges.at(i)[2])[1];
			normalArray[i*9+8] = normals.at(Nedges.at(i)[2])[2];
			
	}
	UVArray = new float[UVedge.size()*6];
	//dump UV coords based on edges
	for(int i = 0; i < UVedge.size(); i++) {

			UVArray[i*6] = texcoords.at(UVedge.at(i)[0])[0];
			UVArray[i*6+1] = texcoords.at(UVedge.at(i)[0])[1];
			//UVArray[i*6+2] = texcoords.at(UVedge.at(i)[0])[2];
			UVArray[i*6+2] = texcoords.at(UVedge.at(i)[1])[0];
			UVArray[i*6+3] = texcoords.at(UVedge.at(i)[1])[1];
			//UVArray[i*6+5] = texcoords.at(UVedge.at(i)[1])[2];
			UVArray[i*6+4] = texcoords.at(UVedge.at(i)[2])[0];
			UVArray[i*6+5] = texcoords.at(UVedge.at(i)[2])[1];
			//UVArray[i*9+8] = texcoords.at(UVedge.at(i)[2])[2];
			
	}
	

	//dump dummy colors
	colourArray = new float[edges.size()*9];
	for(int i = 0; i < edges.size()*3; i++) {
		colourArray[i*3] = 0.5;
		colourArray[(i*3)+1] = 0.5;
		colourArray[(i*3)+2] = 0.5;

	}
	meshsize = edges.size()*3;

}

//material class
material::material() {
	
}