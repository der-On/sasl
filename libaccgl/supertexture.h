#ifndef __SUPER_TEXTURE_H__
#define __SUPER_TEXTURE_H__


#include "texture.h"

#include <vector>


class SuperTexture;


/// Part of super texture
class SubTexture: public Texture
{
    private:
        /// Parent super texture
        SuperTexture *parent;

        /// X-position inside of super texture
        int posX;

        /// Y-position inside of super texture
        int posY;

    public:
        /// Create subtexture
        SubTexture(int id, int width, int height, SuperTexture *parent): 
            Texture(id, width, height), parent(parent), posX(0), posY(0) 
                { };

    public:
        /// Returns parent super texture
        SuperTexture* getParent() const { return parent};

        /// Returns X-position inside of super texture
        int getX() const { return posX; };
        
        /// Returns Y-position inside of super texture
        int getY() const { return posY; };

        /// Set position inside of super texture
        void setPosition(int x, int y) { posX = x; posY = y; };

        /// Map texture coords from subtexture into super texture
        void mapTexCoords(float ox1, float oy1, float ox2, float oy2,
            float &dx1, float &dy1, float &dx2, float &dy2) const;
};


/// Texture consist of smaller subtextures combined to one supertexture
/// for performance
class SuperTexture: public Texture
{
    private:
        /// list of subtextures
        std::vector<SubTexture> subTextures;

    public:
        SuperTexture(int id, int width, int height): Texture(id, width, height)
                { };

    public:
        /// Add subtextures into texture
        /// Returns list of combined textures
        /// Removes combined textures from its argument
        std::vector<std::pair<int, SubTexture*> > combine(std::vector<std::pair<int, Texture> >& textures);
        
        /// Map single texture to super texture
        /// Returns super texture ID
        int mapTexture(int oldTexId, float ox1, float oy1, float ox2, float oy2,
            float &dx1, float &dy1, float &dx2, float &dy2);
};


#endif

