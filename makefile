subdirs :=
subdirs += src
subdirs += tests

#build docs only from linux, because there is doxygen
ifeq ($(platform), windows)

else

subdirs += docs

endif


include ./targets.mk
include ./subdirs.mk





#use exactly this name for the var, so that dh_auto_install could set it if needed
DESTDIR :=
PREFIX := /usr

install:
#install header files
	@for i in $(patsubst src/%,%,$(shell find src/morda -type f -name *.hpp)); do \
	    install -D src/$$i $(DESTDIR)$(PREFIX)/include/$$i; \
	done
#install library files
	@install -d $(DESTDIR)$(PREFIX)/lib/
	@install src/libmorda.* $(DESTDIR)$(PREFIX)/lib/
#install docs
	@install -d $(DESTDIR)$(PREFIX)/share/doc/libmorda
	@install docs/doxygen/* $(DESTDIR)$(PREFIX)/share/doc/libmorda