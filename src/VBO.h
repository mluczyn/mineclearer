#pragma once
#include <cstring>
#include <GL/glew.h>
#include "Utils.h"

class PersistentVBO : public TextureUpdateHandler
{
public:
	PersistentVBO(GLuint numBytes);
	~PersistentVBO();
	void lock();
	void bind();
	void write(GLuint offset, GLuint size, const void* data);
	void setTextureOffset(GLuint offset);
	void setTileTexture(GLuint tile, GLubyte texture);
private:
	void wait();
	GLuint size;
	GLuint textureOffset;
	GLuint VBOd;
	GLsync vboSync;
	GLbyte* VBOp;
	bool synced = 1;
};
