include prorab.mk

this_soname := $(shell cat $(d)../src/soname.txt)

# install default resource pack
define this_rules
install::
$(.RECIPEPREFIX)$(a)for i in $(patsubst $(d)morda_res/%,/%,$(shell find $(d)morda_res -type f -name "*")); do \
		install -d $(DESTDIR)$(PREFIX)/share/morda/res$(this_soname)$$$${i%/*}; \
		install -m 644 $(d)morda_res$$$$i $(DESTDIR)$(PREFIX)/share/morda/res$(this_soname)$$$$i; \
	done
endef
$(eval $(this_rules))


