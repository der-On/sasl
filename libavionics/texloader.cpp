#include "texloader.h"
#include <SOIL.h>
#include "glheaders.h"

using namespace xa;

unsigned int xa::loadTexture(const char *name, GLuint texId, GLint *width, GLint *height)
{
    unsigned id = SOIL_load_OGL_texture(name, 0, texId, SOIL_FLAG_POWER_OF_TWO);
    if (! id)
        return 0;

    if (width)
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, width);
    if (height)
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, height);

    return id;
}

