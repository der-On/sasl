#ifndef __SASL_CALLBACKS_H__
#define __SASL_CALLBACKS_H__

#if defined(__cplusplus)
extern "C" {
#endif

    
// properties related callbacks

// Points to internal properties structures
typedef void* SaslProps;


/// Reference to pointer
typedef void* SaslPropRef;


/// Returns value of double property
typedef SaslPropRef (*sasl_get_prop_ref_callback)(SaslProps props, const char *name, 
        int type);

/// Destroy unneeded reference to pointer
typedef void (*sasl_free_prop_ref_callback)(SaslPropRef prop);

/// Returne value of property as integer
typedef int (*sasl_get_prop_int_callback)(SaslPropRef prop, int *err);

/// Sets value of property as integer
/// Returns zero on cuccess or non-zero on error
typedef int (*sasl_set_prop_int_callback)(SaslPropRef prop, int value);

/// Returne value of property as float
typedef float (*sasl_get_prop_float_callback)(SaslPropRef prop, int *err);

/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
typedef int (*sasl_set_prop_float_callback)(SaslPropRef prop, float value);

/// Returne value of property as float
typedef double (*sasl_get_prop_double_callback)(SaslPropRef prop, int *err);

/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
typedef int (*sasl_set_prop_double_callback)(SaslPropRef prop, double value);

/// Returne value of property as string
/// returns size of string
typedef int (*sasl_get_prop_string_callback)(SaslPropRef prop, 
        char *buf, int maxSize, int *err);

/// Sets value of property as string
/// Returns zero on cuccess or non-zero on error
typedef int (*sasl_set_prop_string_callback)(SaslPropRef prop, const char *value);



#define PROP_INT 1
#define PROP_FLOAT 2
#define PROP_DOUBLE 3
#define PROP_STRING 4


/// Create new propery and returns reference to it.
/// If property already exists just returns reference to it.
/// maxSize is maximum size for string properties, ignored for other types
typedef SaslPropRef (*sasl_create_prop_callback)(SaslProps props, const char *name, 
            int type, int maxSize);

/// Get value of functional property.
/// type - type of requested value
/// value - pointer to buffer have to be filled with actual data
/// maxSize - maximum size of buffer
/// returns size of data stored in property
typedef int (*sasl_prop_getter_callback)(int type, void *value, 
        int maxSize, void *ref);

/// Set value of functional property.
/// type - type of requested value
/// value - pointer to buffer filled with actual data
/// size - size of data
typedef void (*sasl_prop_setter_callback)(int type, void *value, 
        int size, void *ref);

/// Create new functional propery and returns reference to it.
/// If property already exists just returns reference to it.
typedef SaslPropRef (*sasl_create_func_prop_callback)(SaslProps props, const char *name, 
            int type, int maxSize, sasl_prop_getter_callback getter, 
            sasl_prop_setter_callback setter, void *ref);

/// Update properties.
typedef int (*sasl_update_props_callback)(SaslProps props);

/// Destroy properties.
typedef void (*sasl_props_done)(SaslProps props);

/// All callbacks for handy setup
struct SaslPropsCallbacks {
    sasl_get_prop_ref_callback get_prop_ref;
    sasl_free_prop_ref_callback free_prop_ref;
    sasl_create_prop_callback create_prop;
    sasl_create_func_prop_callback create_func_prop;
    sasl_get_prop_int_callback get_prop_int;
    sasl_set_prop_int_callback set_prop_int;
    sasl_get_prop_float_callback get_prop_float;
    sasl_set_prop_float_callback set_prop_float;
    sasl_get_prop_double_callback get_prop_double;
    sasl_set_prop_double_callback set_prop_double;
    sasl_get_prop_string_callback get_prop_string;
    sasl_set_prop_string_callback set_prop_string;
    sasl_update_props_callback update_props;
    sasl_props_done props_done;
};


//
// Commands API
//


/// abstract command type
typedef void* SaslCommand;

/// Command callback function
typedef int (*sasl_command_callback)(SaslCommand command,
        int phase, void *data); 

/// Returns command reference or NULL if not found
typedef SaslCommand (*sasl_find_command_callback)(const char *name, void *cmdDta);

/// Create new command or return reference to existing command
typedef SaslCommand (*sasl_create_command_callback)(const char *name, 
        const char *description, void *cmdDta);

/// Attach handler to comand
typedef void (*sasl_add_command_handler_callback)(SaslCommand command,
        sasl_command_callback handler, int before, void *data, void *cmdDta);

/// Remove command handler
typedef void (*sasl_remove_command_handler_callback)(SaslCommand command,
        sasl_command_callback handler, int before, void *data, void *cmdDta);

// start command
typedef void (*sasl_command_begin_callback)(SaslCommand command, void *cmdDta);

/// Finish command
typedef void (*sasl_command_end_callback)(SaslCommand command, void *cmdDta);

/// Start and finish command immediately
typedef void (*sasl_command_once_callback)(SaslCommand command, void *cmdDta);


/// commands callbacks API
struct SaslCommandCallbacks {
    sasl_find_command_callback find_command;
    sasl_create_command_callback create_command;
    sasl_add_command_handler_callback add_command_handler;
    sasl_remove_command_handler_callback remove_command_handler;
    sasl_command_begin_callback command_begin;
    sasl_command_end_callback command_end;
    sasl_command_once_callback command_once;
};



//
// graphics API
//

/// forward declaration of graphics structure
struct SaslGraphicsCallbacks;

/// initialize graphics before frame start
typedef void (*sasl_draw_begin)(struct SaslGraphicsCallbacks *canvas);

/// flush drawed graphics to screen
typedef void (*sasl_draw_end)(struct SaslGraphicsCallbacks *canvas);

/// load texture to memory.
/// Returns texture ID or -1 on failure.  On success returns texture width
//  and height in pixels
typedef int (*sasl_load_texture)(struct SaslGraphicsCallbacks *canvas,
        const char *buffer, int length, int *width, int *height);

// Unload texture from video memory.
typedef void (*sasl_free_texture)(struct SaslGraphicsCallbacks *canvas, 
        int textureId);

// draw line of specified color.
typedef void (*sasl_draw_line)(struct SaslGraphicsCallbacks *canvas, double x1,
        double y1, double x2, double y2, double r, double g, double b, double a);

// draw untextured triangle.
typedef void (*sasl_draw_triangle)(struct SaslGraphicsCallbacks *canvas, 
        double x1, double y1, double r1, double g1, double b1, double a1,
        double x2, double y2, double r2, double g2, double b2, double a2,
        double x3, double y3, double r3, double g3, double b3, double a3);

// draw textured triangle.
typedef void (*sasl_draw_textured_triangle)(struct SaslGraphicsCallbacks *canvas, 
        int textureId,
        double x1, double y1, double u1, double v1, double r1, double g1, double b1, double a1,
        double x2, double y2, double u2, double v2, double r2, double g2, double b2, double a2,
        double x3, double y3, double u3, double v3, double r3, double g3, double b3, double a3);

// enable clipping to rectangle
typedef void (*sasl_set_clip_area)(struct SaslGraphicsCallbacks *canvas, 
        double x1, double y1, double x2, double y2);

// disable clipping.
typedef void (*sasl_reset_clip_area)(struct SaslGraphicsCallbacks *canvas);

// push affine translation state
typedef void (*sasl_push_transform)(struct SaslGraphicsCallbacks *canvas);

// pop affine transform state
typedef void (*sasl_pop_transform)(struct SaslGraphicsCallbacks *canvas);

// apply move transform to current state
typedef void (*sasl_translate_transform)(struct SaslGraphicsCallbacks *canvas, 
        double x, double y);

// apply scale transform to current state
typedef void (*sasl_scale_transform)(struct SaslGraphicsCallbacks *canvas, 
        double x, double y);

// apply rotate transform to current state
typedef void (*sasl_rotate_transform)(struct SaslGraphicsCallbacks *canvas, 
        double angle);

// find sasl texture in memory by size and marker color
// pass NULL to exclude parameter from searching
// returns texture id or -1 if not found
typedef int (*sasl_find_texture)(struct SaslGraphicsCallbacks *canvas, 
        int width, int height, int *r, int *g, int *b, int *a);


// start rendering to texture
// pass -1 as texture ID to restore default render target
// return -1 on errors or zero on success
typedef int (*sasl_set_render_target)(struct SaslGraphicsCallbacks *canvas, 
        int textureId);

// create new texture of specified size and store it under the same name 
// as old texture
// use it for textures used as render target
typedef void (*sasl_recreate_texture)(struct SaslGraphicsCallbacks *canvas, 
        int textureId, int width, int height);


// grpahics callbacks
struct SaslGraphicsCallbacks {
    sasl_draw_begin draw_begin;
    sasl_draw_end draw_end;
    sasl_load_texture load_texture;
    sasl_free_texture free_texture;
    sasl_draw_line draw_line;
    sasl_draw_triangle draw_triangle;
    sasl_draw_textured_triangle draw_textured_triangle;
    sasl_set_clip_area set_clip_area;
    sasl_reset_clip_area reset_clip_area;
    sasl_push_transform push_transform;
    sasl_pop_transform pop_transform;
    sasl_translate_transform translate_transform;
    sasl_scale_transform scale_transform;
    sasl_rotate_transform rotate_transform;
    sasl_find_texture find_texture;
    sasl_set_render_target set_render_target;
    sasl_recreate_texture recreate_texture;
};


//
// sound API
//

/// forward declaration of graphics structure
struct SaslSoundCallbacks;


/// Load sample into memory.  Returns sample handler or 0 if can't load sample
/// \param sound sound callbacks structure.
/// \param fileName path to sample on disk
typedef int (*sasl_sample_load_callback)(struct SaslSoundCallbacks *sound, 
        const char *fileName);

/// Unload sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_unload_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId);

// Play loaded sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
/// \param loop if non-zero sound will be looped
typedef void (*sasl_sample_play_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, int loop);

/// Stop playing sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_stop_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId);

/// Set gain of sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
/// \param gain gain ratio from 0 to 1000
typedef void (*sasl_sample_set_gain_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, int gain);

/// Set pitch of sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
/// \param pitch pitch ratio from 0 to 1000
typedef void (*sasl_sample_set_pitch_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, int pitch);

/// Rewind sample to beginning
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_rewind_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId);

/// Set sample location, outside, inside or both
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
/// \param scene equals to 1 if inside, 2 if outside, 3 if both
typedef void (*sasl_sample_set_env_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, int scene);

/// Get sample location, outside, inside or both
/// Returns 1 if inside, 2 if outside, 3 if both
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef int (*sasl_sample_get_env_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId);

/// Set sample position
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_set_position_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float x, float y, float z);

/// Get sample position
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_get_position_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float *x, float *y, float *z);

/// Set sample direction
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_set_direction_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float x, float y, float z);

/// Get sample position
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_get_direction_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float *x, float *y, float *z);

/// Set maximum sample position
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_set_max_distance_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float max_distance);

/// Set rolloff factor
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_set_rolloff_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float roloff);

/// Set reference distance
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_set_ref_distance_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float distance);

/// Set sound cone parameters
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_set_cone_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float outer_gain, float inner_angle, float outer_angle);

/// Get sound cone parameters
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_get_cone_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, float *outer_gain, float *inner_angle, float *outer_angle);

/// Set sample relative listener flag
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*sasl_sample_set_relative_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId, int relative);

/// Get sample relative listener flag
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef int (*sasl_sample_get_relative_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId);

/// Set listener location, outside or inside
/// \param sound sound callbacks structure.
/// \param scene equals to 1 if inside or 2 if outside
typedef void (*sasl_listener_set_env_callback)(struct SaslSoundCallbacks *sound, 
        int scene);

/// Set listener position
/// \param sound sound callbacks structure.
typedef void (*sasl_listener_set_position_callback)(struct SaslSoundCallbacks *sound, 
        float x, float y, float z);

/// Get listener position
/// \param sound sound callbacks structure.
typedef void (*sasl_listener_get_position_callback)(struct SaslSoundCallbacks *sound, 
        float *x, float *y, float *z);

/// Set listener direction
/// \param sound sound callbacks structure.
typedef void (*sasl_listener_set_orientation_callback)(struct SaslSoundCallbacks *sound, 
        float x, float y, float z, float ux, float uy, float uz);

/// Get listener orientation
/// \param sound sound callbacks structure.
typedef void (*sasl_listener_get_orientation_callback)(struct SaslSoundCallbacks *sound, 
        float *x, float *y, float *z, 
        float *ux, float *uy, float *uz);

/// Returns non-zero if sample is playing now or zero if not
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef int (*sasl_sample_is_playing_callback)(struct SaslSoundCallbacks *sound, 
        int sampleId);

/// Set gain of all samples
/// \param sound sound callbacks structure.
/// \param gain gain ratio from 0 to 1000
typedef void (*sasl_set_master_gain_callback)(struct SaslSoundCallbacks *sound, 
        int gain);

/// update sound engine
/// \param sound sound callbacks structure.
typedef void (*sasl_sound_update_callback)(struct SaslSoundCallbacks *sound);

// sound callbacks
struct SaslSoundCallbacks {
    sasl_sample_load_callback load;
    sasl_sample_unload_callback unload;
    sasl_sample_play_callback play;
    sasl_sample_stop_callback stop;
    sasl_sample_set_gain_callback set_gain;
    sasl_sample_set_pitch_callback set_pitch;
    sasl_sample_rewind_callback rewind;
    sasl_sample_is_playing_callback is_playing;
    sasl_sample_set_env_callback set_env;
    sasl_sample_get_env_callback get_env;
    sasl_sample_set_position_callback set_position;
    sasl_sample_get_position_callback get_position;
    sasl_sample_set_direction_callback set_direction;
    sasl_sample_get_direction_callback get_direction;
    sasl_sample_set_max_distance_callback set_max_distance;
    sasl_sample_set_rolloff_callback set_rolloff;
    sasl_sample_set_ref_distance_callback set_ref_distance;
    sasl_sample_set_cone_callback set_cone;
    sasl_sample_get_cone_callback get_cone;
    sasl_sample_set_relative_callback set_relative;
    sasl_sample_get_relative_callback get_relative;
    sasl_listener_set_env_callback set_listener_env;
    sasl_listener_set_position_callback set_listener_position;
    sasl_listener_get_position_callback get_listener_position;
    sasl_listener_set_orientation_callback set_listener_orientation;
    sasl_listener_get_orientation_callback get_listener_orientation;
    sasl_set_master_gain_callback set_master_gain;
    sasl_sound_update_callback update;
};


/// Write information into log
/// \param level log message level, from LOG_DEBUG till LOG_ERROR
/// \param message text to log
typedef void (*sasl_log_callback)(int level, const char *message);

#if defined(__cplusplus)
}  /* extern "C" */
#endif


#endif

