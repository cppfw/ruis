#include "../../src/morda/gui.hpp"
#include "../../src/morda/widgets/group/Column.hpp"

#include "../helpers/fake_renderer/FakeRenderer.hpp"

int main(int argc, char** argv){
	// test the get_all_widgets() function
	{
		morda::gui m(std::make_shared<FakeRenderer>(), std::make_shared<morda::updater>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
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

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::Container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 3)

		auto aaas = c->get_all_widgets<morda::Column>();
		ASSERT_ALWAYS(aaas.size() != 0)
		for(auto& id : expected_ids){
			ASSERT_INFO_ALWAYS(
					std::find_if(aaas.begin(), aaas.end(), [&id](const decltype(aaas)::value_type& w)->bool{return w->id == id;}) != aaas.end(),
					"id = '" << id <<"' not found"
				)
		}
	}

	return 0;
}
