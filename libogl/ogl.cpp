#include "ogl.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SOIL.h>
#include "glheaders.h"


// graphics context
struct OglCanvas
{
    /// pointers to this callbacks
    struct XaGraphicsCallbacks callbacks;

    /// texture binder function (or NULL if not available)
    xagl_bind_texture_2d_callback binderCallback;

    /// texture ID generator (or NULL if not available)
    xagl_gen_tex_name_callback genTexNameCallback;

    // number of triangles drawn
    int triangles;

    /// number of lines drawn
    int lines;

    /// number of textures loaded
    int textures;

    /// size of loaded textures
    int texturesSize;

    // number of batches drawn
    int batches;

    /// current texture ID or 0 if texturing disabled
    int currentTexture;

    /// what to draw: GL_LINES or GL_TRIANGLES
    int currentMode;

    /// maximum size of vertex buffer
    int maxVertices;

    /// current number of vertices in buffer
    int numVertices;

    /// vertices buffer
    float *vertexBuffer;
    
    /// texture coords buffer
    float *texBuffer;
    
    /// vertex colors buffer
    float *colorBuffer;
};



/// initialize graphics before frame start
static void drawBegin(struct XaGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    c->currentTexture = -1;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, c->vertexBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, c->texBuffer);
    glColorPointer(4, GL_FLOAT, 0, c->colorBuffer);

    c->triangles = 0;
    c->lines = 0;
    c->batches = 0;
    
    c->numVertices = 0;
    c->currentTexture = 0;
    c->currentMode = GL_TRIANGLES;
}


/// make sure vertex buffer is large enough to fit qty vertices
/// \param qty - additional vertices to add to vertex buffer
static void reserveSpace(OglCanvas *c, int qty)
{
    if (c->numVertices + qty > c->maxVertices) {
        c->maxVertices += (qty / 1024 + 1) * 1024;
        int s = sizeof(float) * c->maxVertices;
        c->vertexBuffer = (float*)realloc(c->vertexBuffer, 2 * s);
        c->texBuffer = (float*)realloc(c->texBuffer, 2 * s);
        c->colorBuffer = (float*)realloc(c->colorBuffer, 4 * s);
        
        glVertexPointer(2, GL_FLOAT, 0, c->vertexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, c->texBuffer);
        glColorPointer(4, GL_FLOAT, 0, c->colorBuffer);
    }
}


/// Add vertex to buffers
static void addVertex(OglCanvas *c, double x, double y, 
        double r, double g, double b, double a,
        double u, double v)
{
    reserveSpace(c, 1);

    int i = c->numVertices * 2;
    c->vertexBuffer[i] = x;
    c->vertexBuffer[i + 1] = y;

    c->texBuffer[i] = u;
    c->texBuffer[i + 1] = v;

    i = c->numVertices * 4;
    c->colorBuffer[i] = r;
    c->colorBuffer[i + 1] = g;
    c->colorBuffer[i + 2] = b;
    c->colorBuffer[i + 3] = a;
 
    c->numVertices++;
}

/// draw vertices accumulated in buffers
static void dumpBuffers(OglCanvas *c)
{
    if (c->numVertices) {
        glDrawArrays(c->currentMode, 0, c->numVertices);
        c->numVertices = 0;
        c->batches++;
    }
}



/// flush drawed graphics to screen
static void drawEnd(struct XaGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    dumpBuffers(c);

    glPopAttrib();
    glPopClientAttrib();
/*    printf("textures: %i (%i Kb) triangles: %i lines: %i  batches: %i\n", 
            c->textures, c->texturesSize / 1024, c->triangles, c->lines,
            c->batches);*/
}


// stop texturing
static void disableTexture(OglCanvas *c)
{
    if (c->currentTexture) {
        dumpBuffers(c);
        glDisable(GL_TEXTURE_2D);
        c->currentTexture = 0;
    }
}


// start texturing
static void setTexture(OglCanvas *c, int texId)
{
    if (c->currentTexture != texId) {
        dumpBuffers(c);
        if (! c->currentTexture)
            glEnable(GL_TEXTURE_2D);
        if (c->binderCallback)
            c->binderCallback(texId);
        else
            glBindTexture(GL_TEXTURE_2D, texId);
        c->currentTexture = texId;
    }
}


// switch primitives to draw
static void setMode(OglCanvas *c, int mode)
{
    if (c->currentMode != mode) {
        dumpBuffers(c);
        c->currentMode = mode;
    }
}


/// load texture to memory.
/// Returns texture ID or -1 on failure.  On success returns texture width
//  and height in pixels
static int loadTexture(struct XaGraphicsCallbacks *canvas,
        const char *name, int *width, int *height)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (! c)
        return -1;

    GLuint texId = 0;
    if (c->genTexNameCallback)
        texId = c->genTexNameCallback();

    unsigned id = SOIL_load_OGL_texture(name, 0, texId, SOIL_FLAG_POWER_OF_TWO);
    if (! id) 
        return -1;
 
    texId = id;

    // because of SOIL issue
    setTexture(c, id);

    if (width) {
        GLint w;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        *width = w;
    }
    if (height) {
        GLint h;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        *height = h;
    }

    c->textures++;

    if (width && height)
        c->texturesSize += (*width) * (*height);

    return texId;
}


// Unload texture from video memory.
static void freeTexture(struct XaGraphicsCallbacks *canvas, int textureId)
{
    GLuint id = (GLuint)textureId;
    glDeleteTextures(1, &id);
}


// draw line of specified color.
static void drawLine(struct XaGraphicsCallbacks *canvas, double x1,
        double y1, double x2, double y2, double r, double g, double b, double a)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    c->lines++;

    disableTexture(c);
    setMode(c, GL_LINES);

    addVertex(c, x1, y1,  r, g, b, a, 0, 0);
    addVertex(c, x2, y2,  r, g, b, a, 0, 0);
}


// draw untextured triangle.
static void drawTriangle(struct XaGraphicsCallbacks *canvas, 
        double x1, double y1, double r1, double g1, double b1, double a1,
        double x2, double y2, double r2, double g2, double b2, double a2,
        double x3, double y3, double r3, double g3, double b3, double a3)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    c->triangles++;

    disableTexture(c);
    setMode(c, GL_TRIANGLES);

    addVertex(c, x1, y1,  r1, g1, b1, a1, 0, 0);
    addVertex(c, x2, y2,  r2, g2, b2, a2, 0, 0);
    addVertex(c, x3, y3,  r3, g3, b3, a3, 0, 0);
}


// draw textured triangle.
static void drawTexturedTriangle(struct XaGraphicsCallbacks *canvas, 
        int textureId,
        double x1, double y1, double u1, double v1, double r1, double g1, double b1, double a1,
        double x2, double y2, double u2, double v2, double r2, double g2, double b2, double a2,
        double x3, double y3, double u3, double v3, double r3, double g3, double b3, double a3)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (! c)
        return;
    
    c->triangles++;

    setTexture(c, textureId);
    setMode(c, GL_TRIANGLES);

    addVertex(c,  x1, y1,  r1, g1, b1, a1,  u1, v1);
    addVertex(c,  x2, y2,  r2, g2, b2, a2,  u2, v2);
    addVertex(c,  x3, y3,  r3, g3, b3, a3,  u3, v3);
}


// enable clipping to rectangle
static void setClipArea(struct XaGraphicsCallbacks *canvas, 
        double x1, double y1, double x2, double y2)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
}


// disable clipping.
static void resetClipArea(struct XaGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
}


// push affine translation state
static void pushTransform(struct XaGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
    glPushMatrix();
}

// pop affine transform state
static void popTransform(struct XaGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
    glPopMatrix();
}


// apply move transform to current state
static void translateTransform(struct XaGraphicsCallbacks *canvas, 
        double x, double y)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
    glTranslated(x, y, 0);
}


// apply scale transform to current state
static void scaleTransform(struct XaGraphicsCallbacks *canvas, 
        double x, double y)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
    glScaled(x, y, 1.0f);
}

// apply rotate transform to current state
static void rotateTransform(struct XaGraphicsCallbacks *canvas, 
        double angle)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
    glRotated(angle, 0, 0, -1.0);
}


// initializa canvas structure
struct XaGraphicsCallbacks* xagl_init_graphics()
{
    OglCanvas *c = new OglCanvas;

    memset(c, 0, sizeof(OglCanvas));

    c->callbacks.draw_begin = drawBegin;
    c->callbacks.draw_end = drawEnd;
    c->callbacks.load_texture = loadTexture;
    c->callbacks.free_texture = freeTexture;
    c->callbacks.draw_line = drawLine;
    c->callbacks.draw_triangle = drawTriangle;
    c->callbacks.draw_textured_triangle = drawTexturedTriangle;
    c->callbacks.set_clip_area = setClipArea;
    c->callbacks.reset_clip_area = resetClipArea;
    c->callbacks.push_transform = pushTransform;
    c->callbacks.pop_transform = popTransform;
    c->callbacks.translate_transform = translateTransform;
    c->callbacks.scale_transform = scaleTransform;
    c->callbacks.rotate_transform = rotateTransform;
    
    c->maxVertices = 1024;
    c->vertexBuffer = (float*)malloc(sizeof(float) * 2 * c->maxVertices);
    c->texBuffer = (float*)malloc(sizeof(float) * 2 * c->maxVertices);
    c->colorBuffer = (float*)malloc(sizeof(float) * 4 * c->maxVertices);

    return (struct XaGraphicsCallbacks*)c;
}


// free canvas structure
void xagl_done_graphics(struct XaGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (c) {
        free(c->vertexBuffer);
        free(c->texBuffer);
        free(c->colorBuffer);
        delete c;
    }
}

void xagl_set_texture2d_binder_callback(struct XaGraphicsCallbacks *canvas, 
        xagl_bind_texture_2d_callback binder)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (! c)
        return;
    c->binderCallback = binder;
}

/// Setup texture name generator function.
/// \param canvas graphics canvas.
/// \param generator ID generator. if NULL default OpenGL function will be used
void xagl_set_gen_tex_name_callback(struct XaGraphicsCallbacks *canvas, 
        xagl_gen_tex_name_callback generator)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (! c)
        return;
    c->genTexNameCallback = generator;
}

