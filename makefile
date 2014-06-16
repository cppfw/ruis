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





install_dst_dir := /usr

install:
#install header files
	@for i in $(patsubst src/%,%,$(shell find src/morda -type f -name *.hpp)); do \
	    install -D src/$$i $(install_dst_dir)/include/$$i; \
	done
#install library files
	@install -D src/libmorda.* $(install_dst_dir)/lib
	