#include "font.h"

#include <fstream>
#include <string>
#include <ctype.h>
#include "texture.h"
#include "utils.h"
#include "unicode.h"
#include "avionics.h"

#include "glheaders.h"



using namespace xa;



/// character in font
struct Glyph
{
    /// X position of character in texture
    int x;
    
    /// Y position of character in texture
    int y;

    /// Character width
    int width;
    
    /// Character height
    int height;

    /// X offset from character start?
    int xOffset;
    
    /// Y offset from character start?
    int yOffset;

    /// How far move cursor for next character?
    int xAdvance;
};


/// font class
struct xa::Font
{
    /// font texture
    TexturePart *texture;

    /// Total height of font
    int lineHeight;

    /// Base line
    int base;

    /// Glyphs
    std::map<int, Glyph> glyphs;
    
    /// kerning pairs
    std::map<std::pair<int, int>, int> kerning;
};


/// read line from file.
/// returns empty line on errors
static std::string readLine(std::ifstream &f)
{
    std::string s;
    std::getline(f, s);
    if (! f.good())
        return "";
    else
        return s;
}


#ifdef _MSC_VER
static bool isblank(char ch)
{
    return (' ' == ch) || ('\t' == ch);
}
#endif

/// Returns word from line
static std::string getKeyword(const std::string &line, int &pos)
{
    int len = line.length();
    int first = pos;
    while ((pos < len) && (! isblank(line[pos]) && ('=' != line[pos])))
        pos++;
    return line.substr(first, pos - first);
}

/// Returns quoted text from line
static std::string getQuoted(const std::string &line, int &pos)
{
    int len = line.length();
    char quote = line[pos];
    pos++;
    int first = pos;
    while ((pos < len) && (quote != line[pos]))
        pos++;
    int end = pos;
    if ((pos < len) && (quote == line[pos]))
        pos++;
    return line.substr(first, end - first);
}

/// Returns attribute value.  Attribute value is either keyword or quoted string
static std::string getValue(const std::string &line, int &pos)
{
    if ('"' == line[pos])
        return getQuoted(line, pos);
    else
        return getKeyword(line, pos);
}


/// Skip all spaces
static void skipSpaces(const std::string &line, int &pos)
{
    int len = line.length();
    while ((pos < len) && isblank(line[pos]))
        pos++;
}


/// short alias for attrs map
typedef std::map<std::string, std::string> Attrs;


/// Split line to keyword (first word) and attributes
/// attributes is pairs name=value
static std::string splitLine(const std::string &line, Attrs &attrs)
{
    attrs.clear();

    int pos = 0;
    std::string keyword = getKeyword(line, pos);

    int len = line.length();
    while (pos < len) {
        skipSpaces(line, pos);
        if (pos >= len)
            break;
        std::string id = getKeyword(line, pos);
        if (pos >= len)
            break;
        if ('=' != line[pos]) {
            attrs[id] = "";
        } else {
            pos++;
            attrs[id] = getValue(line, pos);
        }
    }

    return keyword;
}


/// Load font.  Returns NULL on loading errors
static Font* loadFont(TextureManager &textureManager,
        const std::string &fileName)
{
    std::ifstream f(fileName.c_str(), std::ifstream::in);
    if (! f.good())
        return NULL;   

    std::string line = readLine(f);
    if (! line.length())
        return NULL;
    
    line = readLine(f);
    if (! line.length())
        return NULL;

    Font *font = new Font;

    Attrs attrs;
    std::string type = splitLine(line, attrs);
    if ("common" != type) {
        delete font;
        return NULL;
    }

    font->lineHeight = strToInt(attrs["lineHeight"]);
    font->base = strToInt(attrs["base"]);
    int pages = strToInt(attrs["pages"]);

    if (1 != pages) {
        delete font;
        return NULL;
    }
    
    line = readLine(f);
    type = splitLine(line, attrs);
    if ("page" != type) {
        delete font;
        return NULL;
    }
    std::string pageFile = attrs["file"];
    
    line = readLine(f);
    type = splitLine(line, attrs);
    if ("chars" != type) {
        delete font;
        return NULL;
    }

    line = readLine(f);
    while (0 < line.length()) {
        type = splitLine(line, attrs);
        if ("char" == type) {
            Glyph glyph;
            glyph.x = strToInt(attrs["x"]);
            glyph.y = strToInt(attrs["y"]);
            glyph.width = strToInt(attrs["width"]);
            glyph.height = strToInt(attrs["height"]);
            glyph.xOffset = strToInt(attrs["xoffset"]);
            glyph.yOffset = strToInt(attrs["yoffset"]);
            glyph.xAdvance = strToInt(attrs["xadvance"]);
            font->glyphs[strToInt(attrs["id"])] = glyph;
        } else if ("kerning" == type) {
            int first = strToInt(attrs["first"]);
            int second = strToInt(attrs["second"]);
            int amount = strToInt(attrs["amount"]);
            font->kerning[std::pair<int, int>(first, second)] = amount;
        }
        line = readLine(f);
    }

    std::string texturePath = getDirectory(fileName);
    font->texture = textureManager.load(texturePath + "/" + pageFile);

    if (! font->texture) {
        delete font;
        return NULL;
    }

    return font;
}


/// Free font structures
static void freeFont(Font* font)
{
    if (font)
        delete font;
}


int xa::getFontHeight(struct Font* font)
{
    if (! font)
        return 0;

    return font->lineHeight;
}


int xa::getFontWidth(struct Font* font, const char *str)
{
    if (! font)
        return 0;

    std::wstring ws = fromUtf8(str);
    int len = ws.length();
    int width = 0;
    for (int i = 0; i < len; i++) {
        int chr = ws[i];
        std::map<int, Glyph>::iterator g = font->glyphs.find(chr);
        if (g != font->glyphs.end())
            width += (*g).second.xAdvance;
    }
    return width;
}


void xa::drawFont(Font* font, int x, int y, const char *str,
        float r, float g, float b, float a)
{
    if (! font)
        return;
    
    std::wstring ws = fromUtf8(str);
    int len = ws.length();
    if (! len)
        return;

    GLfloat tW = font->texture->getTexture()->getWidth();
    GLfloat tH = font->texture->getTexture()->getHeight();

    glEnable(GL_TEXTURE_2D);
    font->texture->getTexture()->bind();
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);

    int posX = x;
    for (int i = 0; i < len; i++) {
        int chr = ws[i];
        std::map<int, Glyph>::iterator g = font->glyphs.find(chr);
        if (g != font->glyphs.end()) {
            Glyph &glyph = (*g).second;

            GLfloat gX = glyph.x;
            GLfloat gY = glyph.y;
            GLfloat gW = glyph.width;
            GLfloat gH = glyph.height;
            GLfloat gXO = glyph.xOffset;
            GLfloat gYO = font->base - (glyph.yOffset + gH);

            glTexCoord2f(gX / tW, gY / tH);
            glVertex2f(posX + gXO, y + gH + gYO);

            glTexCoord2f((gX + gW) / tW, gY / tH);
            glVertex2f(posX + gW + gXO, y + gH + gYO);

            glTexCoord2f((gX + gW) / tW, (gY + gH) / tH);
            glVertex2f(posX + gW + gXO, y + gYO);

            glTexCoord2f(gX / tW, (gY + gH) / tH);
            glVertex2f(posX + gXO, y + gYO);

            posX += glyph.xAdvance;
        }
    }

    glEnd();
}



xa::FontManager::FontManager(TextureManager &textureManager): 
    textureManager(textureManager)
{
}


xa::FontManager::~FontManager()
{
    for (FontsMap::iterator i = cache.begin(); i != cache.end(); i++)
        freeFont((*i).second);
    cache.clear();
}


Font* xa::FontManager::loadFont(const std::string &fileName)
{
    FontsMap::iterator i = cache.find(fileName);
    if (i != cache.end()) {
        return (*i).second;
    } else {
        Font* font = ::loadFont(textureManager, fileName);
        if (font)
            cache[fileName] = font;
        return font;
    }
}

/// Lua wrapper for fonts manager
static int luaLoadFont(lua_State *L)
{
    FontManager *fontManager = getAvionics(L)->getFontManager();

    std::string fileName = lua_tostring(L, 1);
    Font *font = NULL;

    font = fontManager->loadFont(fileName);
    
    if (font)
        lua_pushlightuserdata(L, font);
    else
        lua_pushnil(L);
    return 1;
}


void xa::exportFontToLua(Luna &lua)
{
    lua_State *L = lua.getLua();

    lua_register(L, "getGLFont", luaLoadFont);
}

