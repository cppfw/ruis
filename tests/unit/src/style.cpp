#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/span_file.hpp>

#include <ruis/style.hpp>
#include <ruis/util/color.hpp>
#include <ruis/util/length.hpp>
#include <ruis/res/tml.hpp>

#include "../../harness/util/dummy_context.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace{
const tst::set set("style", [](tst::suite& suite){
    // test style_sheet loading and getting style value descriptions
    suite.add("style_sheet__basic", [](){
        auto desc = tml::read(R"qwertyuiop(
                version{1}
                ruis{
                    color_background{0xff353535}
                    color_middleground{0xff424242}
                    color_foreground{0xff505050}

                    color_text_normal{0xffffffff}

                    color_highlight{0xffad9869}

                    length_tree_view_item_indent{17pp}
                }
                user{}
            )qwertyuiop"s);

        ruis::style_sheet ss(std::move(desc));

        auto color_bg = ss.get("color_background"sv);
        tst::check(color_bg, SL);
        tst::check_eq(tml::to_string(*color_bg), "0xff353535"s, SL);

        auto length_tv_indent = ss.get("length_tree_view_item_indent"sv);
        tst::check(length_tv_indent, SL);
        tst::check_eq(tml::to_string(*length_tv_indent), "17pp"s, SL);

        auto non_existent = ss.get("non_existent_id"sv);
        tst::check(!non_existent, SL);
    });

    // test that style values can be obtained from style
    suite.add("style__basic", [](){
        auto desc = tml::read(R"qwertyuiop(
            version{1}
            ruis{
                color_background{0xff353535}
                color_middleground{0xff424242}
                color_foreground{0xff505050}

                color_text_normal{0xffffffff}

                color_highlight{0xffad9869}

                length_tree_view_item_indent{17pp}
            }
            user{}
        )qwertyuiop"s);

        auto ss = utki::make_shared<ruis::style_sheet>(std::move(desc));

        auto c = make_dummy_context();

        ruis::style s(c.get().res_loader);

        s.set(std::move(ss));

        auto color_bg = s.get<ruis::color>("color_background"sv);
        tst::check_eq(color_bg.get(), ruis::color(0xff353535), SL);

        auto length_ident = s.get<ruis::length>("length_tree_view_item_indent"sv);
        tst::check_eq(length_ident.get(), ruis::length::make_pp(17), SL);
    });

    // test that style values are updated when style sheet is changed
    suite.add("style__values_are_updated_when_sheet_changes", [](){
        auto c = make_dummy_context();

        ruis::style s(c.get().res_loader);

        auto ss1 = utki::make_shared<ruis::style_sheet>(
            tml::read(
                R"qwertyuiop(
                    version{1}
                    ruis{
                        color_background{0xff353535}
                        length_tree_view_item_indent{17pp}
                    }
                )qwertyuiop"s
            )
        );

        s.set(ss1);

        auto color_bg = s.get<ruis::color>("color_background"sv);
        tst::check_eq(color_bg.get(), ruis::color(0xff353535), SL);

        auto length_ident = s.get<ruis::length>("length_tree_view_item_indent"sv);
        tst::check_eq(length_ident.get(), ruis::length::make_pp(17), SL);

        auto ss2 = utki::make_shared<ruis::style_sheet>(
            tml::read(
                R"qwertyuiop(
                    version{1}
                    ruis{
                        color_background{0xff00ff00}
                        length_tree_view_item_indent{13pp}
                    }
                )qwertyuiop"s
            )
        );

        s.set(ss2);

        tst::check_eq(color_bg.get(), ruis::color(0xff00ff00), SL);
        tst::check_eq(length_ident.get(), ruis::length::make_pp(13), SL);

        {
            auto color_bg = s.get<ruis::color>("color_background"sv);
            tst::check_eq(color_bg.get(), ruis::color(0xff00ff00), SL);
        }

        {
            auto length_ident = s.get<ruis::length>("length_tree_view_item_indent"sv);
            tst::check_eq(length_ident.get(), ruis::length::make_pp(13), SL);
        }
    });

    // test resource type style values
    suite.add("style__resource_values", [](){
        auto c = make_dummy_context();

        auto res_pack_desc = R"qwertyuiop(
            tml_resource1{
                forest{Hello world!}
            }
            tml_resource2{
                forest{World, hello!}
            }
        )qwertyuiop"s;

        c.get().loader().mount_res_pack(papki::span_file(utki::make_span(res_pack_desc)));

        ruis::style s(c.get().res_loader);

        auto ss1 = utki::make_shared<ruis::style_sheet>(
            tml::read(
                R"qwertyuiop(
                    version{1}
                    ruis{
                        tml_style_value{tml_resource1}
                    }
                )qwertyuiop"s
            )
        );

        s.set(ss1);

        auto tml_sv = s.get<ruis::res::tml>("tml_style_value"sv);
        tst::check(tml_sv.get(), SL);
        tst::check_eq(tml_sv.get()->forest(), tml::read("Hello world!"s), SL);

        auto ss2 = utki::make_shared<ruis::style_sheet>(
            tml::read(
                R"qwertyuiop(
                    version{1}
                    ruis{
                        tml_style_value{tml_resource2}
                    }
                )qwertyuiop"s
            )
        );

        s.set(ss2);

        tst::check_eq(tml_sv.get()->forest(), tml::read("World, hello!"s), SL);

        {
            auto tml_sv = s.get<ruis::res::tml>("tml_style_value"sv);
            tst::check(tml_sv.get(), SL);
            tst::check_eq(tml_sv.get()->forest(), tml::read("World, hello!"s), SL);
        }
    });

    suite.add("styled__value_default_constructor", [](){
        ruis::styled<ruis::color> c;
        tst::check(c.get().is_undefined(), SL);

        ruis::styled<ruis::length> l;
        tst::check(l.get().is_undefined(), SL);

        ruis::styled<ruis::dimension> d;
        tst::check(d.get().is_undefined(), SL);
    });
});
}
