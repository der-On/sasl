#ifndef __TEX_LOADER_H__
#define __TEX_LOADER_H__

#include <stdlib.h>


namespace xa {

/// Load texture from file.  Returns negative value on error or texture ID on success
/// \param name file name
/// \param texId OpenGL texture ID or 0 to enable automaticle generator
/// \param width place to store texture width
/// \param height place to store texture height
unsigned int loadTexture(const char *name, int texId, 
        int *width=NULL, int *heght=NULL);


};


#endif

