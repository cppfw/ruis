#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/gui.hpp>
#include <ruis/container.hpp>
#include <ruis/widgets/label/color.hpp>

#include "../../harness/util/dummy_context.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace{
const tst::set set("inflating", [](tst::suite& suite){
    suite.add("whole_definition_chain_is_substituted", []{
        ruis::gui m(make_dummy_context());
		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				defs{
					dims{dx{max} dy{123}}

					@Cont{
						@container{
							lp{
								dx{fill} dy{456}
							}
						}
					}
				}

				@Cont{
					lp{
						${dims}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(w);
		tst::check_eq(c.get().children().size(), size_t(2), SL);
		auto lp = c.get().children().front().get().get_layout_params();
		tst::check_eq(lp.dims[0], ruis::lp::max, SL);
		tst::check_eq(lp.dims[1], ruis::real(123), SL);
    });

    suite.add("template_properties_overriding", []{
		ruis::gui m(make_dummy_context());
		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				defs{
					@Cont{
						@container{
							x{10} y{12}
							lp{
								dx{fill} dy{456}
							}
						}
					}
				}

				@Cont{
					x{23}
					dx{45}
					lp{
						dx{max}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(w);
		tst::check_eq(c.get().children().size(), size_t(2), SL);
		tst::check_eq(c.get().children().front().get().rect().p.x(), ruis::real(23), SL);
		tst::check_eq(c.get().children().front().get().rect().p.y(), ruis::real(12), SL);
		tst::check_eq(c.get().children().front().get().rect().d.x(), ruis::real(45), SL);
		auto lp = c.get().children().front().get().get_layout_params();
		tst::check_eq(lp.dims[0], ruis::lp::max, SL);
		tst::check_eq(lp.dims[1], ruis::lp::min, SL);
	});

    suite.add("template_arguments", []{
		ruis::gui m(make_dummy_context());
		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				defs{
					@Cont{
						x y lp t
						@container{
							x{${x}} y{${x}}
							lp{
								dx{fill} dy{max}
								${lp}
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
					lp{
						dx{max}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(w);
		tst::check_eq(c.get().children().size(), size_t(2), SL);
		tst::check_eq(c.get().children().front().get().rect().p.x(), ruis::real(23), SL);
		tst::check_eq(c.get().children().front().get().rect().p.y(), ruis::real(23), SL);
		tst::check_eq(c.get().children().front().get().rect().d.x(), ruis::real(45), SL);
		tst::check_eq(c.get().children().front().get().rect().d.x(), ruis::real(45), SL);
		tst::check_eq(c.get().children().front().get().get_widget("test_widget").rect().p.x(), ruis::real(13), SL);
		auto lp = c.get().children().front().get().get_layout_params();
		tst::check_eq(lp.dims[0], ruis::lp::max, SL);
		tst::check_eq(lp.dims[1], ruis::lp::max, SL);
	});

    suite.add("template_arguments_inside_of_nested_containers", []{
		ruis::gui m(make_dummy_context());
		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
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

		auto& r = w.get().get_widget_as<ruis::widget>("test_widget");
		tst::check_eq(r.rect().p.x(), ruis::real(2.5), SL);
	});

    suite.add("two_levels_of_templates", []{
		ruis::gui m(make_dummy_context());
		m.context.get().inflater.push_defs(R"(
			defs{
				@Cont{ x y lp dx
					@container{
						x{${x}} y{${y}}
						dy{${dx}}
						lp{
							dx{fill} dy{max}
							${lp}
						}
					}
				}
			}
		)");
		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
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
					lp{
						dx{max}
					}
				}

				@Cont{}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(w);
		tst::check_eq(c.get().children().size(), size_t(2), SL);
		tst::check_eq(c.get().children().front().get().rect().p.x(), ruis::real(-1), SL);
		tst::check_eq(c.get().children().front().get().rect().p.y(), ruis::real(67), SL);
		tst::check_eq(c.get().children().front().get().rect().d.x(), ruis::real(-1), SL);
		tst::check_eq(c.get().children().front().get().rect().d.y(), ruis::real(45), SL);
		auto lp = c.get().children().front().get().get_layout_params();
		tst::check_eq(lp.dims[0], ruis::lp::max, SL);
		tst::check_eq(lp.dims[1], ruis::lp::max, SL);
	});

    suite.add("template_which_nests_same_named_widget_on_2nd_level", []{
		ruis::gui m(make_dummy_context());
		m.context.get().inflater.push_defs(R"(
			defs{
				@Container_{@container}
			}
			defs{
				@container{ x y layout dx
					@pile{
						@Container_{
							x{${x}} y{${y}}
							dy{${dx}}
							lp{
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
		)");

		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
			@container{
				@Tmpl
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(w);
		tst::check_eq(c.get().children().size(), size_t(2), SL);
		tst::check(std::dynamic_pointer_cast<ruis::container>(c.get().children().front().to_shared_ptr()) != nullptr, SL);
	});

    suite.add("template_recursion_detection", []{
		ruis::gui m(make_dummy_context());
		bool logic_error_occurred = false;
		try{
			auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
				defs{
					@container{ x y layout dx
						@pile{
							@container{
								x{${x}} y{${y}}
								dy{${dx}}
								lp{
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
            logic_error_occurred = true;
		}catch(...){
            tst::check(false, SL);
        }

		tst::check(logic_error_occurred, SL);
	});

    suite.add("two_defs_blocks_in_widget", []{
		ruis::gui m(make_dummy_context());
		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
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

		auto c = utki::dynamic_reference_cast<ruis::container>(w);
		tst::check_eq(c.get().children().size(), size_t(1), SL);
		tst::check(std::dynamic_pointer_cast<ruis::container>(c.get().children().front().to_shared_ptr()) != nullptr, SL);
	});

    suite.add("variables_overriding", []{
		ruis::gui m(make_dummy_context());
		m.context.get().inflater.push_defs(
			R"(
				defs{
					test_var{13}
				}
				defs{
					test_var{42}
				}
			)"
		);

		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
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

		tst::check_eq(w.get().rect().p.x(), ruis::real(2), SL);
	});

	suite.add("inflating_row_layout", [](){
		ruis::gui m(make_dummy_context());

		auto w = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
			@row{
				@widget{
					id{ widget1 }
				}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(w);

		tst::check_eq(c.get().children().size(), size_t(1), SL);
		tst::check_eq(c.get().children().front().get().id(), "widget1"sv, SL);
	});

	suite.add("inflating_left_alignment", [](){
		ruis::gui m(make_dummy_context());

		auto t = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
			@left{
				@widget{
					id{ widget1 }
				}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(t);

		auto w = c.get().try_get_widget("widget1");

		tst::check(w, SL);
	});

	suite.add("inflating_top_alignment", [](){
		ruis::gui m(make_dummy_context());

		auto t = m.context.get().inflater.inflate(treeml::read(R"qwertyuiop(
			@top{
				@widget{
					id{ widget1 }
				}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(t);

		auto w = c.get().try_get_widget("widget1");

		tst::check(w, SL);
	});

	suite.add("using_vars_in_children", [](){
		ruis::gui g(make_dummy_context());

		auto t = g.context.get().inflater.inflate(tml::read(R"qwertyuiop(
			@container{
				defs{
					a{13}
				}

				@color{
					color{${a}}
				}
			}
		)qwertyuiop"));

		auto c = utki::dynamic_reference_cast<ruis::container>(t);

		tst::check_eq(c.get().size(), size_t(1), SL);
		
		auto& w = dynamic_cast<ruis::color&>(c.get().children().front().get());
		tst::check_eq(w.get_color(), uint32_t(13), SL);
	});
});
}
