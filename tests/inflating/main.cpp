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
						@{dims}
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
		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
		ASSERT_ALWAYS(lp.dim[1] == 123)
	}
	
	//test template properties overriding
	{
		auto w = m.inflater.inflate(*stob::parse(R"qwertyuiop(
			Container{
				defs{
					Cont{
						Container{
							x{10} y{12}
							layout{
								dx{fill} dy{456}
							}
						}
					}
				}

				Cont{
					x{23}
					dx{45}
					layout{
						dx{max}
					}
				}

				Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::Container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		ASSERT_ALWAYS(c->children().front()->rect().p.x == 23)
		ASSERT_ALWAYS(c->children().front()->rect().p.y == 12)
		ASSERT_ALWAYS(c->children().front()->rect().d.x == 45)
		auto lp = c->children().front()->getLayoutParams();
		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
		ASSERT_INFO_ALWAYS(lp.dim[1] == morda::Widget::LayoutParams::min_c, "lp.dim[1] = " << lp.dim[1])
	}
	
	//test template arguments
	{
		auto w = m.inflater.inflate(*stob::parse(R"qwertyuiop(
			Container{
				defs{
					Cont{
						x y layout
						Container{
							x{@{x}} y{@{x}}
							layout{
								@{layout}
								dx{fill} dy{max}
							}
						}
					}
				}

				Cont{
					x{23}
					dx{45}
					layout{
						dx{max}
					}
				}

				Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::Container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		ASSERT_ALWAYS(c->children().front()->rect().p.x == 23)
		ASSERT_ALWAYS(c->children().front()->rect().p.y == 23)
		ASSERT_ALWAYS(c->children().front()->rect().d.x == 45)
		auto lp = c->children().front()->getLayoutParams();
		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
		ASSERT_INFO_ALWAYS(lp.dim[1] == morda::Widget::LayoutParams::max_c, "lp.dim[1] = " << lp.dim[1])
	}
	
	return 0;
}