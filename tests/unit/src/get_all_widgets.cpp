#include <tst/set.hpp>
#include <tst/check.hpp>

#include <morda/gui.hpp>
#include <morda/widgets/group/column.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace{
tst::set set("get_all_widgets", [](tst::suite& suite){
    suite.add("get_all_widgets_function", []{
		morda::gui m(make_dummy_context());
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				@column{
					id{1}
				}

				@column{
					id{2}

					@column{
						id{3}
					}
					@row{
						id{4}

						@pile{
							id{8}

							@column{
								id{9}
							}
						}
					}
				}

				@pile{
					id{5}

					@column{
						id{6}
						@row{
							id{7}
						}
					}
				}
			}
		)qwertyuiop"));

		std::vector<std::string> expected_ids = {{
			"1", "2", "3", "6", "9"
		}};

		auto aaas = w->get_all_widgets<morda::column>();
		tst::check_ne(aaas.size(), size_t(0), SL);

		for(const auto& id : expected_ids){
            auto i = std::find_if(
					aaas.begin(),
					aaas.end(),
					[&id](const decltype(aaas)::value_type& wg) -> bool {
						return wg->id == id;
					}
				);
            tst::check(i != aaas.end(), SL) << "id = '" << id <<"' not found";
		}
	});
});
}
