include prorab.mk

this_soname := $(shell cat $(prorab_this_dir)../src/soname.txt)

#install default resource pack
define this_rules
install::
	$(prorab_echo)for i in $(patsubst $(prorab_this_dir)morda_res/%,/%,$(shell find $(prorab_this_dir)morda_res -type f -name "*")); do \
		install -d $(DESTDIR)$(PREFIX)/share/morda/res$(this_soname)$$$${i%/*}; \
		install -m 644 $(prorab_this_dir)morda_res$$$$i $(DESTDIR)$(PREFIX)/share/morda/res$(this_soname)$$$$i; \
	done
endef
$(eval $(this_rules))


