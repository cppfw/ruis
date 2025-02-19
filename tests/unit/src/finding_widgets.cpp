#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/gui.hpp>
#include <ruis/widget/group/scroll_area.hpp>
#include <ruis/widget/label/gap.hpp>

#include "../../harness/util/dummy_context.hpp"

using namespace std::string_literals;

namespace m{
using namespace ruis::make;
}

namespace{
const tst::set set("finding_widgets", [](tst::suite& suite){
    suite.add("get_all_widgets_function", []{
		ruis::gui gui(make_dummy_context());
		auto& c = gui.context;

		// clang-format off
		auto w = m::container(c,
			{},
			{
				m::scroll_area(c,
					{
						.widget_params{
							.id = "1"s
						}
					}
				),
				m::scroll_area(c,
					{
						.widget_params{
							.id = "2"s
						}
					},
					{
						m::scroll_area(c,
							{
								.widget_params{
									.id = "3"s
								}
							}
						),
						m::row(c,
							{
								.widget_params{
									.id = "4"s
								}
							},
							{
								m::pile(c,
									{
										.widget_params{
											.id = "8"s
										}
									},
									{
										m::scroll_area(c,
											{
												.widget_params{
													.id = "9"s
												}
											}
										)
									}
								)
							}
						)
					}
				),
				m::pile(c,
					{
						.widget_params{
							.id = "5"s
						}
					},
					{
						m::scroll_area(c,
							{
								.widget_params{
									.id = "6"s
								}
							},
							{
								m::row(c,
									{
										.widget_params{
											.id = "7"s
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

		std::vector<std::string> expected_ids = {{
			"1", "2", "3", "6", "9"
		}};

		auto aaas = w.get().get_all_widgets<ruis::scroll_area>();
		tst::check_ne(aaas.size(), size_t(0), SL);

		for(const auto& id : expected_ids){
            auto i = std::find_if(
					aaas.begin(),
					aaas.end(),
					[&id](const decltype(aaas)::value_type& wg) -> bool {
						return wg.get().id() == id;
					}
				);
            tst::check(i != aaas.end(), SL) << "id = '" << id <<"' not found";
		}
	});

	suite.add("chaining_get_widget", [](){
		ruis::gui gui(make_dummy_context());

		auto& c = gui.context;

		// clang-format off
		auto w = m::container(c,
			{
				.widget_params{
					.id = "root"s
				}
			},
			{
				m::container(c,
					{},
					{
						m::container(c,
							{
								.widget_params{
									.id = "child"s, // should not find this
									.rectangle = {{1, 1}, {10, 10}}
								}
							},
							{
								m::gap(c, {})
							}
						)
					}
				),
				m::container(c,
					{
						.widget_params{
							.id = "child"s // should find this
						}
					},
					{
						m::container(c,
							{
								.widget_params{
									.id = "child2"s,
									.rectangle = {{1, 2}, {10, 10}}
								}
							}
						)
					}
				),
				m::container(c,
					{
						.widget_params{
							.id = "child3"s
						}
					},
					{
						m::container(c,
							{},
							{
								m::container(c,
									{
										.widget_params{
											.id = "child1"s
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

		auto& found = w.get().get_widget("child").get_widget("child2");
		tst::check_eq(found.rect().p, ruis::vector2{1, 2}, SL);
	});
});
}
