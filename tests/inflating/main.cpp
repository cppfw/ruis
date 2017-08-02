#include "../../src/morda/Morda.hpp"

#include "FakeRenderer.hpp"


class TestMorda : public morda::Morda{
	
public:
	TestMorda() : morda::Morda(utki::makeShared<FakeRenderer>(), 0, 0){}
	void postToUiThread_ts(std::function<void()>&& f) override{
		
	}
};

int main(int argc, char** argv){
	TestMorda m;
	
	//test that whole definition chain is substituted
	{
		auto w = m.inflater.inflate(*stob::parse(R"qwertyuiop(
			Container{
				defs{
					dims{dx{max} dy{123}}

					Cont{
						Container{
							layout{
								dx{fill} dy{456}
							}
						}
					}
				}

				Cont{
					layout{
					//	@{dims}
					}
				}

				Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::Container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		auto lp = c->children().front()->getLayoutParams();
//		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
//		ASSERT_ALWAYS(lp.dim[1] == 123)
	}
	
	return 0;
}