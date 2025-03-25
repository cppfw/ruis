#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/style.hpp>
#include <ruis/util/color.hpp>
#include <ruis/util/length.hpp>

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

        ruis::style_sheet ss(std::move(desc));

        auto c = make_dummy_context();

        ruis::style s(c.get().res_loader);

        s.set(std::move(ss));

        auto color_bg = s.get<ruis::color>("color_background"sv);

        tst::check_eq(color_bg.get(), ruis::color(0xff353535), SL);

        auto length_ident = s.get<ruis::length>("length_tree_view_item_indent"sv);

        tst::check_eq(length_ident.get(), ruis::length::make_pp(17), SL);
    });

    // TODO: test that style values are updated when style sheet is changed
});
}
