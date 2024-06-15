include prorab.mk

this_soname := $(shell cat $(d)../src/soname.txt)

# install default resource pack
define this__rules
install::
$(.RECIPEPREFIX)$(a)for i in $(patsubst $(d)ruis_res/%,/%,$(shell find $(d)ruis_res -type f -name "*")); do \
		install -d $(DESTDIR)$(PREFIX)/share/ruis/res$(this_soname)$$$${i%/*}; \
		install -m 644 $(d)ruis_res$$$$i $(DESTDIR)$(PREFIX)/share/ruis/res$(this_soname)$$$$i; \
	done
endef
$(eval $(this__rules))


