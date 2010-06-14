#include "graph.h"

#include <math.h>
#include <assert.h>

#include "avionics.h"
#include "font.h"

using namespace xa;


static void setupMatrix(Avionics *avionics, double x, double y, 
        double width, double height,
        double originalWidth, double originalHeight)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->translate_transform(graphics, x, y);
    graphics->scale_transform(graphics, 
            width / originalWidth, height / originalHeight);
}

/// Lua wrapper for setupMatrix
static int luaSetupMatrix(lua_State *L)
{
    setupMatrix(getAvionics(L), lua_tonumber(L, 1), lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6));
    return 0;
}


static void saveContext(Avionics *avionics)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);
    graphics->push_transform(graphics);
}

/// Lua wrapper for saveContext
static int luaSaveContext(lua_State *L)
{
    saveContext(getAvionics(L));
    return 0;
}

static void restoreContext(Avionics *avionics)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);
    graphics->pop_transform(graphics);
}

/// Lua wrapper for saveContext
static int luaRestoreContext(lua_State *L)
{
    restoreContext(getAvionics(L));
    return 0;
}


/// Draw white frame (for debugging)
static void drawFrame(Avionics *avionics, double x, double y, 
        double width, double height)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->draw_line(graphics, x, y, x + width, y, 1, 1, 1, 1);
    graphics->draw_line(graphics, x + width, y, x + width, y + height, 1, 1, 1, 1);
    graphics->draw_line(graphics, x + width, y + height, x, y + height, 1, 1, 1, 1);
    graphics->draw_line(graphics, x, y + height, x, y, 1, 1, 1, 1);
}

/// Lua wrapper for drawFrame
static int luaDrawFrame(lua_State *L)
{
    drawFrame(getAvionics(L), lua_tonumber(L, 1), lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4));
    return 0;
}

/// Draw non-filled rectangle
static void drawRectangle(Avionics *avionics, double x, double y, 
        double width, double height,
        double r, double g, double b, double a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->draw_triangle(graphics, 
            x, y + height, r, g, b, a,
            x + width, y + height, r, g, b, a,
            x + width, y, r, g, b, a);
    graphics->draw_triangle(graphics, 
            x, y + height, r, g, b, a,
            x + width, y, r, g, b, a,
            x, y, r, g, b, a);
}

/// Lua wrapper for drawRectangle
static int luaDrawRectangle(lua_State *L)
{
    drawRectangle(getAvionics(L), lua_tonumber(L, 1), lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
    return 0;
}

/// Draw triangle of specified color
static void drawTriangle(Avionics *avionics, double x1, double y1, double x2, double y2,
        double x3, double y3,
        double r, double g, double b, double a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->draw_triangle(graphics, 
            x1, y1, r, g, b, a,
            x2, y2, r, g, b, a,
            x3, y3, r, g, b, a);
}

/// Lua wrapper for drawTriangle
static int luaDrawTriangle(lua_State *L)
{
    drawTriangle(getAvionics(L), lua_tonumber(L, 1), lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8), 
            lua_tonumber(L, 9), lua_tonumber(L, 10));
    return 0;
}


/// Draw line
static void drawLine(Avionics *avionics, double x1, double y1, 
        double x2, double y2, double r, double g, double b, double a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->draw_line(graphics, x1, y1, x2, y2, r, g, b, a);
}

/// Lua wrapper for drawLine
static int luaDrawLine(lua_State *L)
{
    drawLine(getAvionics(L), lua_tonumber(L, 1), lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8));
    return 0;
}


static void drawTexture(Avionics *avionics, TexturePart *tex,
        double x, double y, double width, double height,
        float r, float g, float b, float a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->draw_textured_triangle(graphics, tex->getTexture()->getId(),
            x, y + height, tex->getX1(), tex->getY1(), r, g, b, a,
            x + width, y + height, tex->getX2(), tex->getY1(), r, g, b, a,
            x + width, y, tex->getX2(), tex->getY2(), r, g, b, a);
    graphics->draw_textured_triangle(graphics, tex->getTexture()->getId(),
            x, y + height, tex->getX1(), tex->getY1(), r, g, b, a,
            x + width, y, tex->getX2(), tex->getY2(), r, g, b, a,
            x, y, tex->getX1(), tex->getY2(), r, g, b, a);
}


/// Read rgba from Lua call arguments
/// If rgb is not specified does nothinf
/// if alpha component is not specified it keep default
static void rgbaFromLua(lua_State *L, int base, float &r, float &g,
        float &b, float &a)
{
    if (base + 2 <= lua_gettop(L)) {
        r = lua_tonumber(L, base);
        g = lua_tonumber(L, base + 1);
        b = lua_tonumber(L, base + 2);
        if (base + 3 <= lua_gettop(L))
            a = lua_tonumber(L, base + 3);
    }
}


/// Lua wrapper for drawTexture
static int luaDrawTexture(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    Avionics *avionics = getAvionics(L);

    float r, g, b, a;
    avionics->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 6, r, g, b, a);

    drawTexture(avionics, tex, lua_tonumber(L, 2), lua_tonumber(L, 3), 
            lua_tonumber(L, 4), lua_tonumber(L, 5), r, g, b, a);

    return 0;
}


/// draw texture with ability to set texture coords
static void drawTexturePart(Avionics *avionics, TexturePart *tex, 
        double x, double y, double width, double height, 
        double tx, double ty, double tw, double th,
        float r, float g, float b, float a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    double pw = tex->getX2() - tex->getX1();
    double ph = tex->getY2() - tex->getY1();

    double tx1 = tex->getX1() + pw * tx;
    double ty1 = tex->getY1() + ph * ty;
    double tx2 = tx1 + pw * tw;
    double ty2 = ty1 + ph * th;

    graphics->draw_textured_triangle(graphics, tex->getTexture()->getId(),
            x, y + height, tx1, ty1, r, g, b, a,
            x + width, y + height, tx2, ty1, r, g, b, a,
            x + width, y, tx2, ty2, r, g, b, a);
    graphics->draw_textured_triangle(graphics, tex->getTexture()->getId(),
            x, y + height, tx1, ty1, r, g, b, a,
            x + width, y, tx2, ty2, r, g, b, a,
            x, y, tx1, ty2, r, g, b, a);
}


/// Lua wrapper for drawTexturePart
static int luaDrawTexturePart(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    Avionics *avionics = getAvionics(L);
    
    float r, g, b, a;
    avionics->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 10, r, g, b, a);

    drawTexturePart(avionics, tex, lua_tonumber(L, 2), lua_tonumber(L, 3), 
            lua_tonumber(L, 4), lua_tonumber(L, 5),
            lua_tonumber(L, 6), lua_tonumber(L, 7), 
            lua_tonumber(L, 8), lua_tonumber(L, 9),
            r, g, b, a);

    return 0;
}


static void drawRotatedTexture(Avionics *avionics, TexturePart *tex, 
        double angle, double x, double y, double width, double height,
        float r, float g, float b, float a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->push_transform(graphics);

    double centerX = x + width / 2.0;
    double centerY = y + height / 2.0;
    graphics->translate_transform(graphics, centerX, centerY);
    graphics->rotate_transform(graphics, angle);
    graphics->translate_transform(graphics, -centerX, -centerY);

    drawTexture(avionics, tex, x, y, width, height, r, g, b, a);

    graphics->pop_transform(graphics);
}


/// Lua wrapper for drawRotatedTexture
static int luaDrawRotatedTexture(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    Avionics *avionics = getAvionics(L);
    
    float r, g, b, a;
    avionics->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 7, r, g, b, a);

    drawRotatedTexture(avionics, tex, lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6), r, g, b, a);

    return 0;
}


static void drawRotatedTexturePart(Avionics *avionics, TexturePart *tex, 
        double angle, double x, double y, double width, double height,
        double tx, double ty, double tw, double th,
        float r, float g, float b, float a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    graphics->push_transform(graphics);
    double centerX = x + width / 2.0;
    double centerY = y + height / 2.0;
    graphics->translate_transform(graphics, centerX, centerY);
    graphics->rotate_transform(graphics, angle);
    graphics->translate_transform(graphics, -centerX, -centerY);

    drawTexturePart(avionics, tex, x, y, width, height, tx, ty, tw, th,
            r, g, b, a);

    graphics->pop_transform(graphics);
}


/// Lua wrapper for drawRotatedTexturePart
static int luaDrawRotatedTexturePart(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    Avionics *avionics = getAvionics(L);
    
    float r, g, b, a;
    avionics->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 11, r, g, b, a);

    drawRotatedTexturePart(avionics, tex, lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8), 
            lua_tonumber(L, 9), lua_tonumber(L, 10),
            r, g, b, a);

    return 0;
}


/// Lua wrapper for drawFont
static int luaDrawFont(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    Font *font = (Font*)lua_touserdata(L, 1);
    if (! font)
        return 0;
    Avionics *avionics = getAvionics(L);

    float r, g, b, a;
    avionics->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 5, r, g, b, a);
    
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    drawFont(font, graphics, lua_tonumber(L, 2), lua_tonumber(L, 3), 
            lua_tostring(L, 4), r, g, b, a);

    return 0;
}


static void rotatePoint(double &x, double &y, double ox, double oy, 
        double centerX, double centerY, double angle, TexturePart *tex)
{
    double pw = tex->getX2() - tex->getX1();
    double ph = tex->getY2() - tex->getY1();

    double tx = ox - centerX;
    double ty = oy - centerY;
    x = tx * cos(angle) - ty * sin(angle) + centerX;
    y = ty * cos(angle) + tx * sin(angle) + centerY;

    x = tex->getX1() + x * pw;
    y = tex->getY1() + y * ph;
}


/// can't find good name for this
/// draw rectangle textured in strange way: texture coordinates are rotated
/// by specified angle
static void drawIntricatelyTexturedRectangle(Avionics *avionics, 
        TexturePart *tex, double angle, double x, double y, double width, 
        double height, double tx, double ty, double tw, double th,
        float r, float g, float b, float a)
{
    XaGraphicsCallbacks *graphics = avionics->getGraphics();
    assert(graphics);

    double tx1 = tx;
    double ty1 = ty;
    double tx2 = tx1 + tw;
    double ty2 = ty1 + th;

    double tcx = (tx2 + tx1) / 2;
    double tcy = (ty2 + ty1) / 2;

    double c1x, c1y;
    rotatePoint(c1x, c1y, tx1, ty1, tcx, tcy, angle, tex);
    double c2x, c2y;
    rotatePoint(c2x, c2y, tx2, ty1, tcx, tcy, angle, tex);
    double c3x, c3y;
    rotatePoint(c3x, c3y, tx2, ty2, tcx, tcy, angle, tex);
    double c4x, c4y;
    rotatePoint(c4x, c4y, tx1, ty2, tcx, tcy, angle, tex);
    
    graphics->draw_textured_triangle(graphics, tex->getTexture()->getId(),
            x, y + height, c1x, c1y, r, g, b, a,
            x + width, y + height, c2x, c2y, r, g, b, a,
            x + width, y, c3x, c3y, r, g, b, a);
    graphics->draw_textured_triangle(graphics, tex->getTexture()->getId(),
            x, y + height, c1x, c1y, r, g, b, a,
            x + width, y, c3x, c3y, r, g, b, a,
            x, y, c4x, c4y, r, g, b, a);
}


/// Lua wrapper for drawIntricatelyTexturedRectangle
static int luaDrawIntricatelyTexturedRectangle(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    Avionics *avionics = getAvionics(L);
    
    float r, g, b, a;
    avionics->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 11, r, g, b, a);

    drawIntricatelyTexturedRectangle(avionics, tex, lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8), 
            lua_tonumber(L, 9), lua_tonumber(L, 10),
            r, g, b, a);

    return 0;
}

void xa::exportGraphToLua(Luna &lua)
{
    lua_State *L = lua.getLua();

    lua_register(L, "setTranslation", luaSetupMatrix);
    lua_register(L, "saveGraphicsContext", luaSaveContext);
    lua_register(L, "restoreGraphicsContext", luaRestoreContext);
    lua_register(L, "drawFrame", luaDrawFrame);
    lua_register(L, "drawTexture", luaDrawTexture);
    lua_register(L, "drawRotatedTexture", luaDrawRotatedTexture);
    lua_register(L, "drawTexturePart", luaDrawTexturePart);
    lua_register(L, "drawRotatedTexturePart", luaDrawRotatedTexturePart);
    lua_register(L, "drawRectangle", luaDrawRectangle);
    lua_register(L, "drawTriangle", luaDrawTriangle);
    lua_register(L, "drawLine", luaDrawLine);
    lua_register(L, "drawText", luaDrawFont);
    lua_register(L, "drawTexturedRect", luaDrawIntricatelyTexturedRectangle);
}

