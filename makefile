include prorab.mk

$(eval $(prorab-build-subdirs))


$(prorab-clear-this-vars)


this_soname_dependency := $(prorab_this_dir)src/soname.txt
this_soname := $(shell cat $(this_soname_dependency))

$(eval $(prorab-build-deb))


define this_rules
install::
#install pkg-config files
	$(prorab_echo)install -d $(DESTDIR)$(PREFIX)/lib/pkgconfig
	$(prorab_echo)install pkg-config/*.pc $(DESTDIR)$(PREFIX)/lib/pkgconfig
#resource files
	$(prorab_echo)for i in $(patsubst $(prorab_this_dir)res/%,/%,$(shell find $(prorab_this_dir)res -type f -name "*")); do \
		install -d $(DESTDIR)$(PREFIX)/share/morda/res$(this_soname)$$$${i%/*}; \
		install $(prorab_this_dir)res$$$$i $(DESTDIR)$(PREFIX)/share/morda/res$(this_soname)$$$$i; \
	done
endef
$(eval $(this_rules))


#Update version rule
$(prorab-clear-this-vars)

this_version_files += pkg-config/morda.pc.in

$(eval $(prorab-apply-version))
