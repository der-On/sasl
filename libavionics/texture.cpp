#include "texture.h"

#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "luna.h"
#include "avionics.h"


using namespace xa;



Texture::Texture(int id, int width, int height, TextureManager *manager):
    id(id), width(width), height(height), manager(manager)
{
    managed = true;
}

Texture::Texture(int id, TextureManager *manager):
    id(id), manager(manager)
{
    managed = false;
    manager->getGraphics()->free_texture(manager->getGraphics(), id);
}

Texture::~Texture()
{
    if (managed)
        manager->getGraphics()->free_texture(manager->getGraphics(), id);
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
    buffer = NULL;
    bufLength = 0;
}

TextureManager::~TextureManager()
{
    unloadAll();
    if (buffer) {
        free(buffer);
        buffer = NULL;
    }
}



Texture* TextureManager::loadImage(const unsigned char *buffer, int length)
{
    int width, height;
    int id = graphics->load_texture(graphics, (const char*)buffer, length, 
            &width, &height);
    if (-1 == id)
        return NULL;
    
    Texture *tex = new Texture(id, width, height, this);
    loaded.push_back(tex);
    return tex;
}

Texture* TextureManager::loadImage(const std::string &fileName)
{
    TexturesMap::iterator i = cache.find(fileName);
    if (i != cache.end()) {
        return (*i).second;
    } else {
        FILE *f = fopen(fileName.c_str(), "rb");
        if (! f)
            return NULL;
        if (fseek(f, 0, SEEK_END)) {
            fclose(f);
            return NULL;
        }
        int size = ftell(f);
        if (0 >= size) {
            fclose(f);
            return NULL;
        }
        if (fseek(f, 0, SEEK_SET)) {
            fclose(f);
            return NULL;
        }
        if (! buffer) {
            buffer = (unsigned char*)malloc(size);
            bufLength = size;
        } else if (size > bufLength) {
            buffer = (unsigned char*)realloc(buffer, size);
            bufLength = size;
        }
        if (! buffer) {
            fclose(f);
            return NULL;
        }
        int res = fread(buffer, 1, size, f);
        fclose(f);
        if (res != size) {
            return NULL;
        }
        Texture *tex = loadImage(buffer, size);
        if (tex)
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

TexturePart* TextureManager::getTexturePart(Texture *texture, 
        double x1, double y1, double x2, double y2)
{
    TexturePart *part = new TexturePart(texture, x1, y1, x2, y2);
    partsLoaded.push_back(part);
    return part;
}

TexturePart* TextureManager::getTexturePart(const std::string &fileName, 
        Texture *texture, double x1, double y1, double x2, double y2)
{
    std::string partName = getPartName(fileName, x1, y1, x2, y2);
    TexturesParts::iterator i = partsByName.find(partName);
    if (i != partsByName.end())
        return (*i).second;
    else {
        TexturePart *part = new TexturePart(texture, x1, y1, x2, y2);
        partsLoaded.push_back(part);
        partsByName[partName] = part;
        return part;
    }
}

TexturePart* TextureManager::load(const unsigned char *buffer, int length, 
        double x, double y, double width, double height)
{
    Texture *tex = loadImage(buffer, length);
    if (! tex)
        return NULL;
    else {
        double x1, y1, x2, y2;
        getPartCoords(tex, x, y, width, height, x1, y1, x2, y2);
        return getTexturePart(tex, x1, y1, x2, y2);
    }
}

TexturePart* TextureManager::load(const unsigned char *buffer, int length, 
        double width, double height)
{
    Texture *tex = loadImage(buffer, length);
    if (! tex)
        return NULL;
    else {
        double x1, y1, x2, y2;
        getPartCoords(tex, width, height, x1, y1, x2, y2);
        return getTexturePart(tex, x1, y1, x2, y2);
    }
}

TexturePart* TextureManager::load(const unsigned char *buffer, int length)
{
    Texture *tex = loadImage(buffer, length);
    if (! tex)
        return NULL;
    else {
        double x1, y1, x2, y2;
        getPartCoords(tex, x1, y1, x2, y2);
        return getTexturePart(tex, x1, y1, x2, y2);
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


TexturePart* TextureManager::addForeignTexture(int texId)
{
    Texture *tex = new Texture(texId, this);
    TexturePart *part = new TexturePart(tex, 0, 0, 1, 1);
    partsLoaded.push_back(part);
    loaded.push_back(tex);
    return part;
}


void TextureManager::unload(TexturePart *texturePart)
{
    if (! texturePart)
        return;

    partsLoaded.remove(texturePart);
    for (TexturesParts::iterator i = partsByName.begin(); 
            i != partsByName.end(); i++)
    {
        if ((*i).second == texturePart) {
            partsByName.erase(i);
            break;
        }
    }

    Texture *texture = texturePart->getTexture();
    delete texturePart;

    if (texture) {
        for (PartsList::iterator i = partsLoaded.begin(); 
                i != partsLoaded.end(); i++) 
        {
            if ((*i)->getTexture() == texture)
                // texture still in use
                return;
        }

        loaded.remove(texture);
        
        for (TexturesMap::iterator i = cache.begin(); i != cache.end(); i++)
        {
            if ((*i).second == texture) {
                cache.erase(i);
                break;
            }
        }

        delete texture;
    }
}


void TextureManager::unloadAll()
{
    for (PartsList::iterator i = partsLoaded.begin(); 
            i != partsLoaded.end(); i++)
        delete (*i);
    partsLoaded.clear();

    for (TexturesList::iterator i = loaded.begin(); i != loaded.end(); i++)
        delete (*i);
    loaded.clear();
    cache.clear();
}

void TextureManager::setGraphicsCallbacks(struct SaslGraphicsCallbacks *callbacks)
{
    graphics = callbacks;
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


/// Lua wrapper for unloading textures
static int luaUnloadImage(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;
    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);

    TextureManager *textureManager = getAvionics(L)->getTextureManager();

    if (! textureManager)
        return 0;

    textureManager->unload(tex);

    return 0;
}


/// Lua wrapper for loading image from memory
static int luaLoadImageFromMemory(lua_State *L)
{
    TextureManager *textureManager = getAvionics(L)->getTextureManager();

    size_t size;
    const unsigned char *data = (const unsigned char *)lua_tolstring(L, 1, 
            &size);

    TexturePart *texture = NULL;

    if (lua_isnil(L, 2)) 
        texture = textureManager->load(data, size);
    else if (lua_isnil(L, 4))
        texture = textureManager->load(data, size, 
                lua_tonumber(L, 2), lua_tonumber(L, 3));
    else
        texture = textureManager->load(data, size, 
                lua_tonumber(L, 2), lua_tonumber(L, 3),
                lua_tonumber(L, 4), lua_tonumber(L, 5));
    
    if (texture)
        lua_pushlightuserdata(L, texture);
    else
        lua_pushnil(L);
    return 1;
}


static int luaRecreateImage(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    SaslGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    if (lua_isnumber(L, 1))
        graphics->recreate_texture(graphics, (int)lua_tonumber(L, 1), 
                (int)lua_tonumber(L, 2), (int)lua_tonumber(L, 3));
    else {
        if (lua_islightuserdata(L, 1)) {
            TexturePart *part = (TexturePart*)lua_touserdata(L, 1);
            if (! part) 
                return 0;
            int width = (int)lua_tonumber(L, 2);
            int height = (int)lua_tonumber(L, 3);
            graphics->recreate_texture(graphics, part->getTexture()->getId(), 
                    width, height);
            part->getTexture()->setSize(width, height);
        }
    }

    return 0;
}


static int luaFindImage(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    SaslGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    int texId = -1;
    if (2 == lua_gettop(L))
        texId = graphics->find_texture(graphics, (int)lua_tonumber(L, 1), 
                (int)lua_tonumber(L, 2), NULL, NULL, NULL, NULL);
    else {
        int r = (int)lua_tonumber(L, 3);
        int g = (int)lua_tonumber(L, 4);
        int b = (int)lua_tonumber(L, 5);
        int a = (int)lua_tonumber(L, 6);
        texId = graphics->find_texture(graphics, (int)lua_tonumber(L, 1), 
                (int)lua_tonumber(L, 2), &r, &g, &b, &a);
    }

    if (0 >= texId) {
        TextureManager *textureManager = getAvionics(L)->getTextureManager();
        lua_pushlightuserdata(L, textureManager->addForeignTexture(texId));
        return 1;
    } else
        return 0;
}


static int luaSetRenderTarget(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    SaslGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    int texId;
    if (lua_isnumber(L, 1))
        texId = (int)lua_tonumber(L, 1);
    else {
        if (! lua_islightuserdata(L, 1)) {
            lua_pushboolean(L, false);
            return 1;
        } else {
            TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
            if (! tex) {
                lua_pushboolean(L, false);
                return 1;
            }
            texId = tex->getTexture()->getId();
        }
    }

    lua_pushboolean(L, graphics->set_render_target(graphics, texId));
    return 1;
}

static int luaRestoreRenderTarget(lua_State *L)
{
    Avionics *avionics = getAvionics(L);
    assert(avionics);
    SaslGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->set_render_target(graphics, -1);

    return 0;
}



void xa::exportTextureToLua(Luna &lua)
{
    lua_State *L = lua.getLua();

    lua_register(L, "getGLTexture", luaLoadImage);
    lua_register(L, "getTextureSize", luaGetTextureSize);
    lua_register(L, "loadImageFromMemory", luaLoadImageFromMemory);
    lua_register(L, "unloadImage", luaUnloadImage);
    lua_register(L, "recreateImage", luaRecreateImage);
    lua_register(L, "findImage", luaFindImage);
    lua_register(L, "setRenderTarget", luaSetRenderTarget);
    lua_register(L, "restoreRenderTarget", luaRestoreRenderTarget);
}

