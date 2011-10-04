#include <stdio.h>
#include <string>

#ifdef WINDOWS
#include <Winsock2.h>
#endif

#ifdef APL
#include <SDLMain.h>
#else
#include <SDL.h>
#include <SDL_video.h>
#include <SDL_opengl.h>
#include <SDL_events.h>
#endif

#include "libavionics.h"
#include "ogl.h"
#include "cmdline.h"
#include "fps.h"


using namespace slava;


static void initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Can't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
}


static void updateScreenSettings(int width, int height)
{
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


static void initScreen(int width, int height, bool fullscreen)
{
    int flags = SDL_OPENGL;
#ifndef WINDOWS
    flags |= SDL_RESIZABLE;
#endif
    if (fullscreen)
        flags |= SDL_FULLSCREEN;

    SDL_Surface *screen = SDL_SetVideoMode(width, height, 32, flags);
    if (! screen) {
        fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
        exit(1);
    }

    updateScreenSettings(width, height);

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}


SASL createPanel(SaslGraphicsCallbacks* graphics, int width, int height, 
        const std::string &data, const std::string &panel, 
        const std::string &host, int port, const std::string &secret)
{
    SASL sasl = sasl_init(data.c_str());
    if (! sasl) {
        fprintf(stderr, "Unable to initialize avionics library\n");
        exit(1);
    }
        
    sasl_set_graphics_callbacks(sasl, graphics);
    
    sasl_set_panel_size(sasl, width, height);
    sasl_set_popup_size(sasl, width, height);
    sasl_enable_click_emulator(sasl, true);
    sasl_set_background_color(sasl, 1, 1, 1, 1);

    if (host.size())
        if (sasl_connect_to_server(sasl, host.c_str(), port, secret.c_str())) {
            fprintf(stderr, "Can't connect to server %s %i\n", host.c_str(), port);
            exit(1);
        }

    if (sasl_load_panel(sasl, panel.c_str())) {
        fprintf(stderr, "Can't load panel\n");
        exit(1);
    }

    return sasl;
}

int main(int argc, char *argv[])
{
#ifdef WINDOWS
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0);
    int err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    bool showClickable = false;

    CmdLine cmdLine(argc, argv);

    initSDL();

    int width = cmdLine.getScreenWidth();
    int height = cmdLine.getScreenHeight();

    initScreen(width, height, cmdLine.isFullscreen());
    std::string title = "SLAVA - " + cmdLine.getPanel();
    SDL_WM_SetCaption(title.c_str(), title.c_str());

    SaslGraphicsCallbacks* graphics = saslgl_init_graphics();

    SASL sasl = createPanel(graphics, width, height, cmdLine.getDataDir(), 
            cmdLine.getPanel(), cmdLine.getNetHost(), cmdLine.getNetPort(),
            cmdLine.getNetSecret());

    Fps fps;
    fps.setTargetFps(cmdLine.getTargetFps());

    bool done = false;
    while (! done) {
        if (sasl_update(sasl))
            break;

        glClear(GL_COLOR_BUFFER_BIT);
        if (sasl_draw_panel(sasl, STAGE_ALL))
            break;
        SDL_GL_SwapBuffers();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: 
                    done = true; 
                    break;

                case SDL_MOUSEBUTTONDOWN: 
                    sasl_mouse_button_down(sasl, event.button.x, 
                            height - event.button.y, event.button.button, 3);
                    break;

                case SDL_MOUSEBUTTONUP: 
                    sasl_mouse_button_up(sasl, event.button.x, 
                            height - event.button.y, event.button.button, 3);
                    break;

                case SDL_MOUSEMOTION: 
                    sasl_mouse_move(sasl, event.button.x, 
                            height - event.button.y, 3);
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_F7:
                            showClickable = ! showClickable;
                            sasl_set_show_clickable(sasl, showClickable);
                            break;

                        case SDLK_F8:
                            sasl_done(sasl);
                            sasl = createPanel(graphics, width, height, 
                                    cmdLine.getDataDir(), cmdLine.getPanel(), 
                                    cmdLine.getNetHost(), cmdLine.getNetPort(),
                                    cmdLine.getNetSecret());
                            showClickable = false;
                            break;
                        
                        case SDLK_ESCAPE:
                            done = true;
                            break;

                        default: break;
                    };
                    break;

#ifndef WINDOWS                
                case SDL_VIDEORESIZE: 
                    width = event.resize.w;
                    height = event.resize.h;
                    initScreen(width, height, cmdLine.isFullscreen());
                    sasl_set_panel_size(sasl, width, height);
                    sasl_set_popup_size(sasl, width, height);
                    break;
#endif
            }
        }

        fps.update();
    }

    saslgl_done_graphics(graphics);
    sasl_done(sasl);
    return 0;
}

