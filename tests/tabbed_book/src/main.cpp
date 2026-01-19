#include <sstream>

#include <utki/debug.hpp>
#include <ruisapp/application.hpp>

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/button/tab.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/group/tabbed_book.hpp>

#include "sample_page.hpp"

using namespace std::string_literals;

namespace m{
using namespace ruis::make;
}

utki::shared_ref<ruis::tab> inflate_tab(ruis::tabbed_book& tb, const std::string& name){
	auto& c = tb.context;

	// clang-format off
	auto t = m::tab(c,
		{},
		{
			m::row(c,
				{},
				{
					m::text(c,
						{
							.widget_params{
								.id = "text"s
							}
						}
					),
					m::push_button(c,
						{
							.widget_params{
								.id = "activate_button"s
							}
						},
						{
							m::text(c, {}, U"A"s)
						}
					),
					m::push_button(c,
						{
							.widget_params{
								.id = "close_button"s
							}
						},
						{
							m::image(c,
								{
									.layout_params{
										.dims{
											ruis::length::make_pp(8),
											ruis::length::make_pp(8)
										}
									},
									.image_params{
										.img = c.get().loader().load<ruis::res::image>("ruis_img_close")
									}
								}
							)
						}
					)
				}
			)
		}
	);
	// clang-format on

	t.get().get_widget_as<ruis::text>("text").set_text(name);

	auto& close_btn = t.get().get_widget_as<ruis::push_button>("close_button");
	
	close_btn.click_handler = [
			tabbed_book_wp = utki::make_weak_from(tb),
			tab_wp = utki::make_weak(t)
		](ruis::push_button& btn)
	{
		auto tb = tabbed_book_wp.lock();
		ASSERT(tb)

		auto t = tab_wp.lock();
		ASSERT(t)

		btn.context.get().post_to_ui_thread([tb, t]{
			tb->tear_out(*t);
		});
	};
	return t;
}

namespace{
utki::shared_ref<ruis::widget> make_root_widget(utki::shared_ref<ruis::context> c){
	// clang-format off
	return m::column(c,
		{},
		{
			m::row(c,
				{},
				{
					m::push_button(c,
						{
							.widget_params{
								.id = "add_button"s
							}
						},
						{
							m::text(c,
								{},
								U"add"s
							)
						}
					)
				}
			),
			m::tabbed_book(c,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::max},
						.weight = 1
					},
					.widget_params{
						.id = "book"s
					}
				},
				{
					{
						m::tab(c,
							{},
							{
								m::text(c, {}, U"tab1"s)
							}
						),
						utki::make_shared<sample_page>(c, "initial page 1"s)
					},
					{
						m::tab(c,
							{},
							{
								m::text(c, {}, U"tab2"s)
							}
						),
						utki::make_shared<sample_page>(c, "initial page 2"s)
					}
				}
			)
		}
	);
	// clang-format on
}
}

class application : public ruisapp::application{
	ruisapp::window& window;
public:
	application() :
			ruisapp::application({
						.name = "ruis-tests"
					}
				),
				window(this->make_window({
						.dims = {640, 480}
					}))
	{
		this->window.gui.context.get().window().close_handler = [this](){
			this->quit();
		};

		this->window.gui.init_standard_widgets(this->get_res_file("../../res/ruis_res/"));

		auto c = make_root_widget(this->window.gui.context);
		this->window.gui.set_root(c);

		auto& book = c.get().get_widget_as<ruis::tabbed_book>("book");

		auto& add_btn = c.get().get_widget_as<ruis::push_button>("add_button");
		add_btn.click_handler = [bk = utki::make_shared_from(book), cnt = 0](ruis::push_button& b) mutable {
			std::stringstream ss;
			ss << "page #" << cnt;
			auto txt = ss.str();
			++cnt;
			auto pg = utki::make_shared<sample_page>(b.context, txt);
			auto tb = inflate_tab(bk.get(), txt);

			tb.get().get_widget_as<ruis::push_button>("activate_button").click_handler = [pgw = utki::make_weak(pg)](ruis::push_button&){
				if(auto pg = pgw.lock()){
					pg->activate();
				}
			};

			bk.get().add(tb, pg);
		};
	}
};

const ruisapp::application_factory app_fac([](auto executable, auto args){
	return std::make_unique<::application>();
});
