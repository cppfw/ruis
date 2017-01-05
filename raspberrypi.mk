ifeq ($(raspberrypi),)
    raspberrypi := $(shell test -d /opt/vc && echo true)
    ifeq ($(raspberrypi),)
        raspberrypi := false
    endif
endif
