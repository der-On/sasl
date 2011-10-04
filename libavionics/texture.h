#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <map>
#include <string>
#include "luna.h"
#include "libavcallbacks.h"

namespace xa {


class TextureManager;


/// Simple wrapper for x-plane textures
class Texture
{
    friend class TextureManager;

    private:
        /// OpenGL texture ID
        int id;

        /// Width of texture
        int width;

        /// Height of texture
        int height;
 
        /// Reference to texture manager
        TextureManager *manager;

    private:
        /// Create texture object
        Texture(int id, int width, int height, TextureManager *manager);

    public:
        /// Destroy texture
        ~Texture();

    public:
        /// returns texture ID.
        int getId() const { return id; }

        /// Returns width of texture
        int getWidth() const { return width; }

        /// Returns height of texture
        int getHeight() const { return height; }
};


/// Texture and texture coords
class TexturePart
{
    private:
        /// Texture data
        Texture *texture;

        /// Texture coords
        double x1, y1, x2, y2;

    public:
        /// Create texture and texture coords
        TexturePart(Texture *tex, double x1, double y1, 
                double x2, double y2);
        
    public:
        /// Returns X1 texture coord
        double getX1() const { return x1; }
        
        /// Returns Y1 texture coord
        double getY1() const { return y1; }
        
        /// Returns X2 texture coord
        double getX2() const { return x2; }
        
        /// Returns Y2 texture coord
        double getY2() const { return y2; }

        /// Returns mapped texture
        Texture* getTexture() { return texture; }
};


/// Textures manager
class TextureManager
{
    private:
        /// Textures mapped by file name
        typedef std::map<std::string, Texture*> TexturesMap;

        /// Textures cache
        TexturesMap cache;
        
        /// Textures parts mapped by part name
        typedef std::map<std::string, TexturePart*> TexturesParts;

        /// Textures parts cache
        TexturesParts parts;

        /// Graphics funtions
        SaslGraphicsCallbacks *graphics;
        
    public:
        /// Create texture manager
        TextureManager();

        /// Destroy texture manager and all cached textures
        ~TextureManager();

    public:
        /// Load entire texture
        TexturePart* load(const std::string &path);
        
        /// Load center part of texture
        TexturePart* load(const std::string &path, double width, double height);
        
        /// Load part of texture
        TexturePart* load(const std::string &path, double x, double y,
                double width, double height);

        /// Unload all textures
        void unloadAll();

        /// set graphics callbacks
        void setGraphicsCallbacks(struct SaslGraphicsCallbacks *graphics);

        /// Returns graphics API
        SaslGraphicsCallbacks* getGraphics() { return graphics; }

    private:
        /// Load image from file or return cached image if already loaded.
        Texture* loadImage(const std::string &fileName);

        /// Returns texture coords which covers entire image
        void getPartCoords(Texture *texture, double &x1, double &y1,
                double &x2, double &y2);
        
        /// Returns texture coords which covers rectangle in image center
        void getPartCoords(Texture *texture, double width, double height,
                double &x1, double &y1, double &x2, double &y2);
        
        /// Returns texture coords which covers rectangle in image
        void getPartCoords(Texture *texture, 
                double x, double y, double width, double height,
                double &x1, double &y1, double &x2, double &y2);

        /// Returns name of texture part
        std::string getPartName(const std::string& fileName, 
                double x1, double y1, double x2, double y2);

        /// Returns texture and texture coords
        TexturePart* getTexturePart(const std::string &fileName, 
            Texture *texture, double x1, double y1, double x2, double y2);
};


/// Register functions in Lua
void exportTextureToLua(Luna &lua);

};

#endif

