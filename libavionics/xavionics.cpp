#include "xavionics.h"

#include <stdexcept>
#include <assert.h>
#include <stdlib.h>
#include "avionics.h"


using namespace xa;


// Simple wrapper to make C happy
struct XAvionics {
    Avionics *avionics;
};

XA xa_init(const char *path)
{
    XA xa = (XA)malloc(sizeof(XAvionics));
    xa->avionics = new Avionics(path);
    return xa;
}

void xa_done(XA xa)
{
    assert(xa && xa->avionics);
    delete xa->avionics;
    free(xa);
}

void xa_set_panel_size(XA xa, int width, int height)
{
    assert(xa && xa->avionics);
    xa->avionics->setPanelResolution(width, height);
}

void xa_set_popup_size(XA xa, int width, int height)
{
    assert(xa && xa->avionics);
    xa->avionics->setPopupResolution(width, height);
}

int xa_load_panel(XA xa, const char *path)
{
    assert(xa && xa->avionics);
    try {
        xa->avionics->loadPanel(path);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
        return -1;
    } catch (...) {
        return -1;
    }
    return 0;
}

void xa_key_down(XA xa, int keycode)
{
    assert(xa && xa->avionics);
}

void xa_key_up(XA xa, int keycode)
{
    assert(xa && xa->avionics);
}

int xa_mouse_button_down(XA xa, int x, int y, int button, int layer)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->onMouseDown(x, y, button, layer);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
    } catch (...) {
        printf("Unknown error handling mouse button down event\n");
    }
    return 0;
}

int xa_mouse_button_up(XA xa, int x, int y, int button, int layer)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->onMouseUp(x, y, button, layer);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
    } catch (...) {
        printf("Unknown error handling mouse button up event\n");
    }
    return 0;
}

int xa_mouse_move(XA xa, int x, int y, int layer)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->onMouseMove(x, y, layer);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
    } catch (...) {
        printf("Unknown error handling mouse move event\n");
    }
    return 0;
}

int xa_mouse_button_click(XA xa, int x, int y, int button, int layer)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->onMouseClick(x, y, button, layer);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
    } catch (...) {
        printf("Unknown error handling mouse button click event\n");
    }
    return 0;
}

int xa_update(XA xa)
{
    assert(xa && xa->avionics);
    try {
        xa->avionics->update();
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
        return -1;
    } catch (...) {
        printf("Unknown error updating panel\n");
        return -1;
    }
    return 0;
}

int xa_draw_panel(XA xa, int stage)
{
    assert(xa && xa->avionics);
    try {
        xa->avionics->draw(stage);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
        return -1;
    } catch (...) {
        printf("Unknown error drawing panel\n");
        return -1;
    }
    return 0;
}

int xa_set_click_params(XA xa, int delay, int period)
{
    assert(xa && xa->avionics);
    try {
        xa->avionics->setClickParams(delay, period);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
        return -1;
    } catch (...) {
        printf("Unknown setting mouse click parameters\n");
        return -1;
    }
    return 0;
}

int xa_enable_click_emulator(XA xa, int enable)
{
    assert(xa && xa->avionics);
    try {
        xa->avionics->enableClickEmulator(enable);
    } catch (std::exception &e) {
        printf("ERROR: %s\n", e.what());
        return -1;
    } catch (...) {
        printf("Unknown enabling or disabling mouse click emulator\n");
        return -1;
    }
    return 0;
}

void xa_set_gen_tex_name_callback(XA xa, xa_gen_tex_name_callback generator)
{
    assert(xa && xa->avionics);
    xa->avionics->getTextureManager()->setTexNameGenerator(generator);
}

void xa_set_texture2d_binder_callback(XA xa, 
        xa_bind_texture_2d_callback binder)
{
    assert(xa && xa->avionics);
    xa->avionics->getTextureManager()->setBinder(binder);
}

struct lua_State* xa_get_lua(XA xa)
{
    assert(xa && xa->avionics);
    return xa->avionics->getLua();
}

int xa_set_props(XA xa, struct PropsCallbacks *callbacks, Props props)
{
    assert(xa && xa->avionics);
    xa->avionics->getProps().setProps(callbacks, props);
    return 0;
}

PropRef xa_get_prop_ref(XA xa, const char *name, int type)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().getProp(name, type);
    } catch (...) {
        return NULL;
    }
}

PropRef xa_create_prop(XA xa, const char *name, int type)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().createProp(name, type);
    } catch (...) {
        return NULL;
    }
}

void xa_free_prop_ref(XA xa, PropRef ref)
{
    assert(xa && xa->avionics);
    try {
        xa->avionics->getProps().freeProp(ref);
    } catch (...) { }
}

int xa_get_prop_int(XA xa, PropRef ref, int *err)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().getPropi(ref, 0, err);
    } catch (...) {
        return 0;
    }
}

int xa_set_prop_int(XA xa, PropRef ref, int value)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().setProp(ref, value);
    } catch (...) {
        return -1;
    }
}

float xa_get_prop_float(XA xa, PropRef ref, int *err)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().getPropf(ref, 0.0, err);
    } catch (...) {
        return 0.0;
    }
}

int xa_set_prop_float(XA xa, PropRef ref, float value)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().setProp(ref, value);
    } catch (...) {
        return -1;
    }
}

double xa_get_prop_double(XA xa, PropRef ref, int *err)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().getPropd(ref, 0.0, err);
    } catch (...) {
        return 0.0;
    }
}

int xa_set_prop_double(XA xa, PropRef ref, double value)
{
    assert(xa && xa->avionics);
    try {
        return xa->avionics->getProps().setProp(ref, value);
    } catch (...) {
        return -1;
    }
}


int xa_set_background_color(XA xa, float r, float g, float b, float a)
{
    assert(xa && xa->avionics);
    xa->avionics->setBackgroundColor(r, g, b, a);
    return 0;
}

int xa_set_show_clickable(XA xa, int show)
{
    assert(xa && xa->avionics);
    xa->avionics->setShowClickable(show);
    return 0;
}


int xa_start_netprop_server(XA xa, int port, const char *secret)
{
    assert(xa && xa->avionics);
    return xa->avionics->startPropsServer(port, secret);
}


void xa_stop_netprop_server(XA xa)
{
    assert(xa && xa->avionics);
    xa->avionics->stopPropsServer();
}


