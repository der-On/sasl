#ifndef __SASL_H__
#define __SASL_H__

/// \file SASL core library
/// It contains basic features for avionics simulation.


#if defined(__cplusplus)
extern "C" {
#endif

#include "libavcallbacks.h"
#include "libavconsts.h"



/// Internal SASL structure
struct Sasl;

/// SASL instance handler
typedef struct Sasl *SASL;

struct lua_State;


/// Initialize SASL library.
/// It loads all internal scripts and components definitions.
/// Returns SASL handler to use in other functions.
/// \param path SASL root directory.
SASL sasl_init(const char *path);

/// Unload SASL.
/// \param sasl SASL handler.
void sasl_done(SASL sasl);

/// Set size of panel.
/// \param sasl SASL handler.
/// \param width width of panel in pixels.
/// \param height height of panel in pixels.
void sasl_set_panel_size(SASL sasl, int width, int height);

/// Set size of popup layer
/// \param sasl SASL handler.
/// \param width width of popup layer in pixels.
/// \param height height of popup in pixels.
void sasl_set_popup_size(SASL sasl, int width, int height);

/// Load panel from file.
/// Returns non-zero if error was detected.
/// \param sasl SASL handler.
/// \param path path to panel file
int sasl_load_panel(SASL sasl, const char *path);

/// Handle key down event
/// \param sasl SASL handler.
/// \param charcode code of character.
/// \param keycode code of key.
/// returns non-zero if event was handled
int sasl_key_down(SASL sasl, int charcode, int keycode);

/// Handle key up event.
/// \param sasl SASL handler.
/// \param charcode code of character.
/// \param keycode code of key.
/// returns non-zero if event was handled
int sasl_key_up(SASL sasl, int charcode, int keycode);

/// Handle mouse down event
/// \param sasl SASL handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param button number of button
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int sasl_mouse_button_down(SASL sasl, int x, int y, int button, int layer);

/// Handle mouse up event
/// \param sasl SASL handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param button number of button
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int sasl_mouse_button_up(SASL sasl, int x, int y, int button, int layer);

/// Handle mouse click event
/// Mouse click is periodical event, it can be emulated by xavionics
/// \param sasl SASL handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param button number of button
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int sasl_mouse_button_click(SASL sasl, int x, int y, int button, int layer);

/// Handle mouse move event
/// \param sasl SASL handler.
/// \param x x coord of of mouse
/// \param x y coord of of mouse
/// \param layer 1 - popup layer, 2 - panel layer, 3 - both
int sasl_mouse_move(SASL sasl, int x, int y, int layer);

/// Setup parameters for mouse click emulations
/// \param sasl SASL handler.
/// \param delay number of milliseconds before second click event
/// \param period number of milliseconds before third and other click events
int sasl_set_click_params(SASL sasl, int delay, int period);

/// Enable or diable mouse click emulation
int sasl_enable_click_emulator(SASL sasl, int enable);

/// Update gauges.
/// Call it on each frame
/// \param sasl SASL handler.
int sasl_update(SASL sasl);

/// Draw panel.
/// It assumes that screen that rendering context was setup properly
/// before this call.
/// \param sasl SASL handler.
/// \param stage what to draw: STAGE_GAUGES, STAGE_POPUPS or STAGE_ALL
int sasl_draw_panel(SASL sasl, int stage);

/// Setup graphics functions
/// \param sasl SASL handler.
/// \param callbacks graphics callbacks functions.
void sasl_set_graphics_callbacks(SASL sasl, 
        struct SaslGraphicsCallbacks *callbacks);

/// Returns pointer to LUA machine
/// \param sasl SASL handler.
struct lua_State* sasl_get_lua(SASL sasl);


// Properties related functions


/// Setup properties callbacks
/// Returns zero on success or something other if failed
/// \param sasl SASL handler.
/// \param callbacks structure full of properties callbacks
int sasl_set_props(SASL sasl, struct SaslPropsCallbacks *callbacks, SaslProps props);


/// Returns reference to property or NULL if property doesn't exists.
/// \param sasl SASL handler.
/// \param name name of property.
/// \param type must be PROP_INT, PROP_FLOAT or PROP_DOUBLE
SaslPropRef sasl_get_prop_ref(SASL sasl, const char *name, int type);


/// Returns reference to property or NULL if property doesn't exists.
/// \param sasl SASL handler.
/// \param name name of property.
/// \param type must be PROP_INT, PROP_FLOAT or PROP_DOUBLE
SaslPropRef sasl_create_prop(SASL sasl, const char *name, int type);


/// Destroy unneeded property reference
/// \param sasl SASL handler.
/// \param ref reference to property.
void sasl_free_prop_ref(SASL sasl, SaslPropRef ref);


/// Returns value of property as integer or zero on error.
/// \param sasl SASL handler.
/// \param ref reference to property.
/// \param err optional pointer to error status.  if not NULL, sets to
///            zero on success or to non-zero on errors
int sasl_get_prop_int(SASL sasl, SaslPropRef ref, int *err);


/// Set value of integer property.
/// Returns 0 on success
/// \param sasl SASL handler.
/// \param ref reference to property.
/// \param value new value of property.
int sasl_set_prop_int(SASL sasl, SaslPropRef ref, int value);


/// Returns value of property as float or zero on error.
/// \param sasl SASL handler.
/// \param ref reference to property.
/// \param err optional pointer to error status.  if not NULL, sets to
///            zero on success or to non-zero on errors
float sasl_get_prop_float(SASL sasl, SaslPropRef ref, int *err);

/// Set value of float property.
/// Returns 0 on success
/// \param sasl SASL handler.
/// \param ref reference to property.
/// \param value new value of property.
int sasl_set_prop_float(SASL sasl, SaslPropRef ref, float value);


/// Returns value of property as double or zero on error.
/// \param sasl SASL handler.
/// \param ref reference to property.
/// \param err optional pointer to error status.  if not NULL, sets to
///            zero on success or to non-zero on errors
double sasl_get_prop_double(SASL sasl, SaslPropRef ref, int *err);

/// Set value of double property.
/// Returns 0 on success
/// \param sasl SASL handler.
/// \param ref reference to property.
/// \param value new value of property.
int sasl_set_prop_double(SASL sasl, SaslPropRef ref, double value);


/// Set color of texture background
/// \param sasl SASL handler.
/// \param r color red component
/// \param g color green component
/// \param b color blue component
/// \param a color alpha component
int sasl_set_background_color(SASL sasl, float r, float g, float b, float a);


/// Enable or disable highlight of clickable regions.
/// Returns zero on success
/// \param sasl SASL handler.
/// \param show non-zero to show clickable areas
int sasl_set_show_clickable(SASL sasl, int show);


/// Commands API
void sasl_set_commands(SASL sasl, struct SaslCommandCallbacks *callbacks, void *data);


// networked properties functions


/// Run networked properties server on specified port
/// Returns zero on success.
/// \param sasl SASL handler.
/// \param port port to listen
/// \param secret secret word for clients authentication
int sasl_start_netprop_server(SASL sasl, int port, const char *secret);


/// Stop networked properties server
/// \param sasl SASL handler.
void sasl_stop_netprop_server(SASL sasl);


/// Connect local properties to remote server.
/// Returns zero on success.
/// \param sasl SASL handler.
/// \param host address of host to connect
/// \param port port to listen
/// \param secret secret word for clients authentication
int sasl_connect_to_server(SASL sasl, const char *host, int port, 
        const char *secret);


// Sound API

/// Setup sound engine
/// \param sasl SASL handler.
/// \param callbacks sound engine callbacks.
void sasl_set_sound_engine(SASL sasl, struct SaslSoundCallbacks *callbacks);

/// Load sample into memory.  Returns sample handler or 0 if can't load sample
/// \param sasl SASL handler.
/// \param fileName path to sample on disk
int sasl_sample_load(SASL sasl, const char *fileName);

/// Unload sample
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_unload(SASL sasl, int sampleId);

// Play loaded sample
/// \param sasl SASL handler.
/// \param sampleId sample handler
/// \param loop if non-zero sound will be looped
void sasl_sample_play(SASL sasl, int sampleId, int loop);

/// Stop playing sample
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_stop(SASL sasl, int sampleId);

/// Set gain of sample
/// \param sasl SASL handler.
/// \param sampleId sample handler
/// \param gain gain ratio from 0 to 1000
void sasl_sample_set_gain(SASL sasl, int sampleId, int gain);

/// Set pitch of sample
/// \param sasl SASL handler.
/// \param sampleId sample handler
/// \param pitch pitch ratio from 0 to 1000
void sasl_sample_set_pitch(SASL sasl, int sampleId, int pitch);

/// Rewind sample to beginning
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_rewind(SASL sasl, int sampleId);

/// Returns non-zero if sample is playing now or zero if not
/// \param sasl SASL handler.
/// \param sampleId sample handler
int sasl_sample_is_playing(SASL sasl, int sampleId);

/// Set gain of all samples
/// \param sasl SASL handler.
/// \param gain gain ratio from 0 to 1000
void sasl_set_master_gain(SASL sasl, int gain);

/// Set sample location, outside, inside or both
/// \param sasl SASL handler.
/// \param sampleId sample handler
/// \param scene equals to SOUND_INTERNAL if inside, SOUND_EXTERNAL if outside or SOUND_EVERYWHERE if both
void sasl_sample_set_env(SASL sasl, int sampleId, int scene);

/// Get sample location, outside, inside or both
/// Returns SOUND_INTERNAL if inside, SOUND_EXTERNAL if outside or
/// SOUND_EVERYWHERE if both
/// \param sasl SASL handler.
/// \param sampleId sample handler
int sasl_sample_get_env(SASL sasl, int sampleId);

/// Set sample position
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_set_position(SASL sasl, int sampleId, 
        float x, float y, float z);

/// Get sample position
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_get_position(SASL sasl, int sampleId, 
        float *x, float *y, float *z);

/// Set sample direction
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_set_direction(SASL sasl, int sampleId, 
        float x, float y, float z);

/// Get sample position
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_get_direction(SASL sasl, int sampleId, 
        float *x, float *y, float *z);

/// Set maximum sample position
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_set_max_distance(SASL sasl, int sampleId, float max_distance);

/// Set rolloff factor
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_set_rolloff(SASL sasl, int sampleId, float roloff);

/// Set reference distance
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_set_ref_distance(SASL sasl, int sampleId, float distance);

/// Set sound cone parameters
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_set_cone(SASL sasl, int sampleId, float outer_gain, 
        float inner_angle, float outer_angle);

/// Get sound cone parameters
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_get_cone(SASL sasl, int sampleId, float *outer_gain, 
        float *inner_angle, float *outer_angle);

/// Set sample relative listener flag
/// \param sasl SASL handler.
/// \param sampleId sample handler
void sasl_sample_set_relative(SASL sasl, int sampleId, int relative);

/// Get sample relative listener flag
/// \param sasl SASL handler.
/// \param sampleId sample handler
int sasl_sample_get_relative(SASL sasl, int sampleId);

/// Set listener location, outside or inside
/// \param sasl SASL handler.
/// \param scene equals to SOUND_INTERNAL if inside or SOUND_EXTERNAL if outside
void sasl_listener_set_env(SASL sasl, int scene);

/// Set listener position
/// \param sasl SASL handler.
void sasl_listener_set_position(SASL sasl, float x, float y, float z);

/// Get listener position
/// \param sasl SASL handler.
void sasl_listener_get_position(SASL sasl, float *x, float *y, float *z);

/// Set listener orientation
/// \param sasl SASL handler.
void sasl_listener_set_orientation(SASL sasl, float x, float y, float z, 
        float ux, float uy, float uz);

/// Get listener orientation
/// \param sasl SASL handler.
void sasl_listener_get_orientation(SASL sasl, float *x, float *y, float *z, 
        float *ux, float *uy, float *uz);



// Logger API


/// Set callback for logging
/// \param sasl SASL handler.
/// \param callback log function
void sasl_set_log_callback(SASL sasl, sasl_log_callback callback, void *ref);

/// Returns current log callback
/// \param sasl SASL handler.
sasl_log_callback sasl_get_log_callback(SASL sasl, void **ref);

/// Log debug message
/// \param sasl SASL handler.
/// \param message message as printf format string
void sasl_log_debug(SASL sasl, const char *message, ...);

/// Log info message
/// \param sasl SASL handler.
/// \param message message as printf format string
void sasl_log_info(SASL sasl, const char *message, ...);

/// Log warning message
/// \param sasl SASL handler.
/// \param message message as printf format string
void sasl_log_warning(SASL sasl, const char *message, ...);

/// Log error message
/// \param sasl SASL handler.
/// \param message message as printf format string
void sasl_log_error(SASL sasl, const char *message, ...);



// Graphics API


/// add texture to SASL texture manager
/// Returns pointer to internal texture may be used as light user data in Lua
/// \param sasl SASL handler.
/// \param id texture ID as defined by low level graphics layer
void* sasl_import_texture(SASL sasl, int id);



#if defined(__cplusplus)
}  /* extern "C" */
#endif


#endif

