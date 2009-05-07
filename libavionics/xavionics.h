#ifndef __X_AVIONICS_CORE_H__
#define __X_AVIONICS_CORE_H__

/// \file X-Avionics core library
/// It contains basic features for avionics modeling.


#ifndef __cplusplus
extern "C" {
#endif

#include "xcallbacks.h"
#include "xconsts.h"



/// Internal X-Avionics structure
struct XAvionics;

/// X-Avionics instance handler
typedef struct XAvionics *XA;

struct lua_State;


/// Initialize X-Avionics library.
/// It loads all internal scripts and components definitions.
/// Returns X-Avionics handler to use in other functions.
/// \param path X-Avionics root directory.
XA xa_init(const char *path);

/// Unload X-Avionics.
/// \param xa X-Avionics handler.
void xa_done(XA xa);

/// Set size of panel.
/// \param xa X-Avionics handler.
/// \param width width of panel in pixels.
/// \param height height of panel in pixels.
void xa_set_panel_size(XA xa, int width, int height);

/// Set size of popup layer
/// \param xa X-Avionics handler.
/// \param width width of popup layer in pixels.
/// \param height height of popup in pixels.
void xa_set_popup_size(XA xa, int width, int height);

/// Load panel from file.
/// Returns non-zero if error was detected.
/// \param xa X-Avionics handler.
/// \param path path to panel file
int xa_load_panel(XA xa, const char *path);

/// Handle key down event
/// \param xa X-Avionics handler.
/// \param keycode code of key.
void xa_key_down(XA xa, int keycode);

/// Handle key up event.
/// \param xa X-Avionics handler.
/// \param keycode code of key.
void xa_key_up(XA xa, int keycode);

/// Handle mouse down event
/// \param xa X-Avionics handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param button number of button
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int xa_mouse_button_down(XA xa, int x, int y, int button, int layer);

/// Handle mouse up event
/// \param xa X-Avionics handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param button number of button
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int xa_mouse_button_up(XA xa, int x, int y, int button, int layer);

/// Handle mouse click event
/// Mouse click is periodical event, it can be emulated by xavionics
/// \param xa X-Avionics handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param button number of button
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int xa_mouse_button_click(XA xa, int x, int y, int button, int layer);

/// Handle mouse move event
/// \param xa X-Avionics handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int xa_mouse_move(XA xa, int x, int y, int layer);

/// Setup parameters for mouse click emulations
/// \param xa X-Avionics handler.
/// \param delay number of milliseconds before second click event
/// \param period number of milliseconds before third and other click events
int xa_set_click_params(XA xa, int delay, int period);

/// Enable or diable mouse click emulation
int xa_enable_click_emulator(XA xa, int enable);

/// Update gauges.
/// Call it on each frame
/// \param xa X-Avionics handler.
int xa_update(XA xa);

/// Draw panel.
/// It assumes that screen that rendering context was setup properly
/// before this call.
/// \param xa X-Avionics handler.
/// \param stage what to draw: STAGE_GAUGES, STAGE_POPUPS or STAGE_ALL
int xa_draw_panel(XA xa, int stage);

/// Setup texture binder function
/// \param xa X-Avionics handler.
/// \param binder texture binder. if NULL default OpenGL function will be used
void xa_set_texture2d_binder_callback(XA xa, 
        xa_bind_texture_2d_callback binder);

/// Setup texture name generator function.
/// \param xa X-Avionics handler.
/// \param generator ID generator. if NULL default OpenGL function will be used
void xa_set_gen_tex_name_callback(XA xa, xa_gen_tex_name_callback generator);


/// Returns pointer to LUA machine
/// \param xa X-Avionics handler.
struct lua_State* xa_get_lua(XA xa);


// Properties related functions


/// Setup properties callbacks
/// Returns zero on success or something other if failed
/// \param xa X-Avionics handler.
/// \param callbacks structure full of properties callbacks
int xa_set_props(XA xa, struct PropsCallbacks *callbacks, Props props);


/// Returns reference to property or NULL if property doesn't exists.
/// \param xa X-Avionics handler.
/// \param name name of property.
/// \param type must be PROP_INT, PROP_FLOAT or PROP_DOUBLE
PropRef xa_get_prop_ref(XA xa, const char *name, int type);


/// Returns reference to property or NULL if property doesn't exists.
/// \param xa X-Avionics handler.
/// \param name name of property.
/// \param type must be PROP_INT, PROP_FLOAT or PROP_DOUBLE
PropRef xa_create_prop(XA xa, const char *name, int type);


/// Destroy unneeded property reference
/// \param xa X-Avionics handler.
/// \param ref reference to property.
void xa_free_prop_ref(XA xa, PropRef ref);


/// Returns value of property as integer or zero on error.
/// \param xa X-Avionics handler.
/// \param ref reference to property.
/// \param err optional pointer to error status.  if not NULL, sets to
///            zero on success or to non-zero on errors
int xa_get_prop_int(XA xa, PropRef ref, int *err);


/// Set value of integer property.
/// Returns 0 on success
/// \param xa X-Avionics handler.
/// \param ref reference to property.
/// \param value new value of property.
int xa_set_prop_int(XA xa, PropRef ref, int value);


/// Returns value of property as float or zero on error.
/// \param xa X-Avionics handler.
/// \param ref reference to property.
/// \param err optional pointer to error status.  if not NULL, sets to
///            zero on success or to non-zero on errors
float xa_get_prop_float(XA xa, PropRef ref, int *err);

/// Set value of float property.
/// Returns 0 on success
/// \param xa X-Avionics handler.
/// \param ref reference to property.
/// \param value new value of property.
int xa_set_prop_float(XA xa, PropRef ref, float value);


/// Returns value of property as double or zero on error.
/// \param xa X-Avionics handler.
/// \param ref reference to property.
/// \param err optional pointer to error status.  if not NULL, sets to
///            zero on success or to non-zero on errors
double xa_get_prop_double(XA xa, PropRef ref, int *err);

/// Set value of double property.
/// Returns 0 on success
/// \param xa X-Avionics handler.
/// \param ref reference to property.
/// \param value new value of property.
int xa_set_prop_double(XA xa, PropRef ref, double value);


/// Set color of texture background
/// \param xa X-Avionics handler.
/// \param r color red component
/// \param g color green component
/// \param b color blue component
/// \param a color alpha component
int xa_set_background_color(XA xa, float r, float g, float b, float a);


/// Enable or disable highlight of clickable regions.
/// Returns zero on success
/// \param xa X-Avionics handler.
/// \param show non-zero to show clickable areas
int xa_set_show_clickable(XA xa, int show);


// networked properties functions


/// Run networked properties server on specified port
/// Returns zero on success.
/// \param xa X-Avionics handler.
/// \param port port to listen
/// \param secret secret word for clients authentication
int xa_start_netprop_server(XA xa, int port, const char *secret);


/// Stop networked properties server
/// \param xa X-Avionics handler.
void xa_stop_netprop_server(XA xa);


/// Connect local properties to remote server.
/// Returns zero on success.
/// \param xa X-Avionics handler.
/// \param host address of host to connect
/// \param port port to listen
/// \param secret secret word for clients authentication
int xa_connect_to_server(XA xa, const char *host, int port, 
        const char *secret);


#ifndef __cplusplus
extern "C" {
#endif


#endif

