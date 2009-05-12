#include "graph.h"

#include <GL/gl.h>

#include "avionics.h"


using namespace xa;


static void setupMatrix(double x, double y, double width, double height,
        double originalWidth, double originalHeight)
{
    glTranslated(x, y, 0);
    glScaled(width / originalWidth, height / originalHeight, 0);
}

/// Lua wrapper for setupMatrix
static int luaSetupMatrix(lua_State *L)
{
    setupMatrix(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), 
            lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6));
    return 0;
}


static void saveContext()
{
    glPushMatrix();
}

/// Lua wrapper for saveContext
static int luaSaveContext(lua_State *L)
{
    saveContext();
    return 0;
}

static void restoreContext()
{
    glPopMatrix();
}

/// Lua wrapper for saveContext
static int luaRestoreContext(lua_State *L)
{
    restoreContext();
    return 0;
}


/// Draw white frame (for debugging)
static void drawFrame(double x, double y, double width, double height)
{
    glDisable(GL_TEXTURE_2D);

    glColor3f(1, 1, 1);
    glBegin(GL_LINE_LOOP);
      glVertex2d(x, y);
      glVertex2d(x + width, y);
      glVertex2d(x + width, y + height);
      glVertex2d(x, y + height);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
}

/// Lua wrapper for drawFrame
static int luaDrawFrame(lua_State *L)
{
    drawFrame(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), 
            lua_tonumber(L, 4));
    return 0;
}

/// Draw white frame (for debugging)
static void drawRectangle(double x, double y, double width, double height,
        double r, double g, double b, double a)
{
    glDisable(GL_TEXTURE_2D);

    glColor4d(r, g, b, a);
    glBegin(GL_QUADS);
       glVertex2d(x, y + height);
       glVertex2d(x + width, y + height);
       glVertex2d(x + width, y);
       glVertex2d(x, y);
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
}

/// Lua wrapper for drawFrame
static int luaDrawRectangle(lua_State *L)
{
    drawRectangle(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), 
            lua_tonumber(L, 4), lua_tonumber(L, 5), lua_tonumber(L, 6),
            lua_tonumber(L, 7), lua_tonumber(L, 8));
    return 0;
}


static void drawTexture(Avionics *avionics, TexturePart *tex,
        double x, double y, double width, double height,
        float r, float g, float b, float a)
{
    glEnable(GL_TEXTURE_2D);
 
    tex->getTexture()->bind();

    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
      glTexCoord2f(tex->getX1(), tex->getY1());  glVertex2d(x, y + height);
      glTexCoord2f(tex->getX2(), tex->getY1());  glVertex2d(x + width, y + height);
      glTexCoord2f(tex->getX2(), tex->getY2());  glVertex2d(x + width, y);
      glTexCoord2f(tex->getX1(), tex->getY2());  glVertex2d(x, y);
    glEnd();
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

    float r, g, b, a;
    getAvionics(L)->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 6, r, g, b, a);

    drawTexture(getAvionics(L), tex, lua_tonumber(L, 2), lua_tonumber(L, 3), 
            lua_tonumber(L, 4), lua_tonumber(L, 5), r, g, b, a);

    return 0;
}


/// draw texture with ability to set texture coords
static void drawTexturePart(Avionics *avionics, TexturePart *tex, 
        double x, double y, double width, double height, 
        double tx, double ty, double tw, double th,
        float r, float g, float b, float a)
{
    glEnable(GL_TEXTURE_2D);

    double pw = tex->getX2() - tex->getX1();
    double ph = tex->getY2() - tex->getY1();

    double tx1 = tex->getX1() + pw * tx;
    double ty1 = tex->getY1() + ph * ty;
    double tx2 = tx1 + pw * tw;
    double ty2 = ty1 + ph * th;

    tex->getTexture()->bind();

    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
      glTexCoord2f(tx1, ty1);  glVertex2d(x, y + height);
      glTexCoord2f(tx2, ty1);  glVertex2d(x + width, y + height);
      glTexCoord2f(tx2, ty2);  glVertex2d(x + width, y);
      glTexCoord2f(tx1, ty2);  glVertex2d(x, y);
    glEnd();
}


/// Lua wrapper for drawTexturePart
static int luaDrawTexturePart(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    
    float r, g, b, a;
    getAvionics(L)->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 10, r, g, b, a);

    drawTexturePart(getAvionics(L), tex, lua_tonumber(L, 2), lua_tonumber(L, 3), 
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
    glPushMatrix();
    double centerX = x + width / 2.0;
    double centerY = y + height / 2.0;
    glTranslated(centerX, centerY, 0);
    glRotatef(angle, 0, 0, -1.0f);
    glTranslated(-centerX, -centerY, 0);

    drawTexture(avionics, tex, x, y, width, height, r, g, b, a);

    glPopMatrix();
}


/// Lua wrapper for drawRotatedTexture
static int luaDrawRotatedTexture(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    
    float r, g, b, a;
    getAvionics(L)->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 7, r, g, b, a);

    drawRotatedTexture(getAvionics(L), tex, lua_tonumber(L, 2), 
            lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5), 
            lua_tonumber(L, 6), r, g, b, a);

    return 0;
}


static void drawRotatedTexturePart(Avionics *avionics, TexturePart *tex, 
        double angle, double x, double y, double width, double height,
        double tx, double ty, double tw, double th,
        float r, float g, float b, float a)
{
    glPushMatrix();
    double centerX = x + width / 2.0;
    double centerY = y + height / 2.0;
    glTranslated(centerX, centerY, 0);
    glRotatef(angle, 0, 0, -1.0f);
    glTranslated(-centerX, -centerY, 0);

    drawTexturePart(avionics, tex, x, y, width, height, tx, ty, tw, th,
            r, g, b, a);

    glPopMatrix();
}


/// Lua wrapper for drawRotatedTexturePart
static int luaDrawRotatedTexturePart(lua_State *L)
{
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    
    float r, g, b, a;
    getAvionics(L)->getBackgroundColor(r, g, b, a);
    rgbaFromLua(L, 11, r, g, b, a);

    drawRotatedTexturePart(getAvionics(L), tex, lua_tonumber(L, 2), 
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
}

