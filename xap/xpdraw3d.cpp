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

// current OpenGL matrixes
float projectionMatrix[16];
float modelMatrix[16];
float clipMatrix[16];
float frustum[6][4];

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


void extractMatrixes()
{
    float   t;

    /* Get the current PROJECTION matrix from OpenGL */
    glGetFloatv( GL_PROJECTION_MATRIX, projectionMatrix );

    /* Get the current MODELVIEW matrix from OpenGL */
    glGetFloatv( GL_MODELVIEW_MATRIX, modelMatrix );

    /* Combine the two matrices (multiply projection by modelview) */
    clipMatrix[ 0] = modelMatrix[ 0] * projectionMatrix[ 0] + modelMatrix[ 1] * projectionMatrix[ 4] + modelMatrix[ 2] * projectionMatrix[ 8] + modelMatrix[ 3] * projectionMatrix[12];
    clipMatrix[ 1] = modelMatrix[ 0] * projectionMatrix[ 1] + modelMatrix[ 1] * projectionMatrix[ 5] + modelMatrix[ 2] * projectionMatrix[ 9] + modelMatrix[ 3] * projectionMatrix[13];
    clipMatrix[ 2] = modelMatrix[ 0] * projectionMatrix[ 2] + modelMatrix[ 1] * projectionMatrix[ 6] + modelMatrix[ 2] * projectionMatrix[10] + modelMatrix[ 3] * projectionMatrix[14];
    clipMatrix[ 3] = modelMatrix[ 0] * projectionMatrix[ 3] + modelMatrix[ 1] * projectionMatrix[ 7] + modelMatrix[ 2] * projectionMatrix[11] + modelMatrix[ 3] * projectionMatrix[15];

    clipMatrix[ 4] = modelMatrix[ 4] * projectionMatrix[ 0] + modelMatrix[ 5] * projectionMatrix[ 4] + modelMatrix[ 6] * projectionMatrix[ 8] + modelMatrix[ 7] * projectionMatrix[12];
    clipMatrix[ 5] = modelMatrix[ 4] * projectionMatrix[ 1] + modelMatrix[ 5] * projectionMatrix[ 5] + modelMatrix[ 6] * projectionMatrix[ 9] + modelMatrix[ 7] * projectionMatrix[13];
    clipMatrix[ 6] = modelMatrix[ 4] * projectionMatrix[ 2] + modelMatrix[ 5] * projectionMatrix[ 6] + modelMatrix[ 6] * projectionMatrix[10] + modelMatrix[ 7] * projectionMatrix[14];
    clipMatrix[ 7] = modelMatrix[ 4] * projectionMatrix[ 3] + modelMatrix[ 5] * projectionMatrix[ 7] + modelMatrix[ 6] * projectionMatrix[11] + modelMatrix[ 7] * projectionMatrix[15];

    clipMatrix[ 8] = modelMatrix[ 8] * projectionMatrix[ 0] + modelMatrix[ 9] * projectionMatrix[ 4] + modelMatrix[10] * projectionMatrix[ 8] + modelMatrix[11] * projectionMatrix[12];
    clipMatrix[ 9] = modelMatrix[ 8] * projectionMatrix[ 1] + modelMatrix[ 9] * projectionMatrix[ 5] + modelMatrix[10] * projectionMatrix[ 9] + modelMatrix[11] * projectionMatrix[13];
    clipMatrix[10] = modelMatrix[ 8] * projectionMatrix[ 2] + modelMatrix[ 9] * projectionMatrix[ 6] + modelMatrix[10] * projectionMatrix[10] + modelMatrix[11] * projectionMatrix[14];
    clipMatrix[11] = modelMatrix[ 8] * projectionMatrix[ 3] + modelMatrix[ 9] * projectionMatrix[ 7] + modelMatrix[10] * projectionMatrix[11] + modelMatrix[11] * projectionMatrix[15];

    clipMatrix[12] = modelMatrix[12] * projectionMatrix[ 0] + modelMatrix[13] * projectionMatrix[ 4] + modelMatrix[14] * projectionMatrix[ 8] + modelMatrix[15] * projectionMatrix[12];
    clipMatrix[13] = modelMatrix[12] * projectionMatrix[ 1] + modelMatrix[13] * projectionMatrix[ 5] + modelMatrix[14] * projectionMatrix[ 9] + modelMatrix[15] * projectionMatrix[13];
    clipMatrix[14] = modelMatrix[12] * projectionMatrix[ 2] + modelMatrix[13] * projectionMatrix[ 6] + modelMatrix[14] * projectionMatrix[10] + modelMatrix[15] * projectionMatrix[14];
    clipMatrix[15] = modelMatrix[12] * projectionMatrix[ 3] + modelMatrix[13] * projectionMatrix[ 7] + modelMatrix[14] * projectionMatrix[11] + modelMatrix[15] * projectionMatrix[15];

    /* Extract the numbers for the RIGHT plane */
    frustum[0][0] = clipMatrix[ 3] - clipMatrix[ 0];
    frustum[0][1] = clipMatrix[ 7] - clipMatrix[ 4];
    frustum[0][2] = clipMatrix[11] - clipMatrix[ 8];
    frustum[0][3] = clipMatrix[15] - clipMatrix[12];

    /* Normalize the result */
    t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
    frustum[0][0] /= t;
    frustum[0][1] /= t;
    frustum[0][2] /= t;
    frustum[0][3] /= t;

    /* Extract the numbers for the LEFT plane */
    frustum[1][0] = clipMatrix[ 3] + clipMatrix[ 0];
    frustum[1][1] = clipMatrix[ 7] + clipMatrix[ 4];
    frustum[1][2] = clipMatrix[11] + clipMatrix[ 8];
    frustum[1][3] = clipMatrix[15] + clipMatrix[12];

    /* Normalize the result */
    t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
    frustum[1][0] /= t;
    frustum[1][1] /= t;
    frustum[1][2] /= t;
    frustum[1][3] /= t;

    /* Extract the BOTTOM plane */
    frustum[2][0] = clipMatrix[ 3] + clipMatrix[ 1];
    frustum[2][1] = clipMatrix[ 7] + clipMatrix[ 5];
    frustum[2][2] = clipMatrix[11] + clipMatrix[ 9];
    frustum[2][3] = clipMatrix[15] + clipMatrix[13];

    /* Normalize the result */
    t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
    frustum[2][0] /= t;
    frustum[2][1] /= t;
    frustum[2][2] /= t;
    frustum[2][3] /= t;

    /* Extract the TOP plane */
    frustum[3][0] = clipMatrix[ 3] - clipMatrix[ 1];
    frustum[3][1] = clipMatrix[ 7] - clipMatrix[ 5];
    frustum[3][2] = clipMatrix[11] - clipMatrix[ 9];
    frustum[3][3] = clipMatrix[15] - clipMatrix[13];

    /* Normalize the result */
    t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
    frustum[3][0] /= t;
    frustum[3][1] /= t;
    frustum[3][2] /= t;
    frustum[3][3] /= t;

    /* Extract the FAR plane */
    frustum[4][0] = clipMatrix[ 3] - clipMatrix[ 2];
    frustum[4][1] = clipMatrix[ 7] - clipMatrix[ 6];
    frustum[4][2] = clipMatrix[11] - clipMatrix[10];
    frustum[4][3] = clipMatrix[15] - clipMatrix[14];

    /* Normalize the result */
    t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
    frustum[4][0] /= t;
    frustum[4][1] /= t;
    frustum[4][2] /= t;
    frustum[4][3] /= t;

    /* Extract the NEAR plane */
    frustum[5][0] = clipMatrix[ 3] + clipMatrix[ 2];
    frustum[5][1] = clipMatrix[ 7] + clipMatrix[ 6];
    frustum[5][2] = clipMatrix[11] + clipMatrix[10];
    frustum[5][3] = clipMatrix[15] + clipMatrix[14];

    /* Normalize the result */
    t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
    frustum[5][0] /= t;
    frustum[5][1] /= t;
    frustum[5][2] /= t;
    frustum[5][3] /= t;
}

bool isSphereInFrustum(double x, double y, double z, double r)
{
    int p;

    for( p = 0; p < 6; p++) {
        if ( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= -r) {
            return false;
        }
    }
    return true;
}

void drawTexturedQuad(int texId, float size,
                        double x, double y, double z,
                        float angleX, float angleY, float angleZ,
                        float u1, float v1, float u2, float v2)
{
    float size_half = size / 2;

    // exit if this quad is not in viewing frustum
    if (!isSphereInFrustum(x,y,z, size_half)) {
        return;
    }

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

    // exit if this billboard is not in viewing frustum
    if (!isSphereInFrustum(x,y,z, std::max(width_half,height_half))) {
        return;
    }

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
    //if ((angleCosine < 0.99990) && (angleCosine > -0.9999)) {
        glRotatef(acos(angleCosine)*180/M_PI,upAux.x, upAux.y, upAux.z);
    //}

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

    //if ((angleCosine < 0.99990) && (angleCosine > -0.9999)) {
        if (objToCam[1] < 0)
            glRotatef(acos(angleCosine)*180/M_PI,1,0,0);
        else
            glRotatef(acos(angleCosine)*180/M_PI,-1,0,0);
    //}

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
static std::map<int, std::vector<BillboardCommand> > billboardsToDraw;

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

    if (billboardsToDraw.count(c.texId) == 0) {
        std::vector<BillboardCommand> vector;
        billboardsToDraw[c.texId] = vector;
    }

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

    billboardsToDraw[c.texId].push_back(c);

    return 0;
}

// TODO: store billboards in a vertex buffers, sorted by texture before rendering
void drawBillboards()
{
    if (billboardsToDraw.size()) {
        // set correct graphic states
        XPLMSetGraphicsState(1,1,1,0,1,1,0);
        glEnable(GL_TEXTURE_2D);
        glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        /*glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );*/

        for (std::map<int, std::vector<BillboardCommand> >::iterator iter = billboardsToDraw.begin(); iter != billboardsToDraw.end(); ++iter) {
            std::vector <BillboardCommand> vector = (*iter).second;

            for (std::vector<BillboardCommand>::iterator i = vector.begin();
                    i != vector.end(); i++)
            {
                BillboardCommand &c = *i;

                drawTexturedBillboard(c.texId, c.width, c.height,
                                      c.x, c.y, c.z,
                                      c.r, c.g, c.b, c.alpha,
                                      c.u1, c.v1, c.u2, c.v2);
            }

            /*glVertexPointer( 3, GL_FLOAT, sizeof(Vertex), &(m_VertexBuffer[0].m_Pos) );
            glTexCoordPointer( 2, GL_FLOAT, sizeof(Vertex), &(m_VertexBuffer[0].m_Tex0) );
            glColorPointer( 4, GL_FLOAT, sizeof(Vertex), &(m_VertexBuffer[0].m_Diffuse) );*/

            //glDrawArrays( GL_QUADS, 0, m_VertexBuffer.size() );
        }

        /*glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        glDisableClientState( GL_COLOR_ARRAY );*/

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

        // extract current OpenGL matrixes
        extractMatrixes();

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
