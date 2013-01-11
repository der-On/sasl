#include "xpdraw3d.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <SOIL.h>
#include "glheaders.h"

#include <vector>
#include <string.h>

#include "math3d.h"
#include "xpsdk.h"

#include "avionics.h"

using namespace xa;
using namespace xap;
using namespace xap3d;

static XPLMDataRef viewX;
static XPLMDataRef viewY;
static XPLMDataRef viewZ;
static XPLMDataRef viewPitch;
static XPLMDataRef viewRoll;
static XPLMDataRef viewHeading;

/*
static XPLMDataRef pilotX;
static XPLMDataRef pilotY;
static XPLMDataRef pilotZ;
static XPLMDataRef pilotPitch;
static XPLMDataRef pilotRoll;
static XPLMDataRef pilotHeading;
*/
//static XPLMDataRef viewType;

double lastViewX;
double lastViewY;
double lastViewZ;
double lastViewPitch;
double lastViewRoll;
double lastViewHeading;
//int lastViewType;

//static int viewType3dCockpit = 1026;

void xap3d::initDraw3d()
{
    viewX = XPLMFindDataRef("sim/graphics/view/view_x");
    viewY = XPLMFindDataRef("sim/graphics/view/view_y");
    viewZ = XPLMFindDataRef("sim/graphics/view/view_z");
    viewPitch = XPLMFindDataRef("sim/graphics/view/view_pitch");
    viewRoll = XPLMFindDataRef("sim/graphics/view/view_roll");
    viewHeading = XPLMFindDataRef("sim/graphics/view/view_heading");

    /*
    pilotX = XPLMFindDataRef("sim/graphics/view/pilots_head_x");
    pilotY = XPLMFindDataRef("sim/graphics/view/pilots_head_y");
    pilotZ = XPLMFindDataRef("sim/graphics/view/pilots_head_z");
    pilotPitch = XPLMFindDataRef("sim/graphics/view/pilots_head_the");
    pilotRoll = XPLMFindDataRef("sim/graphics/view/pilots_head_psi");
    pilotHeading = XPLMFindDataRef("sim/graphics/view/cockpit_heading");

    viewType = XPLMFindDataRef("sim/graphics/view/view_type");
    */
}

double radians(double degrees)
{
    double radians = 0;
    radians = degrees * (M_PI/180);
    return radians;
}

Vector crossProduct(Vector v1, Vector v2)
{
    Vector vec;
    vec.x = v1.y * v2.z - v2.y * v1.z;
    vec.y = v2.x * v1.z - v1.x * v2.z;
    vec.z = v1.x * v2.y - v1.y * v2.x;

    return vec;
}

double dotProduct(Vector v1, Vector v2) {
	double dot;
    dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    return dot;
 }

// start texturing
void setTexture(int texId)
{
    GLint currentTexture[1];
    glGetIntegerv(GL_TEXTURE_BINDING_2D,currentTexture);

    if (currentTexture[0] != texId) {
        if (! currentTexture[0])
            glEnable(GL_TEXTURE_2D);

        XPLMBindTexture2d(texId, 0);
    }
}

void drawTexturedQuad(int texId, float size,
                            double x, double y, double z,
                            float angleX, float angleY, float angleZ,
                            float u1, float v1, float u2, float v2)
{
    float size_half = size / 2;

    setTexture(texId);

    glPushMatrix();
    glTranslatef(x,y,z);
    glRotatef((GLfloat)angleX,(GLfloat)1.0f,(GLfloat)0.0f,(GLfloat)0.0f);
    glRotatef((GLfloat)angleY,(GLfloat)0.0f,(GLfloat)1.0f,(GLfloat)0.0f);
    glRotatef((GLfloat)angleZ,(GLfloat)0.0f,(GLfloat)0.0f,(GLfloat)1.0f);

    glBegin(GL_QUADS);
        glColor4f(1.0f,1.0f,1.0f,0.5f);
        glTexCoord2f(u1, v1);
        glVertex3f( -size_half, size_half, 0.0f);
        glTexCoord2f(u2, v1);
        glVertex3f( size_half, size_half, 0.0f);
        glTexCoord2f(u2, v2);
        glVertex3f( size_half, -size_half, 0.0f);
        glTexCoord2f(u1, v2);
        glVertex3f( -size_half, -size_half, 0.0f);
    glEnd();

    glPopMatrix();
}

// billboarding code taken from http://www.lighthouse3d.com/opengl/billboarding/

void drawTexturedBillboard(int texId, float width, float height,
                           double x, double y, double z,
                           float r, float g, float b, float alpha,
                           float u1, float v1, float u2, float v2)
{
    float width_half = width / 2;
    float height_half = height / 2;
    Vector objToCamProj,lookAt,upAux,objToCam;
    double angleCosine;

    setTexture(texId);

    glPushMatrix();
    glTranslatef(x,y,z);

    // objToCamProj is the vector in world coordinates from the
    // local origin to the camera projected in the XZ plane
    objToCamProj.x = lastViewX - x;
    objToCamProj.y = 0.0f;
    objToCamProj.z = lastViewZ - z;

    // This is the original lookAt vector for the object
    // in world coordinates
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

    // normalize both vectors to get the cosine directly afterwards
    objToCamProj.normalize();

    // easy fix to determine wether the angle is negative or positive
    // for positive angles upAux will be a vector pointing in the
    // positive y direction, otherwise upAux will point downwards
    // effectively reversing the rotation.
    upAux = crossProduct(lookAt,objToCamProj);

    // compute the angle
    angleCosine = dotProduct(lookAt,objToCamProj);

    // perform the rotation. The if statement is used for stability reasons
    // if the lookAt and objToCamProj vectors are too close together then
    // |angleCosine| could be bigger than 1 due to lack of precision
    if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
        glRotatef(acos(angleCosine)*180/M_PI,upAux.x, upAux.y, upAux.z);

    // so far it is just like the cylindrical billboard. The code for the
    // second rotation comes now
    // The second part tilts the object so that it faces the camera

    // objToCam is the vector in world coordinates from
    // the local origin to the camera
    objToCam.x = lastViewX - x;
    objToCam.y = lastViewY - y;
    objToCam.z = lastViewZ - z;

    // Normalize to get the cosine afterwards
    objToCam.normalize();

    // Compute the angle between objToCamProj and objToCam,
    //i.e. compute the required angle for the lookup vector

    angleCosine = dotProduct(objToCamProj,objToCam);


    // Tilt the object. The test is done to prevent instability
    // when objToCam and objToCamProj have a very small
    // angle between them

    if ((angleCosine < 0.99990) && (angleCosine > -0.9999)) {
        if (objToCam[1] < 0)
            glRotatef(acos(angleCosine)*180/M_PI,1,0,0);
        else
            glRotatef(acos(angleCosine)*180/M_PI,-1,0,0);
     }

    glBegin(GL_QUADS);
        glColor4f(r,g,b,alpha);
        glTexCoord2f(u1, v1);
        glVertex3f( -width_half, height_half, 0.0f);
        glTexCoord2f(u2, v1);
        glVertex3f( width_half, height_half, 0.0f);
        glTexCoord2f(u2, v2);
        glVertex3f( width_half, -height_half, 0.0f);
        glTexCoord2f(u1, v2);
        glVertex3f( -width_half, -height_half, 0.0f);
    glEnd();

    glPopMatrix();
}

// delayed draw call
struct BillboardCommand
{
    // texture
    int texId;

    // size
    float width;
    float height;

    // position of object
    double x;
    double y;
    double z;

    // alpha and color
    float r;
    float g;
    float b;
    float alpha;

    // uv coordinates
    float u1;
    float v1;
    float u2;
    float v2;
};

// list of billboards to draw
static std::vector<BillboardCommand> billboardsToDraw;

// draw billboard
static int luaDrawBillboard(lua_State *L)
{
    double tx,ty,tw,th;

    if (6 != lua_gettop(L) && 10 != lua_gettop(L) && 14 != lua_gettop(L))
        return 0;

    BillboardCommand c;
    if ((! lua_islightuserdata(L, 1) || lua_isnil(L, 1)))
        return 0;

    TexturePart *tex = (TexturePart*)lua_touserdata(L, 1);
    c.texId = tex->getTexture()->getId();

    if (! c.texId)
        return 0;

    int texWidth = tex->getTexture()->getWidth();
    int texHeight = tex->getTexture()->getHeight();
    double fw = 1/(float)texWidth;
    double fh = 1/(float)texHeight;

    c.width = lua_tonumber(L, 2);
    c.height = lua_tonumber(L, 3);
    c.x = lua_tonumber(L, 4);
    c.y = lua_tonumber(L, 5);
    c.z = lua_tonumber(L, 6);

    if (lua_gettop(L) >= 10)
    {
        c.r = lua_tonumber(L, 7);
        c.g = lua_tonumber(L, 8);
        c.b = lua_tonumber(L, 9);
        c.alpha = lua_tonumber(L, 10);
    } else {
        c.r = 1.0f;
        c.g = 1.0f;
        c.b = 1.0f;
        c.alpha = 1.0f;
    }

    if (14 == lua_gettop(L))
    {
        tx = lua_tonumber(L, 11);
        ty = lua_tonumber(L, 12);
        tw = lua_tonumber(L, 13);
        th = lua_tonumber(L, 14);
    } else {
        tx = 0.0;
        ty = 0.0;
        tw = texWidth;
        th = texHeight;
    }

    double tx1 = tex->getX1() + (tx * fw);
    double ty1 = tex->getY1() + (ty * fh);
    double tx2 = tx1 + (tw * fw);
    double ty2 = ty1 + (fh * th);

    c.u1 = tx1;
    c.v1 = ty1;
    c.u2 = tx2;
    c.v2 = ty2;

    billboardsToDraw.push_back(c);

    return 0;
}

void drawBillboards()
{
    if (billboardsToDraw.size()) {

        // set correct graphic states
        XPLMSetGraphicsState(1,1,1,0,1,1,0);
        glEnable(GL_TEXTURE_2D);
        glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (std::vector<BillboardCommand>::iterator i = billboardsToDraw.begin();
                i != billboardsToDraw.end(); i++)
        {
            BillboardCommand &c = *i;

            drawTexturedBillboard(c.texId, c.width, c.height,
                                  c.x, c.y, c.z,
                                  c.r, c.g, c.b, c.alpha,
                                  c.u1, c.v1, c.u2, c.v2);
        }

        glDepthMask( GL_TRUE );
        glPopAttrib();
    }

}


void xap3d::draw3d(XPLMDrawingPhase phase)
{
    if (phase == xplm_Phase_LastScene) {
        //lastViewType = XPLMGetDatai(viewType);

        lastViewPitch = XPLMGetDataf(viewPitch);
        lastViewRoll = XPLMGetDataf(viewRoll);
        lastViewHeading = XPLMGetDataf(viewHeading);

        lastViewX = XPLMGetDataf(viewX);
        lastViewY = XPLMGetDataf(viewY);
        lastViewZ = XPLMGetDataf(viewZ);

        drawBillboards();
    }
}

void xap3d::frameFinished()
{
    billboardsToDraw.clear();
}

void xap3d::exportDraw3dFunctions(lua_State *L)
{
    lua_register(L, "drawBillboard", luaDrawBillboard);
}
