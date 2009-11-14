#ifndef __X_CALLBACKS_H__
#define __X_CALLBACKS_H__

#ifndef __cplusplus
extern "C" {
#endif

    
/// Texture binder callback.
/// This is because of X-Plane use strange texture system
/// \param textureId texture ID
typedef void (*xa_bind_texture_2d_callback)(int textureId);


/// Generates ID for one texture
typedef int (*xa_gen_tex_name_callback)();


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


#define PROP_INT 1
#define PROP_FLOAT 2
#define PROP_DOUBLE 3


/// Create new propery and returns reference to it.
/// If property already exists just returns reference to it.
typedef PropRef (*xa_create_prop_callback)(Props props, const char *name, 
            int type);

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
            int type, xa_prop_getter_callback getter, 
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

/// Start command
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


#ifndef __cplusplus
extern "C" {
#endif


#endif

