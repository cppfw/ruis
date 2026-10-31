#include <ruis/gui.hpp>
#include <ruis/layout/pile_layout.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>
#include <utki/config.hpp>

#include "../../harness/util/dummy_context.hpp"

// Tests the ruis::make:: factory functions (widget construction through the make API):
// make::container, make::gap and make::rectangle, checking that widgets can be created
// and assembled with the factory helpers.
// TODO: is it needed to test that? What is needed to be tested is that general make factories createe a certain implementation of widget, e.g. that make::push_button() constructs rectangle_push_button.
namespace {
// NOLINTNEXTLINE(cppcoreguidelines-interfaces-global-init)
const tst::set set("make", [](tst::suite& suite) {
	suite.add("make_empty_container", []() {
		ruis::gui gui(make_dummy_context());

		auto c = ruis::make::container(gui.context, {}, {});
	});

	suite.add("basic_make", []() {
		ruis::gui gui(make_dummy_context());

		auto c = ruis::make::container(
			gui.context,
			{.layout_params = {.dims = {ruis::dim::min, ruis::dim::fill}}, .params = {.layout = ruis::layout::pile}},
			{ruis::make::gap(gui.context, {.widget = {.rectangle = {10, 20, 30, 40}}}),
			 ruis::make::gap(gui.context, {}),
			 ruis::make::rectangle(gui.context, {.params{.specific{.fill_color = 0xff0000ff}}})}
		);
	}); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
});
} // namespace
