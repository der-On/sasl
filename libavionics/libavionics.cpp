#include "libavionics.h"

#include <stdexcept>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include "avionics.h"
#include "propsclient.h"


using namespace xa;


// Simple wrapper to make C happy
struct Sasl {
    Avionics *avionics;
};

/*#define TRY \
    assert(sasl && sasl->avionics); \
    try {

#define CATCH(msg) \
    } catch (std::exception &e) { \
        sasl_log_error(sasl, e.what()); \
    } catch (...) {  \
        sasl_log_error(sasl, "Unknown error " msg); \
    }

#define CATCH_ERR(err, msg) \
    } catch (std::exception &e) { \
        sasl_log_warning(sasl, e.what()); \
        if (err) *err = -1; \
    } catch (...) {  \
        sasl_log_warning(sasl, "Unknown error " msg); \
        if (err) *err = -1; \
    }
*/

#define TRY \
    assert(sasl && sasl->avionics);
#define CATCH(msg)
#define CATCH_ERR(err, msg)

SASL sasl_init(const char *path)
{
//    try {
        SASL sasl = (SASL)malloc(sizeof(Sasl));
        sasl->avionics = new Avionics(path);
        if (sasl->avionics->initLua()) {
            delete sasl->avionics;
            delete sasl;
            return NULL;
        }
        return sasl;
/*    } catch (...) {
        return NULL;
    }*/
}

void sasl_done(SASL sasl)
{
    TRY
    delete sasl->avionics;
    CATCH("destroyig SASL")
    free(sasl);
}

void sasl_set_panel_size(SASL sasl, int width, int height)
{
    TRY
    sasl->avionics->setPanelResolution(width, height);
    CATCH("changing panel size")
}

void sasl_set_popup_size(SASL sasl, int width, int height)
{
    TRY
    sasl->avionics->setPopupResolution(width, height);
    CATCH("changing popup size")
}

int sasl_load_panel(SASL sasl, const char *path)
{
    TRY
        return sasl->avionics->loadPanel(path);
    CATCH("loading panel")
    return -1;
}

int sasl_key_down(SASL sasl, int charcode, int keycode)
{
    TRY
        return sasl->avionics->onKeyDown(charcode, keycode);
    CATCH("handling key down event")
    return 0;
}

int sasl_key_up(SASL sasl, int charcode, int keycode)
{
    TRY
        return sasl->avionics->onKeyUp(charcode, keycode);
    CATCH("handling key up event")
    return 0;
}

int sasl_mouse_button_down(SASL sasl, int x, int y, int button, int layer)
{
    TRY
        return sasl->avionics->onMouseDown(x, y, button, layer);
    CATCH("handling mouse button down event")
    return 0;
}

int sasl_mouse_button_up(SASL sasl, int x, int y, int button, int layer)
{
    TRY
        return sasl->avionics->onMouseUp(x, y, button, layer);
    CATCH("handling mouse button up event")
    return 0;
}

int sasl_mouse_move(SASL sasl, int x, int y, int layer)
{
    TRY
        return sasl->avionics->onMouseMove(x, y, layer);
    CATCH("handling mouse move event")
    return 0;
}

int sasl_mouse_button_click(SASL sasl, int x, int y, int button, int layer)
{
    TRY
        return sasl->avionics->onMouseClick(x, y, button, layer);
    CATCH("handling mouse click event")
    return 0;
}

int sasl_update(SASL sasl)
{
    TRY
        sasl->avionics->update();
        return 0;
    CATCH("updating avionics")
    return -1;
}

int sasl_draw_panel(SASL sasl, int stage)
{
    TRY
        sasl->avionics->draw(stage);
        return 0;
    CATCH("drawing panel")
    return -1;
}

int sasl_set_click_params(SASL sasl, int delay, int period)
{
    TRY
        sasl->avionics->setClickParams(delay, period);
        return 0;
    CATCH("setting mouse click parameters")
    return -1;
}

int sasl_enable_click_emulator(SASL sasl, int enable)
{
    TRY
        sasl->avionics->enableClickEmulator(enable);
        return 0;
    CATCH("enabling or disabling mouse click emulator")
    return -1;
}

void sasl_set_graphics_callbacks(SASL sasl, 
        struct SaslGraphicsCallbacks *callbacks)
{
    TRY
        sasl->avionics->setGraphicsCallbacks(callbacks);
    CATCH("setting graphics callbacks")
}


struct lua_State* sasl_get_lua(SASL sasl)
{
    TRY
        return sasl->avionics->getLua();
    CATCH("getting Lua state")
    return NULL;
}

int sasl_set_props(SASL sasl, struct SaslPropsCallbacks *callbacks, SaslProps props)
{
    TRY
        sasl->avionics->getProps().setProps(callbacks, props);
        return 0;
    CATCH("installing properties callbacks")
    return -1;
}

SaslPropRef sasl_get_prop_ref(SASL sasl, const char *name, int type)
{
    TRY
        return sasl->avionics->getProps().getProp(name, type);
    CATCH("finding property reference")
    return NULL;
}

SaslPropRef sasl_create_prop(SASL sasl, const char *name, int type)
{
    TRY
        return sasl->avionics->getProps().createProp(name, type);
    CATCH("creating property")
    return NULL;
}

void sasl_free_prop_ref(SASL sasl, SaslPropRef ref)
{
    TRY
        sasl->avionics->getProps().freeProp(ref);
    CATCH("freeing property")
}

int sasl_get_prop_int(SASL sasl, SaslPropRef ref, int *err)
{
    TRY
        return sasl->avionics->getProps().getPropi(ref, 0, err);
    CATCH_ERR(err, "getting int property value")
    return 0;
}

int sasl_set_prop_int(SASL sasl, SaslPropRef ref, int value)
{
    TRY
        return sasl->avionics->getProps().setProp(ref, value);
    CATCH("setting int property value")
    return -1;
}

float sasl_get_prop_float(SASL sasl, SaslPropRef ref, int *err)
{
    TRY
        return sasl->avionics->getProps().getPropf(ref, 0.0, err);
    CATCH_ERR(err, "getting float property value")
    return 0.0;
}

int sasl_set_prop_float(SASL sasl, SaslPropRef ref, float value)
{
    TRY
        return sasl->avionics->getProps().setProp(ref, value);
    CATCH("setting float property value")
    return -1;
}

double sasl_get_prop_double(SASL sasl, SaslPropRef ref, int *err)
{
    TRY
        return sasl->avionics->getProps().getPropd(ref, 0.0, err);
    CATCH_ERR(err, "getting double property value")
    return 0.0;
}

int sasl_set_prop_double(SASL sasl, SaslPropRef ref, double value)
{
    TRY
        return sasl->avionics->getProps().setProp(ref, value);
    CATCH("setting double property value")
    return -1;
}


int sasl_set_background_color(SASL sasl, float r, float g, float b, float a)
{
    TRY
        sasl->avionics->setBackgroundColor(r, g, b, a);
        return 0;
    CATCH("setting background color")
    return -1;
}

int sasl_set_show_clickable(SASL sasl, int show)
{
    TRY
        sasl->avionics->setShowClickable(show);
        return 0;
    CATCH("setting show clickable")
    return -1;
}


int sasl_start_netprop_server(SASL sasl, int port, const char *secret)
{
    TRY
        return sasl->avionics->startPropsServer(port, secret);
    CATCH("starting network server")
    return -1;
}


void sasl_stop_netprop_server(SASL sasl)
{
    TRY
        sasl->avionics->stopPropsServer();
    CATCH("stoping network server")
}


void sasl_set_commands(SASL sasl, struct SaslCommandCallbacks *callbacks, void *data)
{
    TRY
        sasl->avionics->setCommandsCallbacks(callbacks, data);
    CATCH("setting commands callbacks")
}

int sasl_connect_to_server(SASL sasl, const char *host, int port, 
        const char *secret)
{
    TRY
        return connectToServer(sasl, sasl->avionics->getLog(), host, port, secret);
    CATCH("connecting to remote properties server")
    return -1;
}



void sasl_set_sound_engine(SASL sasl, struct SaslSoundCallbacks *callbacks)
{
    TRY
        sasl->avionics->getSound().setCallbacks(callbacks);
    CATCH("setting sound callbacks")
}


int sasl_sample_load(SASL sasl, const char *fileName)
{
    TRY
        return sasl->avionics->getSound().loadSample(fileName);
    CATCH("loading sound")
    return 0;
}


void sasl_sample_unload(SASL sasl, int sampleId)
{
    TRY
        return sasl->avionics->getSound().unloadSample(sampleId);
    CATCH("unloading sound")
}


void sasl_sample_play(SASL sasl, int sampleId, int loop)
{
    TRY
        sasl->avionics->getSound().playSample(sampleId, loop);
    CATCH("playing sound")
}


void sasl_sample_stop(SASL sasl, int sampleId)
{
    TRY
        sasl->avionics->getSound().stopSample(sampleId);
    CATCH("stoping sound")
}

void sasl_sample_set_gain(SASL sasl, int sampleId, int gain)
{
    TRY
        sasl->avionics->getSound().setSampleGain(sampleId, gain);
    CATCH("setting sound gain")
}


void sasl_sample_set_pitch(SASL sasl, int sampleId, int pitch)
{
    TRY
        sasl->avionics->getSound().setSamplePitch(sampleId, pitch);
    CATCH("setting sound pitch")
}


void sasl_sample_rewind(SASL sasl, int sampleId)
{
    TRY
        sasl->avionics->getSound().rewindSample(sampleId);
    CATCH("rewinding sound")
}

int sasl_sample_is_playing(SASL sasl, int sampleId)
{
    TRY
        return sasl->avionics->getSound().isSamplePlaying(sampleId);
    CATCH("testing sound is playing")
    return 0;
}

void sasl_set_master_gain(SASL sasl, int gain)
{
    TRY
        sasl->avionics->getSound().setMasterGain(gain);
    CATCH("setting master sound gain")
}

void sasl_sample_set_env(SASL sasl, int sampleId, int scene)
{
    TRY
        sasl->avionics->getSound().setSampleEnv(sampleId, scene);
    CATCH("setting sound envinronment")
}

int sasl_sample_get_env(SASL sasl, int sampleId)
{
    TRY
        return sasl->avionics->getSound().getSampleEnv(sampleId);
    CATCH("testing sound environment")
    return 0;
}


void sasl_sample_set_position(SASL sasl, int sampleId, 
        float x, float y, float z)
{
    TRY
        sasl->avionics->getSound().setSamplePosition(sampleId, x, y, z);
    CATCH("setting sound position")
}

void sasl_sample_get_position(SASL sasl, int sampleId, 
        float *x, float *y, float *z)
{
    TRY
        sasl->avionics->getSound().getSamplePosition(sampleId, *x, *y, *z);
    CATCH("getting sound position")
}


void sasl_sample_set_direction(SASL sasl, int sampleId, 
        float x, float y, float z)
{
    TRY
        sasl->avionics->getSound().setSampleDirection(sampleId, x, y, z);
    CATCH("setting sound direction")
}


void sasl_sample_get_direction(SASL sasl, int sampleId, 
        float *x, float *y, float *z)
{
    TRY
        float dx, dy, dz;
        sasl->avionics->getSound().getSampleDirection(sampleId, dx, dy, dz);
        if (x) *x = dx;
        if (y) *y = dy;
        if (z) *z = dz;
    CATCH("getting sound direction")
}


void sasl_sample_set_max_distance(SASL sasl, int sampleId, float max_distance)
{
    TRY
        sasl->avionics->getSound().setSampleMaxDistance(sampleId, max_distance);
    CATCH("setting sound max distance")
}


void sasl_sample_set_rolloff(SASL sasl, int sampleId, float rolloff)
{
    TRY
        sasl->avionics->getSound().setSampleRolloff(sampleId, rolloff);
    CATCH("setting sound rolloff factor")
}


void sasl_sample_set_ref_distance(SASL sasl, int sampleId, float distance)
{
    TRY
        sasl->avionics->getSound().setSampleRefDistance(sampleId, distance);
    CATCH("setting sound reference distance")
}


void sasl_sample_set_cone(SASL sasl, int sampleId, float outer_gain, 
        float inner_angle, float outer_angle)
{
    TRY
        sasl->avionics->getSound().setSampleCone(sampleId, outer_gain, 
                inner_angle, outer_angle);
    CATCH("setting sound cone properties")
}


void sasl_sample_get_cone(SASL sasl, int sampleId, float *outer_gain, 
        float *inner_angle, float *outer_angle)
{
    TRY
        sasl->avionics->getSound().getSampleCone(sampleId, *outer_gain, 
                *inner_angle, *outer_angle);
    CATCH("getting sound cone properties")
}


void sasl_sample_set_relative(SASL sasl, int sampleId, int relative)
{
    TRY
        sasl->avionics->getSound().setSampleRelative(sampleId, relative);
    CATCH("setting sound relative")
}


int sasl_sample_get_relative(SASL sasl, int sampleId)
{
    TRY
        return sasl->avionics->getSound().getSampleRelative(sampleId);
    CATCH("getting sound relative")
    return 0;
}

void sasl_listener_set_env(SASL sasl, int scene)
{
    TRY
        sasl->avionics->getSound().setListenerEnv(scene);
    CATCH("setting listener environment")
}


void sasl_listener_set_position(SASL sasl, float x, float y, float z)
{
    TRY
        sasl->avionics->getSound().setListenerPosition(x, y, z);
    CATCH("setting listener position")
}


void sasl_listener_get_position(SASL sasl, float *x, float *y, float *z)
{
    TRY
        sasl->avionics->getSound().getListenerPosition(*x, *y, *z);
    CATCH("getting listener position")
}


void sasl_listener_set_orientation(SASL sasl, float x, float y, float z, 
        float ux, float uy, float uz)
{
    TRY
        sasl->avionics->getSound().setListenerOrientation(x, y, z, ux, uy, uz);
    CATCH("setting listener orientation")
}


void sasl_listener_get_orientation(SASL sasl, float *x, float *y, float *z, 
        float *ux, float *uy, float *uz)
{
    TRY
        sasl->avionics->getSound().getListenerOrientation(*x, *y, *z, 
                *ux, *uy, *uz);
    CATCH("getting listener orientation")
}



void sasl_set_log_callback(SASL sasl, sasl_log_callback callback, void *ref)
{
    assert(sasl && sasl->avionics);
    sasl->avionics->getLog().setLogger(callback, ref);
}

sasl_log_callback sasl_get_log_callback(SASL sasl, void **ref)
{
    assert(sasl && sasl->avionics);
    return sasl->avionics->getLog().getLogger(ref);
}

void sasl_log_debug(SASL sasl, const char *message, ...)
{
    assert(sasl && sasl->avionics);
    va_list args;
    va_start(args, message);
    sasl->avionics->getLog().log(LOG_DEBUG, message, args);
    va_end(args);
}

void sasl_log_info(SASL sasl, const char *message, ...)
{
    assert(sasl && sasl->avionics);
    va_list args;
    va_start(args, message);
    sasl->avionics->getLog().log(LOG_INFO, message, args);
    va_end(args);
}

void sasl_log_warning(SASL sasl, const char *message, ...)
{
    assert(sasl && sasl->avionics);
    va_list args;
    va_start(args, message);
    sasl->avionics->getLog().log(LOG_WARNING, message, args);
    va_end(args);
}

void sasl_log_error(SASL sasl, const char *message, ...)
{
    assert(sasl && sasl->avionics);
    va_list args;
    va_start(args, message);
    sasl->avionics->getLog().log(LOG_ERROR, message, args);
    va_end(args);
}


