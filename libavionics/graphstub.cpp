#include "graphstub.h"

using namespace xa;


static void drawBegin(struct SaslGraphicsCallbacks *canvas)
{
}


/// flush drawed graphics to screen
static void drawEnd(struct SaslGraphicsCallbacks *canvas)
{
}


/// load texture to memory.
/// Returns texture ID or -1 on failure.  On success returns texture width
//  and height in pixels
static int loadTexture(struct SaslGraphicsCallbacks *canvas,
        const char *buf, int length, int *width, int *height)
{
    return -1;
}


// Unload texture from video memory.
static void freeTexture(struct SaslGraphicsCallbacks *canvas, int textureId)
{
}


// draw line of specified color.
static void drawLine(struct SaslGraphicsCallbacks *canvas, double x1,
        double y1, double x2, double y2, double r, double g, double b, double a)
{
}


// draw untextured triangle.
static void drawTriangle(struct SaslGraphicsCallbacks *canvas, 
        double x1, double y1, double r1, double g1, double b1, double a1,
        double x2, double y2, double r2, double g2, double b2, double a2,
        double x3, double y3, double r3, double g3, double b3, double a3)
{
}


// draw textured triangle.
static void drawTexturedTriangle(struct SaslGraphicsCallbacks *canvas, 
        int textureId,
        double x1, double y1, double u1, double v1, double r1, double g1, double b1, double a1,
        double x2, double y2, double u2, double v2, double r2, double g2, double b2, double a2,
        double x3, double y3, double u3, double v3, double r3, double g3, double b3, double a3)
{
}


// enable clipping to rectangle
static void setClipArea(struct SaslGraphicsCallbacks *canvas, 
        double x1, double y1, double x2, double y2)
{
}


// disable clipping.
static void resetClipArea(struct SaslGraphicsCallbacks *canvas)
{
}


// push affine translation state
static void pushTransform(struct SaslGraphicsCallbacks *canvas)
{
}

// pop affine transform state
static void popTransform(struct SaslGraphicsCallbacks *canvas)
{
}


// apply move transform to current state
static void translateTransform(struct SaslGraphicsCallbacks *canvas, 
        double x, double y)
{
}


// apply scale transform to current state
static void scaleTransform(struct SaslGraphicsCallbacks *canvas, 
        double x, double y)
{
}

// apply rotate transform to current state
static void rotateTransform(struct SaslGraphicsCallbacks *canvas, 
        double angle)
{
}


// find sasl texture in memory by size and marker color
// returns texture id or -1 if not found
static int findTexture(struct SaslGraphicsCallbacks *canvas, 
        int width, int height, int *r, int *g, int *b, int *a)
{
    return -1;
}

// start rendering to texture
// pass -1 as texture ID to restore default render target
static int setRenderTarget(struct SaslGraphicsCallbacks *canvas, 
        int textureId)
{
    return -1;
}


// create new texture of specified size and store it under the same name 
// as old texture
// use it for textures used as render target
static void recreateTexture(struct SaslGraphicsCallbacks *canvas, 
        int textureId, int width, int height)
{
}


static struct SaslGraphicsCallbacks callbacks = { drawBegin, drawEnd,
    loadTexture, freeTexture, drawLine, drawTriangle, drawTexturedTriangle,
    setClipArea, resetClipArea, pushTransform, popTransform, 
    translateTransform, scaleTransform, rotateTransform, findTexture,
    setRenderTarget, recreateTexture };


SaslGraphicsCallbacks* xa::getGraphicsStub()
{
    return &callbacks;
}


