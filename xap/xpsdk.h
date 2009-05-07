#ifndef __XP_GL_H__
#define __XP_GL_H__


// That's is x-plane way to include OpenGL headers


/// Handle cross platform differences
#if IBM
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#elif LIN
#define TRUE 1
#define FALSE 0
#include <GL/gl.h>
#include <GL/glu.h>
#else
#define TRUE 1
#define FALSE 0
#if __GNUC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <gl.h>
#include <glu.h>
#endif
#endif

#include <XPLMCamera.h>
#include <XPLMDefs.h>
#include <XPLMDisplay.h>
#include <XPLMDataAccess.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>
#include <XPLMPlanes.h>
#include <XPLMPlugin.h>
#include <XPLMProcessing.h>
#include <XPLMMenus.h>
#include <XPWidgets.h>
#include <XPStandardWidgets.h>



#endif

