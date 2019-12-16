#include "../../src/morda/Morda.hpp"
#include "../../src/morda/widgets/group/Column.hpp"

#include "../helpers/fake_renderer/FakeRenderer.hpp"


int main(int argc, char** argv){
	// test the get_all_widgets() function
	{
		morda::Morda m(std::make_shared<FakeRenderer>(), 0, 0, [](std::function<void()>&&){});
		auto w = m.inflater.inflate(*stob::parse(R"qwertyuiop(
			Container{
				Column{
					id{1}
				}

				Column{
					id{2}

					Column{
						id{3}
					}
					Row{
						id{4}

						Pile{
							id{8}

							Column{
								id{9}
							}
						}
					}
				}

				Pile{
					id{5}

					Column{
						id{6}
						Row{
							if{7}
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
