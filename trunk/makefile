include prorab.mk

$(eval $(prorab-build-subdirs))

install::
#install pkg-config files
	@install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	@install pkg-config/*.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig

#make debian packages
.PHONY: deb
deb: $(prorab_this_dir)debian/control
	@(cd $(prorab_this_dir); dpkg-buildpackage)
	
$(prorab_this_dir)debian/control: $(prorab_this_dir)src/soname.txt $(prorab_this_dir)debian/control.in
	@sed -e "s/\$$(soname)/$(shell cat $<)/" $(prorab_this_dir)debian/control.in >> $@
