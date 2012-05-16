include conf.mk

SUBDIRS=libavionics libaccgl alsound

ifeq ($(BUILD_XAP),yes)
SUBDIRS+=xap
endif

ifeq ($(BUILD_SLAVA),yes)
SUBDIRS+=slava
endif


all:
	for d in $(SUBDIRS) ; do ( cd $$d ; $(MAKE) ) ; done

clean:
	for d in $(SUBDIRS) ; do ( cd $$d ; $(MAKE) clean ) ; done


