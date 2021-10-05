#include "../../../src/morda/morda/context.hpp"
#include "../../../src/morda/morda/widgets/group/pile.hpp"
#include "../../../src/morda/morda/widgets/proxy/ratio_proxy.hpp"

#include "../../harness/fake_renderer/fake_renderer.hpp"

#include "../../../src/morda/morda/gui.hpp"

int main(int argc, char** argv){
	// test that whole definition chain is substituted
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				defs{
					dims{dx{max} dy{123}}

					@Cont{
						@container{
							layout{
								dx{fill} dy{456}
							}
						}
					}
				}

				@Cont{
					layout{
						${dims}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		auto lp = c->children().front()->get_layout_params();
		ASSERT_ALWAYS(lp.dims[0] == morda::widget::layout_params::max)
		ASSERT_ALWAYS(lp.dims[1] == 123)
	}

	// test template properties overriding
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				defs{
					@Cont{
						@container{
							x{10} y{12}
							layout{
								dx{fill} dy{456}
							}
						}
					}
				}

				@Cont{
					x{23}
					dx{45}
					layout{
						dx{max}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		ASSERT_ALWAYS(c->children().front()->rect().p.x() == 23)
		ASSERT_ALWAYS(c->children().front()->rect().p.y() == 12)
		ASSERT_ALWAYS(c->children().front()->rect().d.x() == 45)
		auto lp = c->children().front()->get_layout_params();
		ASSERT_ALWAYS(lp.dims[0] == morda::widget::layout_params::max)
		ASSERT_INFO_ALWAYS(lp.dims[1] == morda::widget::layout_params::min, "lp.dim[1] = " << lp.dims[1])
	}

	// test template arguments
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				defs{
					@Cont{
						x y layout t
						@container{
							x{${x}} y{${x}}
							layout{
								dx{fill} dy{max}
								${layout}
							}
							@widget{
								id {test_widget}
								x{${t}}
							}
						}
					}
				}

				@Cont{
					x{23}
					dx{45}
					t{13}
					layout{
						dx{max}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		ASSERT_ALWAYS(c->children().front()->rect().p.x() == 23)
		ASSERT_ALWAYS(c->children().front()->rect().p.y() == 23)
		ASSERT_ALWAYS(c->children().front()->rect().d.x() == 45)
		ASSERT_ALWAYS(c->children().front()->rect().d.x() == 45)
		ASSERT_ALWAYS(c->children().front()->get_widget("test_widget").rect().p.x() == 13)
		auto lp = c->children().front()->get_layout_params();
		ASSERT_INFO_ALWAYS(lp.dims[0] == morda::widget::layout_params::max, "lp.dims[0] = " << lp.dims[0])
		ASSERT_INFO_ALWAYS(lp.dims[1] == morda::widget::layout_params::max, "lp.dims[1] = " << lp.dims[1])
	}

	// test template arguments inside of nested containers
	{
		// TRACE_ALWAYS(<< "!!!!!!!!!!!!!!!" << std::endl)
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				@container{
					defs{
						@test_box{
							r t
							@widget{
								id {test_widget}
								x {${r}}
							}
						}
					}

					@test_box{
						r {2.5}
						t {"1:2"}
					}
				}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		ASSERT_ALWAYS(c)
		auto& r = w->get_widget_as<morda::widget>("test_widget");
		ASSERT_INFO_ALWAYS(r.rect().p.x() == 2.5, "x() = " << r.rect().p.x())
	}

	// test two levels of templates
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			defs{
				@Cont{ x y layout dx
					@container{
						x{${x}} y{${y}}
						dy{${dx}}
						layout{
							dx{fill} dy{max}
							${layout}
						}
					}
				}
			}
			@container{
				defs{
					@Cont{ x y
						@Cont{
							y{67}
						}
					}
				}

				@Cont{
					x{23} y{106}
					dx{45}
					layout{
						dx{max}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		ASSERT_ALWAYS(c->children().front()->rect().p.x() == 0)
		ASSERT_ALWAYS(c->children().front()->rect().p.y() == 67)
		ASSERT_ALWAYS(c->children().front()->rect().d.x() == 0)
		ASSERT_ALWAYS(c->children().front()->rect().d.y() == 45)
		auto lp = c->children().front()->get_layout_params();
		ASSERT_ALWAYS(lp.dims[0] == morda::widget::layout_params::max)
		ASSERT_INFO_ALWAYS(lp.dims[1] == morda::widget::layout_params::max, "lp.dim[1] = " << lp.dims[1])
	}

	// test template which nests same named widget on 2nd level
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			defs{
				@Container_{@container}
			}
			defs{
				@container{ x y layout dx
					@pile{
						@Container_{
							x{${x}} y{${y}}
							dy{${dx}}
							layout{
								${layout}
								dx{fill} dy{max}
							}
							${children}
						}
						@Container_{}
					}
				}

				@Tmpl{
					@container
				}
			}
			@container{
				@Tmpl
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::pile>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 2)
		ASSERT_ALWAYS(std::dynamic_pointer_cast<morda::container>(c->children().front()))
	}

	// test template recursion detection
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		bool exc_caught = false;
		try{
			auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
				defs{
					@container{ x y layout dx
						@pile{
							@container{
								x{${x}} y{${y}}
								dy{${dx}}
								layout{
									${layout}
									dx{fill} dy{max}
								}
								${children}
							}
							@container{}
						}
					}

					@Tmpl{
						@container
					}
				}
				@container{
					@Tmpl
				}
			)qwertyuiop"));
		}catch(std::logic_error& e){
			exc_caught = true;
		}

		ASSERT_ALWAYS(exc_caught)
	}

	// test two defs blocks in widget
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				defs{
					@Tmpl1{@pile}
				}
				defs{
					@Tmpl{
						@Tmpl1
					}
				}

				@Tmpl
			}
		)qwertyuiop"));

		ASSERT_ALWAYS(w)
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		ASSERT_ALWAYS(c)
		ASSERT_ALWAYS(c->children().size() == 1)
		ASSERT_ALWAYS(std::dynamic_pointer_cast<morda::pile>(c->children().front()))
	}

	// test variables overriding
	{
		morda::gui m(std::make_shared<morda::context>(
				std::make_shared<fake_renderer>(),
				std::make_shared<morda::updater>(),
				[](std::function<void()>&&){},
				[](morda::mouse_cursor){},
				0,
				0
			));
		auto w = m.context->inflater.inflate(treeml::read(R"qwertyuiop(
			defs{
				test_var{13}
			}
			defs{
				test_var{42}
			}
			@widget{
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
		ASSERT_INFO_ALWAYS(w->rect().p.x() == 2, "w->rect().p.x() = " << w->rect().p.x())
	}

	return 0;
}
