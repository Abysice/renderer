#ifndef _pngloader_h
#define _pngloader_h

#include <gl/freeglut.h>
#include <gl/gl.h>
#include <png.h>


// USING PNG.LIB EXAMPLES AS REFERENCES
bool loadPngImage(char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData);
void loadTexture(GLuint texture, char* filename);






#endif