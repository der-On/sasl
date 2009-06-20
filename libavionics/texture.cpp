#include "texture.h"
#include "glheaders.h"

#include "utils.h"
#include "luna.h"
#include "avionics.h"
#include "texloader.h"


using namespace xa;


TextureManager textureManager;



Texture::Texture(): id(0)
{
    binderCallback = NULL;
}

Texture::Texture(int id, xa_bind_texture_2d_callback binder): id(id)
{
    binderCallback = binder;
    bind();
    
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width); 
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height); 
}

Texture::~Texture()
{
    glDeleteTextures(1, (const GLuint*)&id);
}

void Texture::bind()
{
    if (binderCallback)
        binderCallback(id);
    else
        glBindTexture(GL_TEXTURE_2D, id);
}




TexturePart::TexturePart(Texture *tex, double x1, double y1, 
        double x2, double y2)
{
    texture = tex;
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}



TextureManager::TextureManager()
{
    binderCallback = NULL;
    genTexNameCallback = NULL;
}

TextureManager::~TextureManager()
{
    unloadAll();
}



Texture* TextureManager::loadImage(const std::string &fileName)
{
    TexturesMap::iterator i = cache.find(fileName);
    if (i != cache.end()) {
        return (*i).second;
    } else {
        GLuint id = 0;
        if (genTexNameCallback)
            id = genTexNameCallback();
        else
            glGenTextures(1, &id);
        id = loadTexture(fileName.c_str(), id);
        if (! id)
            return NULL;
        
        // it is wrong, but i don't want patch SOIL
        if (binderCallback)
            binderCallback(id);
        else
            glBindTexture(GL_TEXTURE_2D, id);

        Texture *tex = new Texture(id, binderCallback);
        cache[fileName] = tex;
        return tex;
    }
}


std::string TextureManager::getPartName(const std::string& fileName, 
        double x, double y, double width, double height)
{
    return fileName + ":" + toString(x) + ";" + toString(y) + ";" +
        toString(width) + ":" + toString(height);
}

void TextureManager::getPartCoords(Texture *texture, double &x1, double &y1,
        double &x2, double &y2)
{
    x1 = y1 = 0.0;
    x2 = y2 = 1.0;
}
        
void TextureManager::getPartCoords(Texture *texture, 
        double width, double height,
        double &x1, double &y1, double &x2, double &y2)
{
    double imageWidth = texture->getWidth();
    double imageHeight = texture->getHeight();
    x1 = ((imageWidth - width) / 2.0) / imageWidth;
    y1 = ((imageHeight - height) / 2.0) / imageHeight;
    x2 = ((imageWidth + width) / 2.0) / imageWidth;
    y2 = ((imageHeight + height) / 2.0) / imageHeight;
}


void TextureManager::getPartCoords(Texture *texture, 
        double x, double y, double width, double height,
        double &x1, double &y1, double &x2, double &y2)
{
    double imageWidth = texture->getWidth();
    double imageHeight = texture->getHeight();
    x1 = x / imageWidth;
    y1 = y / imageHeight;
    x2 = (x + width) / imageWidth;
    y2 = (y + height) / imageHeight;
}

TexturePart* TextureManager::getTexturePart(const std::string &fileName, 
        Texture *texture, double x1, double y1, double x2, double y2)
{
    std::string partName = getPartName(fileName, x1, y1, x2, y2);
    TexturesParts::iterator i = parts.find(partName);
    if (i != parts.end())
        return (*i).second;
    else {
        TexturePart *part = new TexturePart(texture, x1, y1, x2, y2);
        parts[partName] = part;
        return part;
    }
}

TexturePart* TextureManager::load(const std::string &fileName, 
        double x, double y, double width, double height)
{
    Texture *tex = loadImage(fileName);
    if (! tex)
        return NULL;
    else {
        double x1, y1, x2, y2;
        getPartCoords(tex, x, y, width, height, x1, y1, x2, y2);
        return getTexturePart(fileName, tex, x1, y1, x2, y2);
    }
}

TexturePart* TextureManager::load(const std::string &fileName, 
        double width, double height)
{
    Texture *tex = loadImage(fileName);
    if (! tex)
        return NULL;
    else {
        double x1, y1, x2, y2;
        getPartCoords(tex, width, height, x1, y1, x2, y2);
        return getTexturePart(fileName, tex, x1, y1, x2, y2);
    }
}

TexturePart* TextureManager::load(const std::string &fileName)
{
    Texture *tex = loadImage(fileName);
    if (! tex)
        return NULL;
    else {
        double x1, y1, x2, y2;
        getPartCoords(tex, x1, y1, x2, y2);
        return getTexturePart(fileName, tex, x1, y1, x2, y2);
    }
}


void TextureManager::unloadAll()
{
    for (TexturesParts::iterator i = parts.begin(); i != parts.end(); i++)
        delete (*i).second;
    parts.clear();

    for (TexturesMap::iterator i = cache.begin(); i != cache.end(); i++)
        delete (*i).second;
    cache.clear();
}

void TextureManager::setBinder(xa_bind_texture_2d_callback c)
{
    binderCallback = c;
}

void TextureManager::setTexNameGenerator(xa_gen_tex_name_callback c)
{
    genTexNameCallback = c;
}

/// Lua wrapper for texture manager
static int luaLoadImage(lua_State *L)
{
    TextureManager *textureManager = getAvionics(L)->getTextureManager();

    std::string fileName = lua_tostring(L, 1);
    TexturePart *texture = NULL;

    if (lua_isnil(L, 2)) 
        texture = textureManager->load(fileName);
    else if (lua_isnil(L, 4))
        texture = textureManager->load(fileName, 
                lua_tonumber(L, 2), lua_tonumber(L, 3));
    else
        texture = textureManager->load(fileName, 
                lua_tonumber(L, 2), lua_tonumber(L, 3),
                lua_tonumber(L, 4), lua_tonumber(L, 5));
    
    if (texture)
        lua_pushlightuserdata(L, texture);
    else
        lua_pushnil(L);
    return 1;
}

/// Returns size of texture in pixels
static int luaGetTextureSize(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;
    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);

    lua_pushnumber(L, (tex->getX2() - tex->getX1()) * 
            tex->getTexture()->getWidth());
    lua_pushnumber(L, (tex->getY2() - tex->getY1()) * 
            tex->getTexture()->getHeight());
    return 2;
}


void xa::exportTextureToLua(Luna &lua)
{
    lua_State *L = lua.getLua();

    lua_register(L, "getGLTexture", luaLoadImage);
    lua_register(L, "getTextureSize", luaGetTextureSize);
}

