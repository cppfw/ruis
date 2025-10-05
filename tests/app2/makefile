include prorab.mk
include prorab-test.mk
include prorab-clang-format.mk
include prorab-license.mk

this__is_interactive := true

include $(d)../common.mk

$(eval $(prorab-clang-format))

this_license_file := ../../LICENSE
$(eval $(prorab-license))
