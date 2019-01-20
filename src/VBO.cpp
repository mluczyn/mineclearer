#include "VBO.h"

PersistentVBO::PersistentVBO(GLuint numBytes) : size(numBytes)
{
	glGenBuffers(1, &(VBOd));
	glBindBuffer(GL_ARRAY_BUFFER, VBOd);
	glBufferStorage(GL_ARRAY_BUFFER, numBytes, 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
	VBOp = (GLbyte*)glMapBufferRange(GL_ARRAY_BUFFER, 0, numBytes, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

PersistentVBO::~PersistentVBO()
{
	bind();
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glDeleteBuffers(1, &VBOd);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PersistentVBO::wait()
{
	if (vboSync)
	{
		GLenum waitReturn;
		do
		{
			waitReturn = glClientWaitSync(vboSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
		} while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED);
	}
	synced = 1;
}

void PersistentVBO::lock()
{
	if (vboSync)
		glDeleteSync(vboSync);
	vboSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	synced = 0;
}

void PersistentVBO::write(GLuint offset, GLuint size, const void* data)
{
	if (!synced)
		wait();
	memcpy(&(VBOp)[offset], data, size);
}

void PersistentVBO::setTextureOffset(GLuint offset) 
{
	textureOffset = offset;
}

void PersistentVBO::setTileTexture(GLuint tile, GLubyte texture)
{
	GLubyte val = texture;
	write(textureOffset + tile, 1, &val);
}

void PersistentVBO::bind()
{
	int x = glGetError();
	glBindBuffer(GL_ARRAY_BUFFER, VBOd);
}