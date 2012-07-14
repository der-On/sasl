#include "ogl.h"


#include <list>
#include <map>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <SOIL.h>
#include "glheaders.h"
#include "math2d.h"


#if !defined(WIN32) && !defined(__APPLE__)
	#include <GL/gl.h>
	#include <GL/glx.h>
#elif defined(__APPLE__)
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
#else
	#include <GL/gl.h>
#endif


#ifndef __APPLE__
// OpenGL functions
typedef void (*GenFramebuffers)(GLsizei n, GLuint *buffers);
static GenFramebuffers glGenFramebuffers = NULL;

typedef void (*BindFramebuffer)(GLenum target, GLuint framebuffer);
static BindFramebuffer glBindFramebuffer = NULL;

typedef void (*FramebufferTexture2D)(GLenum target, GLenum attachment, 
        GLenum textarget, GLuint texture, GLint level);
static FramebufferTexture2D glFramebufferTexture2D = NULL;

typedef void (*DeleteFramebuffers)(GLsizei n, GLuint *buffers);
static DeleteFramebuffers glDeleteFramebuffers = NULL;

typedef void (*GenerateMipmap)(GLenum target);
static GenerateMipmap glGenerateMipmap = NULL;

// opengl defines
#ifndef GL_DRAW_FRAMEBUFFER_BINDING
#define GL_DRAW_FRAMEBUFFER_BINDING       GL_FRAMEBUFFER_BINDING
#endif

#ifndef GL_FRAMEBUFFER_BINDING
#define GL_FRAMEBUFFER_BINDING            0x8CA6
#endif

#ifndef GL_DRAW_FRAMEBUFFER
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#endif

#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0              0x8CE0
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE                  0x812F
#endif

#endif

// graphics context
struct OglCanvas
{
    /// pointers to this callbacks
    struct SaslGraphicsCallbacks callbacks;

    /// texture binder function (or NULL if not available)
    saslgl_bind_texture_2d_callback binderCallback;

    /// texture ID generator (or NULL if not available)
    saslgl_gen_tex_name_callback genTexNameCallback;

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

    // number of batches because of texture changed
    int batchTex;
    
    // number of batches because of translations
    int batchTrans;
    
    // number of batches because of untextured geometry
    int batchNoTex;
    
    // number of batches because of switch to lines
    int batchLines;

    /// current texture ID or 0 if texturing disabled
    int currentTexture;

    /// what to draw: GL_LINES or GL_TRIANGLES
    int currentMode;

    /// maximum size of vertex buffer
    int maxVertices;

    /// current number of vertices in buffer
    int numVertices;

    /// vertices buffer
    GLfloat *vertexBuffer;
    
    /// texture coords buffer
    GLfloat *texBuffer;
    
    /// vertex colors buffer
    GLfloat *colorBuffer;

    // transformation stack
    std::vector<Matrix> transform;

    // true if FBO functions allowed to use
    bool fboAvailable;

    // map of FBOs by texture IDs
    std::map<int, GLuint> fboByTex;

    // default fbo
    GLuint defaultFbo;

    // texture assigned to current fbo
    int currentFboTex;
};



/// initialize graphics before frame start
static void drawBegin(struct SaslGraphicsCallbacks *canvas)
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
    if (c->vertexBuffer) {
        glVertexPointer(2, GL_FLOAT, 0, c->vertexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, c->texBuffer);
        glColorPointer(4, GL_FLOAT, 0, c->colorBuffer);
    }

    c->triangles = 0;
    c->lines = 0;
    c->batches = 0;

    c->batchTex = 0;
    c->batchTrans = 0;
    c->batchNoTex = 0;
    c->batchLines = 0;
    
    c->numVertices = 0;
    c->currentTexture = 0;
    c->currentMode = GL_TRIANGLES;

    c->transform.clear();
    c->transform.push_back(Matrix::identity());
}


/// make sure vertex buffer is large enough to fit qty vertices
/// \param qty - additional vertices to add to vertex buffer
static void reserveSpace(OglCanvas *c, int qty)
{
    if (c->numVertices + qty > c->maxVertices) {
        c->maxVertices += (qty / 1024 + 1) * 1024;
        int s = sizeof(GLfloat) * c->maxVertices;
        c->vertexBuffer = (GLfloat*)realloc(c->vertexBuffer, 2 * s);
        c->texBuffer = (GLfloat*)realloc(c->texBuffer, 2 * s);
        c->colorBuffer = (GLfloat*)realloc(c->colorBuffer, 4 * s);
        
        glVertexPointer(2, GL_FLOAT, 0, c->vertexBuffer);
        glTexCoordPointer(2, GL_FLOAT, 0, c->texBuffer);
        glColorPointer(4, GL_FLOAT, 0, c->colorBuffer);
    }
}


/// Add vertex to buffers
static void addVertex(OglCanvas *c, GLfloat x, GLfloat y, 
        GLfloat r, GLfloat g, GLfloat b, GLfloat a,
        GLfloat u, GLfloat v)
{
    reserveSpace(c, 1);

    Vector rv = c->transform.back() * Vector(x, y);
    x = rv.getX();
    y = rv.getY();

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
static void drawEnd(struct SaslGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    dumpBuffers(c);

    glPopAttrib();
    glPopClientAttrib();
/*    printf("textures: %i (%i Kb) triangles: %i lines: %i  batches: %i\n", 
            c->textures, c->texturesSize / 1024, c->triangles, c->lines,
            c->batches);
    printf("batches reasons: textures: %i  translation: %i  notex: %i  "
            "lines: %i\n", c->batchTex, c->batchTrans, c->batchNoTex, 
            c->batchLines);*/
}


// stop texturing
static void disableTexture(OglCanvas *c)
{
    if (c->currentTexture) {
        if (c->numVertices)
            c->batchNoTex++;
        dumpBuffers(c);
        glDisable(GL_TEXTURE_2D);
        c->currentTexture = 0;
    }
}


// start texturing
static void setTexture(OglCanvas *c, int texId)
{
    if (c->currentTexture != texId) {
        if (c->numVertices)
            c->batchTex++;
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
        if (c->numVertices)
            c->batchLines++;
        dumpBuffers(c);
        c->currentMode = mode;
    }
}


/// load texture to memory.
/// Returns texture ID or -1 on failure.  On success returns texture width
//  and height in pixels
static int loadTexture(struct SaslGraphicsCallbacks *canvas,
        const char *buffer, int length, int *width, int *height)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (! c)
        return -1;

    GLuint texId = 0;
    if (c->genTexNameCallback)
        texId = c->genTexNameCallback();

    unsigned id = SOIL_load_OGL_texture_from_memory(
            (const unsigned char*)buffer, length, 
            0, texId, SOIL_FLAG_POWER_OF_TWO);
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
static void freeTexture(struct SaslGraphicsCallbacks *canvas, int textureId)
{
    GLuint id = (GLuint)textureId;
    glDeleteTextures(1, &id);
}


// draw line of specified color.
static void drawLine(struct SaslGraphicsCallbacks *canvas, double x1,
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
static void drawTriangle(struct SaslGraphicsCallbacks *canvas, 
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
static void drawTexturedTriangle(struct SaslGraphicsCallbacks *canvas, 
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
static void setClipArea(struct SaslGraphicsCallbacks *canvas, 
        double x1, double y1, double x2, double y2)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
}


// disable clipping.
static void resetClipArea(struct SaslGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    dumpBuffers(c);
}


// push affine translation state
static void pushTransform(struct SaslGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
/*    if (c->numVertices)
        c->batchTrans++;
    dumpBuffers(c);
    glPushMatrix();*/
    Matrix m = c->transform.back();
    c->transform.push_back(m);
}

// pop affine transform state
static void popTransform(struct SaslGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
/*    if (c->numVertices)
        c->batchTrans++;
    dumpBuffers(c);
    glPopMatrix();*/
    if (1 < c->transform.size())
        c->transform.pop_back();
    else
        printf("invalid pop!\n");
}


// apply move transform to current state
static void translateTransform(struct SaslGraphicsCallbacks *canvas, 
        double x, double y)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
/*    if (c->numVertices)
        c->batchTrans++;
    dumpBuffers(c);
    glTranslated(x, y, 0);*/
    c->transform.back() = Matrix::translate(x, y) * c->transform.back();
}


// apply scale transform to current state
static void scaleTransform(struct SaslGraphicsCallbacks *canvas, 
        double x, double y)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    /*if (c->numVertices)
        c->batchTrans++;
    dumpBuffers(c);
    glScaled(x, y, 1.0f);*/
    c->transform[c->transform.size() - 1] = Matrix::scale(x, y) * c->transform.back();
}

// apply rotate transform to current state
static void rotateTransform(struct SaslGraphicsCallbacks *canvas, 
        double angle)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);
    /*if (c->numVertices)
        c->batchTrans++;
    dumpBuffers(c);
    glRotated(angle, 0, 0, -1.0);*/
    c->transform.back() = Matrix::rotate(-angle) * c->transform.back();
}


// find sasl texture in memory by size and marker color
// returns texture id or -1 if not found
static int findTexture(struct SaslGraphicsCallbacks *canvas, 
        int width, int height, int *r, int *g, int *b, int *a)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    unsigned char *buf = new unsigned char[4*width*height];

    for (GLuint i = 0; i < 2048; i++) {
        if (glIsTexture(i)) {
            GLint w, h;
            glBindTexture(GL_TEXTURE_2D, i);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
            if ((w == width) && (h == height)) {
                if (a && r && g && b) {
                    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
                    if ((10 > abs(*r - buf[0])) && (10 > abs(*g - buf[1])) && 
                            (10 > abs(*b - buf[2])) && (10 > abs(*a - buf[3])))
                    {
                        if (c->currentTexture)
                            glBindTexture(GL_TEXTURE_2D, c->currentTexture);
                        delete[] buf;
                        return i;
                    }
                } else {
                    if (c->currentTexture)
                        glBindTexture(GL_TEXTURE_2D, c->currentTexture);
                    delete[] buf;
                    return i;
                }
            }
        }
    }

    if (c->currentTexture)
        glBindTexture(GL_TEXTURE_2D, c->currentTexture);
    delete[] buf;
    
    return -1;
}



// returns fbo currently binded
static GLuint getCurrentFbo()
{
    GLuint oldFbo;
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&oldFbo);

    return oldFbo;
}



// find or allocate FBO object
static GLuint getFbo(OglCanvas *c, int textureId)
{
    std::map<int, GLuint>::const_iterator i = c->fboByTex.find(textureId);
    if (i == c->fboByTex.end()) {
        GLuint fbo = -1;
        glGenFramebuffers(1, &fbo);
        GLuint oldFbo = getCurrentFbo();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                GL_TEXTURE_2D, textureId, 0);
        // job done, switch to old fbo
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldFbo);
        c->fboByTex[textureId] = fbo;
        return fbo;
    } else
        return (*i).second;
}

// setup matrices
static void prepareFbo(OglCanvas *c, int textureId, int width, int height)
{
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    c->transform.push_back(Matrix::identity());
}


// start rendering to texture
// pass -1 as texture ID to restore default render target
static int setRenderTarget(struct SaslGraphicsCallbacks *canvas, 
        int textureId)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    dumpBuffers(c);

    if (! c->fboAvailable)
        return -1;

    if (-1 != textureId) {
        // save state
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

        GLint w, h;
        glBindTexture(GL_TEXTURE_2D, textureId);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

        // enable fbo
        c->defaultFbo = getCurrentFbo();
        GLuint fbo = getFbo(c, textureId);
        if ((GLuint)-1 == fbo)
            return -1;
        c->currentFboTex = textureId;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                GL_TEXTURE_2D, textureId, 0); 
        
        prepareFbo(c, textureId, w, h);
    } else {
        // restore default fbo
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, c->defaultFbo);
        glBindTexture(GL_TEXTURE_2D, c->currentFboTex);
        glGenerateMipmap(GL_TEXTURE);
        glBindTexture(GL_TEXTURE_2D, 0);

        // restore x-plane state
        glPopClientAttrib();
        glPopAttrib();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    
        if (c->currentTexture)
            glBindTexture(GL_TEXTURE_2D, c->currentTexture);
        
        c->transform.pop_back();
    }

    return 0;
}


// create new texture of specified size and store it under the same name 
// as old texture
// use it for textures used as render target
static void recreateTexture(struct SaslGraphicsCallbacks *canvas, 
        int textureId, int width, int height)
{
    OglCanvas *c = (OglCanvas*)canvas;
    assert(canvas);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, 
            GL_BYTE, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    if (c->currentTexture)
        glBindTexture(GL_TEXTURE_2D, c->currentTexture);
}

#ifndef __APPLE__
// returns address of OpenGL functions.  check EXT variants if normal not found
typedef void (*Func)();
static Func getProcAddress(const char *name)
{
#ifndef WIN32
    Func res = glXGetProcAddressARB((GLubyte*)name);
#else
    Func res = (Func)wglGetProcAddress(name);
#endif
    if (! res) {
        char buf[250];
        strcpy(buf, name);
        strcat(buf, "EXT");
#ifndef WIN32
        res = glXGetProcAddressARB((GLubyte*)buf);
#else
        res = (Func)wglGetProcAddress(buf);
#endif
    }
    return res;
}


// find pointers of OpenGL functions
static bool initGlFunctions()
{
    glGenFramebuffers = (GenFramebuffers)getProcAddress("glGenFramebuffers");
    glBindFramebuffer = (BindFramebuffer)getProcAddress("glBindFramebuffer");
    glFramebufferTexture2D = (FramebufferTexture2D)getProcAddress("glFramebufferTexture2D");
    glDeleteFramebuffers = (DeleteFramebuffers)getProcAddress("glDeleteFramebuffers");
    glGenerateMipmap = (GenerateMipmap)getProcAddress("glGenerateMipmap");

    return glGenFramebuffers && glBindFramebuffer && glFramebufferTexture2D &&
        glDeleteFramebuffers && glGenerateMipmap;
}

#else

static bool initGlFunctions()
{
    return 1;
}

#endif


// initializa canvas structure
struct SaslGraphicsCallbacks* saslgl_init_graphics()
{
    OglCanvas *c = new OglCanvas;

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
    c->callbacks.find_texture = findTexture;
    c->callbacks.set_render_target = setRenderTarget;
    c->callbacks.recreate_texture = recreateTexture;
    
    c->maxVertices = c->numVertices = 0;
    c->vertexBuffer = c->texBuffer = c->colorBuffer = NULL;
    c->fboAvailable = initGlFunctions();

    return (struct SaslGraphicsCallbacks*)c;
}


// free canvas structure
void saslgl_done_graphics(struct SaslGraphicsCallbacks *canvas)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (c) {
        if (c->fboByTex.size()) {
            for (std::map<int, GLuint>::iterator i = c->fboByTex.begin();
                    i != c->fboByTex.end(); i++)
                glDeleteFramebuffers(1, &(*i).second);
        }

        free(c->vertexBuffer);
        free(c->texBuffer);
        free(c->colorBuffer);
        delete c;
    }
}

void saslgl_set_texture2d_binder_callback(struct SaslGraphicsCallbacks *canvas, 
        saslgl_bind_texture_2d_callback binder)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (! c)
        return;
    c->binderCallback = binder;
}

/// Setup texture name generator function.
/// \param canvas graphics canvas.
/// \param generator ID generator. if NULL default OpenGL function will be used
void saslgl_set_gen_tex_name_callback(struct SaslGraphicsCallbacks *canvas, 
        saslgl_gen_tex_name_callback generator)
{
    OglCanvas *c = (OglCanvas*)canvas;
    if (! c)
        return;
    c->genTexNameCallback = generator;
}
