#include "parser.h"
#include "textures.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace std;
float* vertex;
int* Evertex;
float* Nvertex;
float* UVtex;

mesh parse(string filename) {

	mesh objectMesh;

	ifstream file;
	file.open(filename);
	string line;
	int counter;

	cout << "Loading Mesh: " << filename << endl;
	while(!file.eof()) {
		getline(file, line);
		
		string word[9];
		

		// if line = vertex
		if (line[0] == 'v' && line[1] == ' ') {
			//reset 
			word[0] = "";
			word[1] = "";
			word[2] = "";
			counter = -1;
			//parse line
			for(int i = 1; i < line.size(); i++) {
				// if space, next word
				if (line[i] == ' ') 
					counter++;
				//add character to word
				if (line[i] != ' ' && counter < 3) 
					word[counter] += line[i];
			}
			// add line to array
			vertex = new float[3];
			vertex[0] = stof(word[0]);
			vertex[1] = stof(word[1]);
			vertex[2] = stof(word[2]);
			//cout << vertex[0] << " " << vertex[1] << " "<< vertex[2] << endl;
			objectMesh.Vadd(vertex);
				
		}
		
		//Normal parsing
		else if (line[0] == 'v' && line[1] == 'n') {
			//reset 
			word[0] = "";
			word[1] = "";
			word[2] = "";
			
			counter = -1;
			//parse line
			for(int i = 2; i < line.size(); i++) {
				// if space, next word
				if (line[i] == ' ' ) 
					counter++;
				//add character to word
				if (line[i] != ' ' && counter < 3) 
					word[counter] += line[i];
			}
			// add line to array
			Nvertex = new float[3];
			Nvertex[0] = stof(word[0]);
			Nvertex[1] = stof(word[1]);
			Nvertex[2] = stof(word[2]);
			objectMesh.Nadd(Nvertex);
		}

		// UV coord parsing
		else if (line[0] == 'v' && line[1] == 't') {
			//reset 
			word[0] = "";
			word[1] = "";
			word[2] = "";			
			counter = -1;
			//parse line
			for(int i = 2; i < line.size(); i++) {
				// if space, next word
				if (line[i] == ' ' ) 
					counter++;
				//add character to word
				if (line[i] != ' ' && counter < 2) 
					word[counter] += line[i];
			}
			// add line to array
			UVtex = new float[2];
			UVtex[0] = stof(word[0]);
			UVtex[1] = stof(word[1]);
			objectMesh.UVadd(UVtex);
		} 
		
		// edge importer
		else if (line[0] == 'f' && line[1] == ' ') {
			//reset 
			word[0] = "";
			word[1] = "";
			word[2] = "";
			word[3] = "";
			word[4] = "";
			word[5] = "";
			word[6] = "";
			word[7] = "";
			word[8] = "";
			counter = -1;
			//parse line
			for(int i = 1; i < line.size(); i++) {
				
				// if space, next word
				if ((line[i] == ' ' ) || (line[i] == '/')) 
					counter++;
				//add character to word
				if (line[i] != ' '  &&  line[i] != '/') 
					word[counter] += line[i];
			}
			// add line to array
			Evertex = new int[3];
			Evertex[0] = stoi(word[0]);
			Evertex[1] = stoi(word[3]);
			Evertex[2] = stoi(word[6]);
			Evertex[0] -= 1;
			Evertex[1] -= 1;
			Evertex[2] -= 1;
			
			objectMesh.Eadd(Evertex);
			
			Evertex = new int[3];
			Evertex[0] = stoi(word[2]);
			Evertex[1] = stoi(word[5]);
			Evertex[2] = stoi(word[8]);
			Evertex[0] -= 1;
			Evertex[1] -= 1;
			Evertex[2] -= 1;
			objectMesh.ENadd(Evertex);
			
						
			Evertex = new int[3];
			Evertex[0] = stoi(word[1]);
			Evertex[1] = stoi(word[4]);
			Evertex[2] = stoi(word[7]);
			Evertex[0] -= 1;
			Evertex[1] -= 1;
			Evertex[2] -= 1;
			objectMesh.UVEadd(Evertex);

		}
	}
	
	objectMesh.Vdump();
	
	return objectMesh;
}
