#include <tst/set.hpp>
#include <tst/check.hpp>

#include <morda/gui.hpp>
#include <morda/widgets/group/pile.hpp>

#include "../../harness/util/dummy_context.hpp"

namespace{
tst::set set("inflaitng", [](tst::suite& suite){
    suite.add("whole_definition_chain_is_substituted", []{
        morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		tst::check(c != nullptr, SL);
		tst::check_eq(c->children().size(), size_t(2), SL);
		auto lp = c->children().front()->get_layout_params();
		tst::check_eq(lp.dims[0], morda::widget::layout_params::max, SL);
		tst::check_eq(lp.dims[1], morda::real(123), SL);
    });

    suite.add("template_properties_overriding", []{
		morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		tst::check(c != nullptr, SL);
		tst::check_eq(c->children().size(), size_t(2), SL);
		tst::check_eq(c->children().front()->rect().p.x(), morda::real(23), SL);
		tst::check_eq(c->children().front()->rect().p.y(), morda::real(12), SL);
		tst::check_eq(c->children().front()->rect().d.x(), morda::real(45), SL);
		auto lp = c->children().front()->get_layout_params();
		tst::check_eq(lp.dims[0], morda::widget::layout_params::max, SL);
		tst::check_eq(lp.dims[1], morda::widget::layout_params::min, SL);
	});

    suite.add("template_arguments", []{
		morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		tst::check(c != nullptr, SL);
		tst::check_eq(c->children().size(), size_t(2), SL);
		tst::check_eq(c->children().front()->rect().p.x(), morda::real(23), SL);
		tst::check_eq(c->children().front()->rect().p.y(), morda::real(23), SL);
		tst::check_eq(c->children().front()->rect().d.x(), morda::real(45), SL);
		tst::check_eq(c->children().front()->rect().d.x(), morda::real(45), SL);
		tst::check_eq(c->children().front()->get_widget("test_widget").rect().p.x(), morda::real(13), SL);
		auto lp = c->children().front()->get_layout_params();
		tst::check_eq(lp.dims[0], morda::widget::layout_params::max, SL);
		tst::check_eq(lp.dims[1], morda::widget::layout_params::max, SL);
	});

    suite.add("template_arguments_inside_of_nested_containers", []{
		morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		tst::check(c != nullptr, SL);
		auto& r = w->get_widget_as<morda::widget>("test_widget");
		tst::check_eq(r.rect().p.x(), morda::real(2.5), SL);
	});

    suite.add("two_levels_of_templates", []{
		morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		tst::check(c != nullptr, SL);
		tst::check_eq(c->children().size(), size_t(2), SL);
		tst::check_eq(c->children().front()->rect().p.x(), morda::real(0), SL);
		tst::check_eq(c->children().front()->rect().p.y(), morda::real(67), SL);
		tst::check_eq(c->children().front()->rect().d.x(), morda::real(0), SL);
		tst::check_eq(c->children().front()->rect().d.y(), morda::real(45), SL);
		auto lp = c->children().front()->get_layout_params();
		tst::check_eq(lp.dims[0], morda::widget::layout_params::max, SL);
		tst::check_eq(lp.dims[1], morda::widget::layout_params::max, SL);
	});

    suite.add("template_which_nests_same_named_widget_on_2nd_level", []{
		morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		auto c = std::dynamic_pointer_cast<morda::pile>(w);
		tst::check(c != nullptr, SL);
		tst::check_eq(c->children().size(), size_t(2), SL);
		tst::check(std::dynamic_pointer_cast<morda::container>(c->children().front()) != nullptr, SL);
	});

    suite.add("template_recursion_detection", []{
		morda::gui m(make_dummy_context());
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
            tst::check(false, SL);
		}catch(std::logic_error&){
            // expected to get here
		}catch(...){
            tst::check(false, SL);
        }
	});

    suite.add("two_defs_blocks_in_widget", []{
		morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		auto c = std::dynamic_pointer_cast<morda::container>(w);
		tst::check(c != nullptr, SL);
		tst::check_eq(c->children().size(), size_t(1), SL);
		tst::check(std::dynamic_pointer_cast<morda::pile>(c->children().front()) != nullptr, SL);
	});

    suite.add("variables_overriding", []{
		morda::gui m(make_dummy_context());
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

		tst::check(w != nullptr, SL);
		tst::check_eq(w->rect().p.x(), morda::real(2), SL);
	});
});
}
