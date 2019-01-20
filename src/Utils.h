#pragma once
#include <memory>
//Texture Values
enum Texture
{
	mine = 8,
	cover = 9,
	flag = 10
};

class TextureUpdateHandler
{
public:
	virtual void setTileTexture(GLuint tile, GLubyte texture) = 0;
	virtual void setAllTileTextures(std::shared_ptr<GLubyte[]> textures) = 0;
};