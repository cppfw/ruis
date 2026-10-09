#include <r4/vector.hpp>
#include <ruis/layout/measure_mode.hpp>
#include <ruis/util/length.hpp>
#include <ruis/widget/base/list_widget.hpp>
#include <ruis/widget/group/touch/context_menu.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/padding.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>
#include <utki/unique_ref.hpp>

#include "../../harness/util/dummy_context.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace {
// A ruis::list_provider which provides two items of fixed natural widths (100 px and
// 80 px respectively, plus the padding borders). Used to test the context menu sizing.
class two_item_provider : public ruis::list_provider
{
public:
	explicit two_item_provider(const utki::shared_ref<ruis::context>& context) :
		ruis::list_provider(context)
	{}

	size_t count() const noexcept override
	{
		return 2;
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) const override
	{
		// clang-format off
		return ruis::make::padding(this->context,
			{
				.layout_params{
					.dims = {ruis::dim::max, ruis::dim::min}
				},
				.params{
					.container{
						.layout = ruis::layout::pile
					},
					.specific{
						.borders = {
							ruis::length::make_pp(12), // left
							ruis::length::make_pp(6), // top
							ruis::length::make_pp(12), // right
							ruis::length::make_pp(6) // bottom
						}
					}
				}
			},
			{
				ruis::make::gap(this->context,
					{
						.layout_params{
							.dims = {
								ruis::dim(ruis::length::make_px(index == 0 ? 100 : 80)),
								ruis::dim(ruis::length::make_px(30))
							}
						}
					})
			}
		);
		// clang-format on
	}
};

// Emulates the sizing done by ruis::overlay::show_popup(): the popup is measured
// against the overlay size (which is exactly known) and resized to the measured size.
ruis::vec2 measure_and_resize(
	ruis::widget& w, //
	const ruis::vec2& parent_dims
)
{
	auto dim = w.measure_within_parent(
		parent_dims, //
		r4::vector2<ruis::measure_mode>(ruis::measure_mode::exactly)
	);
	w.resize(dim);
	return dim;
}
} // namespace

namespace {
// NOLINTNEXTLINE(cppcoreguidelines-interfaces-global-init)
const tst::set set("context_menu", [](tst::suite& suite) {
	suite.add("menu_shown_with_concrete_width_should_fill_its_frame_and_list_to_that_width", [] {
		auto context = make_dummy_context();

		// A menu shown with a concrete width (like the automatic food lookup drop down
		// of the calslog app, which matches the menu width to the food name text field
		// width): the frame and the list must fill the menu width.
		// clang-format off
		auto menu = ruis::touch::make::context_menu(context,
			{
				.layout_params{
					.dims = {ruis::dim(ruis::length::make_px(300)), ruis::dim::min}
				},
				.widget{},
				.params{
					.list{
						.provider = utki::make_unique<two_item_provider>(context)
					}
				}
			}
		);
		// clang-format on

		measure_and_resize(menu.get(), ruis::vec2(1000, 1000));

		tst::check_eq(menu.get().rect().d.x(), ruis::real(300), SL);

		auto& frame = menu.get().get_widget_as<ruis::rectangle>("ruis_contextmenu_frame"sv);
		tst::check_eq(frame.rect().d.x(), ruis::real(300), SL);

		auto& list = frame.get_container().children().front().get();
		const auto border = context.get().style().get_len_border().get().get(context.get());
		tst::check_eq(list.rect().d.x(), ruis::real(300) - 2 * border, SL);
	});

	suite.add("menu_shown_with_its_natural_size_should_wrap_its_content", [] {
		auto context = make_dummy_context();

		// A menu shown with its natural size (the default layout parameters): the menu
		// wraps its content and the frame and the list fill the menu width (the frame
		// must not overflow the menu and the list must not be narrower than the frame
		// content area).
		// clang-format off
		auto menu = ruis::touch::make::context_menu(context,
			{
				.layout_params{},
				.widget{},
				.params{
					.list{
						.provider = utki::make_unique<two_item_provider>(context)
					}
				}
			}
		);
		// clang-format on

		measure_and_resize(menu.get(), ruis::vec2(1000, 1000));

		// the natural menu width is the natural width of its content, well below 300 px
		tst::check(menu.get().rect().d.x() < ruis::real(300), SL);
		tst::check(menu.get().rect().d.x() > ruis::real(0), SL);

		auto& frame = menu.get().get_widget_as<ruis::rectangle>("ruis_contextmenu_frame"sv);
		tst::check_eq(frame.rect().d.x(), menu.get().rect().d.x(), SL);

		auto& list = frame.get_container().children().front().get();
		const auto border = context.get().style().get_len_border().get().get(context.get());
		tst::check_eq(list.rect().d.x(), menu.get().rect().d.x() - 2 * border, SL);
	});
});
} // namespace
