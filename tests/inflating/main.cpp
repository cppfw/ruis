#include "../../src/morda/Morda.hpp"
#include "../../src/morda/widgets/group/Pile.hpp"

#include "FakeRenderer.hpp"


int main(int argc, char** argv){
	morda::Morda m(std::make_shared<FakeRenderer>(), 0, 0, [](std::function<void()>&&){});
	
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
	
	//test two levels of templates
	{
		auto w = m.inflater.inflate(*stob::parse(R"qwertyuiop(
			defs{
				Cont{ x y layout dx
					Container{
						x{@{x}} y{@{y}}
						dy{@{dx}}
						layout{
							@{layout}
							dx{fill} dy{max}
						}
					}
				}
			}
			Container{
				defs{
					Cont{ x y
						Cont{
							y{67}
						}
					}
				}

				Cont{
					x{23} y{106}
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
		ASSERT_ALWAYS(c->children().front()->rect().p.x == 0)
		ASSERT_ALWAYS(c->children().front()->rect().p.y == 67)
		ASSERT_ALWAYS(c->children().front()->rect().d.x == 0)
		ASSERT_ALWAYS(c->children().front()->rect().d.y == 45)
		auto lp = c->children().front()->getLayoutParams();
		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
		ASSERT_INFO_ALWAYS(lp.dim[1] == morda::Widget::LayoutParams::max_c, "lp.dim[1] = " << lp.dim[1])
	}
	
	//test template which nests same named widget on 2nd level
	{
		auto w = m.inflater.inflate(*stob::parse(R"qwertyuiop(
			defs{
				Container_{Container}
			}
			defs{
				Container{ x y layout dx
					Pile{
						Container_{
							x{@{x}} y{@{y}}
							dy{@{dx}}
							layout{
								@{layout}
								dx{fill} dy{max}
							}
							@{children}
						}
						Container_{}
					}
				}

				Tmpl{
					Container
				}
			}
			Container{
				Tmpl
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::Pile>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		ASSERT_ALWAYS(std::dynamic_pointer_cast<morda::Container>(c->children().front()))
	}
	
	return 0;
}