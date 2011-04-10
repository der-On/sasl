#ifndef __X_CALLBACKS_H__
#define __X_CALLBACKS_H__

#if defined(__cplusplus)
extern "C" {
#endif

    
// properties related callbacks

// Points to internal properties structures
typedef void* Props;

/// Reference to pointer
typedef void* PropRef;


/// Returns value of double property
typedef PropRef (*xa_get_prop_ref_callback)(Props props, const char *name, 
        int type);

/// Destroy unneeded reference to pointer
typedef void (*xa_free_prop_ref_callback)(PropRef prop);

/// Returne value of property as integer
typedef int (*xa_get_prop_int_callback)(PropRef prop, int *err);

/// Sets value of property as integer
/// Returns zero on cuccess or non-zero on error
typedef int (*xa_set_prop_int_callback)(PropRef prop, int value);

/// Returne value of property as float
typedef float (*xa_get_prop_float_callback)(PropRef prop, int *err);

/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
typedef int (*xa_set_prop_float_callback)(PropRef prop, float value);

/// Returne value of property as float
typedef double (*xa_get_prop_double_callback)(PropRef prop, int *err);

/// Sets value of property as float
/// Returns zero on cuccess or non-zero on error
typedef int (*xa_set_prop_double_callback)(PropRef prop, double value);

/// Returne value of property as string
/// returns size of string
typedef int (*xa_get_prop_string_callback)(PropRef prop, 
        char *buf, int maxSize, int *err);

/// Sets value of property as string
/// Returns zero on cuccess or non-zero on error
typedef int (*xa_set_prop_string_callback)(PropRef prop, const char *value);



#define PROP_INT 1
#define PROP_FLOAT 2
#define PROP_DOUBLE 3
#define PROP_STRING 4


/// Create new propery and returns reference to it.
/// If property already exists just returns reference to it.
/// maxSize is maximum size for string properties, ignored for other types
typedef PropRef (*xa_create_prop_callback)(Props props, const char *name, 
            int type, int maxSize);

/// Get value of functional property.
/// type - type of requested value
/// value - pointer to buffer have to be filled with actual data
/// maxSize - maximum size of buffer
/// returns size of data stored in property
typedef int (*xa_prop_getter_callback)(int type, void *value, 
        int maxSize, void *ref);

/// Set value of functional property.
/// type - type of requested value
/// value - pointer to buffer filled with actual data
/// size - size of data
typedef void (*xa_prop_setter_callback)(int type, void *value, 
        int size, void *ref);

/// Create new functional propery and returns reference to it.
/// If property already exists just returns reference to it.
typedef PropRef (*xa_create_func_prop_callback)(Props props, const char *name, 
            int type, int maxSize, xa_prop_getter_callback getter, 
            xa_prop_setter_callback setter, void *ref);

/// Update properties.
typedef int (*xa_update_props_callback)(Props props);

/// Destroy properties.
typedef void (*xa_props_done)(Props props);

/// All callbacks for handy setup
struct PropsCallbacks {
    xa_get_prop_ref_callback get_prop_ref;
    xa_free_prop_ref_callback free_prop_ref;
    xa_create_prop_callback create_prop;
    xa_create_func_prop_callback create_func_prop;
    xa_get_prop_int_callback get_prop_int;
    xa_set_prop_int_callback set_prop_int;
    xa_get_prop_float_callback get_prop_float;
    xa_set_prop_float_callback set_prop_float;
    xa_get_prop_double_callback get_prop_double;
    xa_set_prop_double_callback set_prop_double;
    xa_get_prop_string_callback get_prop_string;
    xa_set_prop_string_callback set_prop_string;
    xa_update_props_callback update_props;
    xa_props_done props_done;
};


//
// Commands API
//


/// abstract command type
typedef void* XaCommand;

/// Command callback function
typedef int (*xa_command_callback)(XaCommand command,
        int phase, void *data); 

/// Returns command reference or NULL if not found
typedef XaCommand (*xa_find_command_callback)(const char *name, void *cmdDta);

/// Create new command or return reference to existing command
typedef XaCommand (*xa_create_command_callback)(const char *name, 
        const char *description, void *cmdDta);

/// Attach handler to comand
typedef void (*xa_add_command_handler_callback)(XaCommand command,
        xa_command_callback handler, int before, void *data, void *cmdDta);

/// Remove command handler
typedef void (*xa_remove_command_handler_callback)(XaCommand command,
        xa_command_callback handler, int before, void *data, void *cmdDta);

// start command
typedef void (*xa_command_begin_callback)(XaCommand command, void *cmdDta);

/// Finish command
typedef void (*xa_command_end_callback)(XaCommand command, void *cmdDta);

/// Start and finish command immediately
typedef void (*xa_command_once_callback)(XaCommand command, void *cmdDta);


/// commands callbacks API
struct XaCommandCallbacks {
    xa_find_command_callback find_command;
    xa_create_command_callback create_command;
    xa_add_command_handler_callback add_command_handler;
    xa_remove_command_handler_callback remove_command_handler;
    xa_command_begin_callback command_begin;
    xa_command_end_callback command_end;
    xa_command_once_callback command_once;
};



//
// graphics API
//

/// forward declaration of graphics structure
struct XaGraphicsCallbacks;

/// initialize graphics before frame start
typedef void (*xa_draw_begin)(struct XaGraphicsCallbacks *canvas);

/// flush drawed graphics to screen
typedef void (*xa_draw_end)(struct XaGraphicsCallbacks *canvas);

/// load texture to memory.
/// Returns texture ID or -1 on failure.  On success returns texture width
//  and height in pixels
typedef int (*xa_load_texture)(struct XaGraphicsCallbacks *canvas,
        const char *name, int *width, int *height);

// Unload texture from video memory.
typedef void (*xa_free_texture)(struct XaGraphicsCallbacks *canvas, 
        int textureId);

// draw line of specified color.
typedef void (*xa_draw_line)(struct XaGraphicsCallbacks *canvas, double x1,
        double y1, double x2, double y2, double r, double g, double b, double a);

// draw untextured triangle.
typedef void (*xa_draw_triangle)(struct XaGraphicsCallbacks *canvas, 
        double x1, double y1, double r1, double g1, double b1, double a1,
        double x2, double y2, double r2, double g2, double b2, double a2,
        double x3, double y3, double r3, double g3, double b3, double a3);

// draw textured triangle.
typedef void (*xa_draw_textured_triangle)(struct XaGraphicsCallbacks *canvas, 
        int textureId,
        double x1, double y1, double u1, double v1, double r1, double g1, double b1, double a1,
        double x2, double y2, double u2, double v2, double r2, double g2, double b2, double a2,
        double x3, double y3, double u3, double v3, double r3, double g3, double b3, double a3);

// enable clipping to rectangle
typedef void (*xa_set_clip_area)(struct XaGraphicsCallbacks *canvas, 
        double x1, double y1, double x2, double y2);

// disable clipping.
typedef void (*xa_reset_clip_area)(struct XaGraphicsCallbacks *canvas);

// push affine translation state
typedef void (*xa_push_transform)(struct XaGraphicsCallbacks *canvas);

// pop affine transform state
typedef void (*xa_pop_transform)(struct XaGraphicsCallbacks *canvas);

// apply move transform to current state
typedef void (*xa_translate_transform)(struct XaGraphicsCallbacks *canvas, 
        double x, double y);

// apply scale transform to current state
typedef void (*xa_scale_transform)(struct XaGraphicsCallbacks *canvas, 
        double x, double y);

// apply rotate transform to current state
typedef void (*xa_rotate_transform)(struct XaGraphicsCallbacks *canvas, 
        double angle);

// grpahics callbacks
struct XaGraphicsCallbacks {
    xa_draw_begin draw_begin;
    xa_draw_end draw_end;
    xa_load_texture load_texture;
    xa_free_texture free_texture;
    xa_draw_line draw_line;
    xa_draw_triangle draw_triangle;
    xa_draw_textured_triangle draw_textured_triangle;
    xa_set_clip_area set_clip_area;
    xa_reset_clip_area reset_clip_area;
    xa_push_transform push_transform;
    xa_pop_transform pop_transform;
    xa_translate_transform translate_transform;
    xa_scale_transform scale_transform;
    xa_rotate_transform rotate_transform;
};


//
// sound API
//

/// forward declaration of graphics structure
struct XaSoundCallbacks;


/// Load sample into memory.  Returns sample handler or 0 if can't load sample
/// \param sound sound callbacks structure.
/// \param fileName path to sample on disk
typedef int (*xa_sample_load_callback)(struct XaSoundCallbacks *sound, 
        const char *fileName);

// Play loaded sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
/// \param loop if non-zero sound will be looped
typedef void (*xa_sample_play_callback)(struct XaSoundCallbacks *sound, 
        int sampleId, int loop);

/// Stop playing sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*xa_sample_stop_callback)(struct XaSoundCallbacks *sound, 
        int sampleId);

/// Set gain of sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
/// \param gain gain ratio from 0 to 1000
typedef void (*xa_sample_set_gain_callback)(struct XaSoundCallbacks *sound, 
        int sampleId, int gain);

/// Set pitch of sample
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
/// \param pitch pitch ratio from 0 to 1000
typedef void (*xa_sample_set_pitch_callback)(struct XaSoundCallbacks *sound, 
        int sampleId, int pitch);

/// Rewind sample to beginning
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef void (*xa_sample_rewind_callback)(struct XaSoundCallbacks *sound, 
        int sampleId);

/// Returns non-zero if sample is playing now or zero if not
/// \param sound sound callbacks structure.
/// \param sampleId sample handler
typedef int (*xa_sample_is_playing_callback)(struct XaSoundCallbacks *sound, 
        int sampleId);

/// Set gain of all samples
/// \param sound sound callbacks structure.
/// \param gain gain ratio from 0 to 1000
typedef void (*xa_set_master_gain_callback)(struct XaSoundCallbacks *sound, 
        int gain);

// sound callbacks
struct XaSoundCallbacks {
    xa_sample_load_callback load;
    xa_sample_play_callback play;
    xa_sample_stop_callback stop;
    xa_sample_set_gain_callback set_gain;
    xa_sample_set_pitch_callback set_pitch;
    xa_sample_rewind_callback rewind;
    xa_sample_is_playing_callback is_playing;
    xa_set_master_gain_callback set_master_gain;
};


#if defined(__cplusplus)
}  /* extern "C" */
#endif


#endif

