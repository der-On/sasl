#ifndef __OGL_H__
#define __OGL_H__


// implementation of OpenGL graphics canvas for libavionics


#include "../libavionics/libavionics.h"


#ifdef __cplusplus
extern "C" {
#endif

/// Texture binder callback.
/// This is because of X-Plane use strange texture system
/// \param textureId texture ID
typedef void (*saslgl_bind_texture_2d_callback)(int textureId);


/// Generates ID for one texture
typedef int (*saslgl_gen_tex_name_callback)();


/// returns pointer to canvas structure
struct SaslGraphicsCallbacks* saslgl_init_graphics();

/// finish canvas
void saslgl_done_graphics(struct SaslGraphicsCallbacks *canvas);

/// Setup texture binder function
/// \param canvas graphics canvas.
/// \param binder texture binder. if NULL default OpenGL function will be used
void saslgl_set_texture2d_binder_callback(struct SaslGraphicsCallbacks *canvas, 
        saslgl_bind_texture_2d_callback binder);

/// Setup texture name generator function.
/// \param canvas graphics canvas.
/// \param generator ID generator. if NULL default OpenGL function will be used
void saslgl_set_gen_tex_name_callback(struct SaslGraphicsCallbacks *canvas, 
        saslgl_gen_tex_name_callback generator);




#ifdef __cplusplus
}
#endif

#endif

