include prorab.mk

$(eval $(prorab-build-subdirs))

install::
#install pkg-config files
	@install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	@install pkg-config/*.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig

this_soname := $(shell cat $(prorab_this_dir)src/soname.txt)


#make debian packages
.PHONY: deb
deb: $(prorab_this_dir)debian/control
deb: $(patsubst %.install.in, %$(this_soname).install, $(shell ls $(prorab_this_dir)debian/*.install.in))
	$(prorab_echo)(cd $(prorab_this_dir); dpkg-buildpackage)

$(prorab_this_dir)debian/control: $(prorab_this_dir)src/soname.txt $(prorab_this_dir)debian/control.in
	$(prorab_echo)sed -e "s/\$$(soname)/$(shell cat $<)/" $(filter %debian/control.in, $^) >> $@

%$(this_soname).install: %.install.in
	$(prorab_echo)cp $< $@