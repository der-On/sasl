#ifndef __CUSTOM_ALLOC_H__
#define __CUSTOM_ALLOC_H__

#include <stddef.h>
#include "XPLMPlugin.h"


struct lua_alloc_request_t {
			void *	ud;
			void *	ptr;
			size_t	osize;
			size_t	nsize;
};

#define		ALLOC_OPEN		0x00A110C1
#define		ALLOC_REALLOC	0x00A110C2
#define		ALLOC_CLOSE		0x00A110C3

void *lj_alloc_create(void);

void  lj_alloc_destroy(void *msp);

void *lj_alloc_f(void *msp, void *ptr, size_t osize, size_t nsize);


#endif

