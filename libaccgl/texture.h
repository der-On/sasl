#ifndef __TEXTURE_H__
#define __TEXTURE_H__


/// Texture with ID and size
class Texture
{
    protected:
        /// OpenGL texture ID
        int id;

        /// Texture width in pixels
        int width;
        
        /// Texture height in pixels
        int height;

    public:
        /// Create subtexture with specified ID and size
        SubTexture(int id, int width, int height): id(id), width(width),
                height(height) { };

        virtual ~SubTexture() { };

    public:
        /// Return OpenGL texture ID
        int getId() const { return id; };
        
        /// Returns width of texture in pixels
        int getWidth() const { return width; };
        
        /// Returns height of texture in pixels
        int getHeight() const { return height; };
};


#endif

