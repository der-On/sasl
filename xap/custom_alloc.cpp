
#include "custom_alloc.h"
#include <cstdio>

void *lj_alloc_create(void)
{
	struct lua_alloc_request_t r = { 0 };
#ifndef NDEBUG
	printf ("in lj_alloc_create\n");
#endif
	XPLMSendMessageToPlugin(XPLM_PLUGIN_XPLANE, ALLOC_OPEN,&r);
	return r.ud;	
}

void  lj_alloc_destroy(void *msp)
{
	struct lua_alloc_request_t r = { 0 };
	r.ud = msp;
#ifndef NDEBUG
	printf ("in lj_alloc_destroy ud=msp=%p, ptr=%p, osize=%lu, nsize=%lu\n", msp, r.ptr, r.osize, r.nsize);
#endif
	XPLMSendMessageToPlugin(XPLM_PLUGIN_XPLANE, ALLOC_CLOSE,&r);
}

void *lj_alloc_f(void *msp, void *ptr, size_t osize, size_t nsize)
{
	struct lua_alloc_request_t r = { 0 };
	r.ud = msp;
	r.ptr = ptr;
	r.osize = osize;
	r.nsize = nsize;
#ifndef NDEBUG
	printf ("in lj_alloc_f ud=msp=%p, ptr=%p, osize=%lu, nsize=%lu\n", msp, ptr, osize, nsize);
#endif
	XPLMSendMessageToPlugin(XPLM_PLUGIN_XPLANE, ALLOC_REALLOC,&r);
	return r.ptr;
}
