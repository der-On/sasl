#ifndef __TEX_COMBINER_H__
#define __TEX_COMBINER_H__


#include "supertexture.h"


/// Combines small textures int bigger
class TexCombiner
{
    private:
        /// list of available super textures
        std::vector<SuperTexture> superTextures;

        /// list of textures not combined yet
        std::vector<std::pair<int, Texture> > uncombined;

        /// combined textures mapped by its id
        std::map<int, SubTexture*> combined;

    public:
        /// Create new texture combiner instance
        TexCombiner();

    public:
        /// request texture loading into super texture
        /// returns internal texture ID.  to get real texture ID
        /// combine texture and use mapTexture function
        int loadTexture(const char *name, int *width, int *height);

        /// combine textures
        void combine();

        /// Map single texture to super texture
        /// Returns super texture ID
        int mapTexture(int oldTexId, float ox1, float oy1, float ox2, float oy2,
            float &dx1, float &dy1, float &dx2, float &dy2);
};


#endif

