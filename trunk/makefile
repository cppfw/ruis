subdirs :=
subdirs += src
subdirs += tests

#build docs only from linux, because there is doxygen
ifeq ($(platform),win32)

else

subdirs += docs

endif


include ./targets.mk
include ./subdirs.mk
