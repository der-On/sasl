#ifndef __OGL_H__
#define __OGL_H__


// implementation of OpenGL graphics canvas for libavionics


#include "../libavionics/xavionics.h"


#ifdef __cplusplus
extern "C" {
#endif

/// Texture binder callback.
/// This is because of X-Plane use strange texture system
/// \param textureId texture ID
typedef void (*xagl_bind_texture_2d_callback)(int textureId);


/// Generates ID for one texture
typedef int (*xagl_gen_tex_name_callback)();


/// returns pointer to canvas structure
struct XaGraphicsCallbacks* xagl_init_graphics();

/// finish canvas
void xagl_done_graphics(struct XaGraphicsCallbacks *canvas);

/// Setup texture binder function
/// \param canvas graphics canvas.
/// \param binder texture binder. if NULL default OpenGL function will be used
void xagl_set_texture2d_binder_callback(struct XaGraphicsCallbacks *canvas, 
        xagl_bind_texture_2d_callback binder);

/// Setup texture name generator function.
/// \param canvas graphics canvas.
/// \param generator ID generator. if NULL default OpenGL function will be used
void xagl_set_gen_tex_name_callback(struct XaGraphicsCallbacks *canvas, 
        xagl_gen_tex_name_callback generator);




#ifdef __cplusplus
}
#endif

#endif

