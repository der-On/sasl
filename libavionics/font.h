#ifndef __FONT_H__
#define __FONT_H__


#include <string>
#include <map>
#include "luna.h"
#include "xcallbacks.h"


namespace xa {


/// font structure
struct Font;


class TextureManager;



/// Returns height of font
int getFontHeight(struct Font* font);

/// Returns width of font
int getFontWidth(struct Font* font, const char *str);

/// Draw text
void drawFont(struct Font* font, XaGraphicsCallbacks *graphics, 
        double x, double y, const char *str, 
        double red, double green, double blue, double alpha);



/// Fonts loader
class FontManager
{
    private:
        /// Fonts mapped by file name
        typedef std::map<std::string, Font*> FontsMap;

        /// Textures cache
        FontsMap cache;

        /// textures loader
        TextureManager &textureManager;

    public:
        /// Create new fonts manager
        FontManager(TextureManager &textureManager);

        /// Destroy fonts manager and all fonts
        ~FontManager();

    public:
        /// Load font or extract it from already loaded fonts.
        /// Do not delete loaded font manually
        Font* loadFont(const std::string &fontName);
};


/// Register functions in Lua
void exportFontToLua(Luna &lua);

};


#endif

