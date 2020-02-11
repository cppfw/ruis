#include "../../src/morda/context.hpp"
#include "../../src/morda/widgets/group/Pile.hpp"

#include "../helpers/fake_renderer/FakeRenderer.hpp"


int main(int argc, char** argv){
	// test that whole definition chain is substituted
	{
		morda::gui m(std::make_shared<FakeRenderer>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
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
						${dims}
					}
				}

				Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::Container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		auto lp = c->children().front()->get_layout_params();
		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
		ASSERT_ALWAYS(lp.dim[1] == 123)
	}

	// test template properties overriding
	{
		morda::gui m(std::make_shared<FakeRenderer>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
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
		auto lp = c->children().front()->get_layout_params();
		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
		ASSERT_INFO_ALWAYS(lp.dim[1] == morda::Widget::LayoutParams::min_c, "lp.dim[1] = " << lp.dim[1])
	}

	// test template arguments
	{
		morda::gui m(std::make_shared<FakeRenderer>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
			Container{
				defs{
					Cont{
						x y layout
						Container{
							x{${x}} y{${x}}
							layout{
								dx{fill} dy{max}
								${layout}
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
		auto lp = c->children().front()->get_layout_params();
		ASSERT_INFO_ALWAYS(lp.dims[0] == morda::Widget::LayoutParams::max_c, "lp.dims[0] = " << lp.dims[0])
		ASSERT_INFO_ALWAYS(lp.dims[1] == morda::Widget::LayoutParams::max_c, "lp.dims[1] = " << lp.dims[1])
	}

	// test two levels of templates
	{
		morda::gui m(std::make_shared<FakeRenderer>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
			defs{
				Cont{ x y layout dx
					Container{
						x{${x}} y{${y}}
						dy{${dx}}
						layout{
							dx{fill} dy{max}
							${layout}
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
		auto lp = c->children().front()->get_layout_params();
		ASSERT_ALWAYS(lp.dim[0] == morda::Widget::LayoutParams::max_c)
		ASSERT_INFO_ALWAYS(lp.dim[1] == morda::Widget::LayoutParams::max_c, "lp.dim[1] = " << lp.dim[1])
	}

	// test template which nests same named widget on 2nd level
	{
		morda::gui m(std::make_shared<FakeRenderer>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
			defs{
				Container_{Container}
			}
			defs{
				Container{ x y layout dx
					Pile{
						Container_{
							x{${x}} y{${y}}
							dy{${dx}}
							layout{
								${layout}
								dx{fill} dy{max}
							}
							${children}
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

	// test two defs blocks in widget
	{
		morda::gui m(std::make_shared<FakeRenderer>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
			Container{
				defs{
					Tmpl1{Pile}
				}
				defs{
					Tmpl{
						Tmpl1
					}
				}

				Tmpl
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::Container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 1)
		ASSERT_ALWAYS(std::dynamic_pointer_cast<morda::Pile>(c->children().front()))
	}

	// test variables overriding
	{
		morda::gui m(std::make_shared<FakeRenderer>(), [](std::function<void()>&&){}, 0, 0);
		auto w = m.context->inflater.inflate(puu::read(R"qwertyuiop(
			defs{
				test_var{13}
			}
			defs{
				test_var{42}
			}
			Widget{
				defs{
					test_var{666}
				}
				defs{
					test_var{2}
				}

				x{${test_var}}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		ASSERT_INFO_ALWAYS(w->rect().p.x == 2, "w->rect().p.x = " << w->rect().p.x)
	}

	return 0;
}
