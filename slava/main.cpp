#include <stdio.h>
#include <string>

#ifdef WINDOWS
#include <Winsock2.h>
#endif

#include <SDL.h>
#include <SDL_video.h>
#include <SDL_opengl.h>
#include <SDL_events.h>

#include "xavionics.h"
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


static void initScreen(int width, int height, bool fullscreen)
{
    int flags = SDL_OPENGL;
    if (fullscreen)
        flags |= SDL_FULLSCREEN;

    SDL_Surface *screen = SDL_SetVideoMode(width, height, 32, flags);
    if (! screen) {
        fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
        exit(1);
    }

    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}


XA createPanel(int width, int height, const std::string &data,
        const std::string &panel, const std::string &host, int port,
        const std::string &secret)
{
    XA xa = xa_init(data.c_str());
    if (! xa) {
        fprintf(stderr, "Unable to initialize avionics library\n");
        exit(1);
    }
    
    xa_set_panel_size(xa, width, height);
    xa_set_popup_size(xa, width, height);
    xa_enable_click_emulator(xa, true);
    xa_set_background_color(xa, 1, 1, 1, 1);

    if (host.size())
        if (xa_connect_to_server(xa, host.c_str(), port, secret.c_str())) {
            fprintf(stderr, "Can't connect to server %s %i\n", host.c_str(), port);
            exit(1);
        }

    if (xa_load_panel(xa, panel.c_str())) {
        fprintf(stderr, "Can't load panel\n");
        exit(1);
    }

    return xa;
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

    XA xa = createPanel(width, height, cmdLine.getDataDir(), 
            cmdLine.getPanel(), cmdLine.getNetHost(), cmdLine.getNetPort(),
            cmdLine.getNetSecret());

    Fps fps;
    fps.setTargetFps(cmdLine.getTargetFps());

    bool done = false;
    while (! done) {
        if (xa_update(xa))
            break;

        glClear(GL_COLOR_BUFFER_BIT);
        if (xa_draw_panel(xa, STAGE_ALL))
            break;
        SDL_GL_SwapBuffers();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: 
                    done = true; 
                    break;

                case SDL_MOUSEBUTTONDOWN: 
                    xa_mouse_button_down(xa, event.button.x, 
                            height - event.button.y, event.button.button, 3);
                    break;

                case SDL_MOUSEBUTTONUP: 
                    xa_mouse_button_up(xa, event.button.x, 
                            height - event.button.y, event.button.button, 3);
                    break;

                case SDL_MOUSEMOTION: 
                    xa_mouse_move(xa, event.button.x, 
                            height - event.button.y, 3);
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_F7:
                            showClickable = ! showClickable;
                            xa_set_show_clickable(xa, showClickable);
                            break;

                        case SDLK_F8:
                            xa_done(xa);
                            xa = createPanel(width, height, 
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
            }
        }

        fps.update();
    }

    xa_done(xa);
    return 0;
}

