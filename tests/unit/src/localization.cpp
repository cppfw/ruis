#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/util/localization.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace{
const tst::set set("localization", [](tst::suite& suite){
    suite.add("load", [](){
        auto desc = tml::read(R"qwertyuiop(
            vocabulary{
                str_1{
                    str{"hello"}
                }
                str_2{
                    str{world!}
                }
            }
        )qwertyuiop");

        ruis::localization loc(desc);

        auto wording_1 = loc.get("str_1"sv);
        auto wording_2 = loc.get("str_2"sv);
        {
            bool thrown = false;
            try{
                loc.get("str_3"sv);
                tst::check(false, SL);
            }catch(std::invalid_argument&){
                thrown = true;
            }
            tst::check(thrown, SL);
        }

        tst::check(!wording_1.empty(), SL);
        tst::check(!wording_2.empty(), SL);

        tst::check_eq(wording_1.id(), "str_1"s, SL);
        tst::check_eq(wording_2.id(), "str_2"s, SL);
        tst::check(wording_1.string() == U"hello"sv, SL);
        tst::check(wording_2.string() == U"world!"sv, SL);
    });
});
}
