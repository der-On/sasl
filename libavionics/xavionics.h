#ifndef __X_AVIONICS_CORE_H__
#define __X_AVIONICS_CORE_H__

/// \file X-Avionics core library
/// It contains basic features for avionics modeling.


#if defined(__cplusplus)
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
/// \param charcode code of character.
/// \param keycode code of key.
/// returns non-zero if event was handled
int xa_key_down(XA xa, int charcode, int keycode);

/// Handle key up event.
/// \param xa X-Avionics handler.
/// \param charcode code of character.
/// \param keycode code of key.
/// returns non-zero if event was handled
int xa_key_up(XA xa, int charcode, int keycode);

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

/// Setup graphics functions
/// \param xa X-Avionics handler.
/// \param callbacks graphics callbacks functions.
void xa_set_graphics_callbacks(XA xa, 
        struct XaGraphicsCallbacks *callbacks);

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


/// Commands API
void xa_set_commands(XA xa, struct XaCommandCallbacks *callbacks, void *data);


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


// Sound API

/// Setup sound engine
/// \param xa X-Avionics handler.
/// \param callbacks sound engine callbacks.
/// \param data custom data for sound engine
void xa_set_sound_engine(XA xa, struct XaSoundCallbacks *callbacks, void *data);

/// Load sample into memory.  Returns sample handler or 0 if can't load sample
/// \param xa X-Avionics handler.
/// \param fileName path to sample on disk
int xa_sample_load(XA xa, const char *fileName);

// Play loaded sample
/// \param xa X-Avionics handler.
/// \param sampleId sample handler
/// \param loop if non-zero sound will be looped
void xa_sample_play(XA xa, int sampleId, int loop);

/// Stop playing sample
/// \param xa X-Avionics handler.
/// \param sampleId sample handler
void xa_sample_stop(XA xa, int sampleId);

/// Set gain of sample
/// \param xa X-Avionics handler.
/// \param sampleId sample handler
/// \param gain gain ratio from 0 to 1000
void xa_sample_set_gain(XA xa, int sampleId, int gain);

/// Set pitch of sample
/// \param xa X-Avionics handler.
/// \param sampleId sample handler
/// \param pitch pitch ratio from 0 to 1000
void xa_sample_set_pitch(XA xa, int sampleId, int pitch);

/// Rewind sample to beginning
/// \param xa X-Avionics handler.
/// \param sampleId sample handler
void xa_sample_rewind(XA xa, int sampleId);

/// Returns non-zero if sample is playing now or zero if not
/// \param xa X-Avionics handler.
/// \param sampleId sample handler
int xa_sample_is_playing(XA xa, int sampleId);


/// Set gain of all samples
/// \param xa X-Avionics handler.
/// \param gain gain ratio from 0 to 1000
void xa_set_master_gain(XA xa, int gain);


#if defined(__cplusplus)
}  /* extern "C" */
#endif


#endif

