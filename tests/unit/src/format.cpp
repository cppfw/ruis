#include <tst/set.hpp>
#include <tst/check.hpp>

#include <ruis/util/format.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace{
const tst::set set("format", [](tst::suite& suite){
    suite.add("basic", [](){
        auto str = ruis::format(
            U"{1} {0}!"sv, //
            {
                U"world"s,
                U"Hello"s
            }
        );

        tst::check(str == U"Hello world!"s, SL);
    });

    suite.add("substitute_same_string_twice", [](){
        auto str = ruis::format(
            U"{1}-{1} {0}!"sv, //
            {
                U"world"s,
                U"Hello"s
            }
        );

        tst::check(str == U"Hello-Hello world!"s, SL);
    });

    suite.add("too_big_replacement_id_throws", [](){
        bool thrown = false;
        try{
            ruis::format(
                U"{10000} {0}!"sv, //
                {
                    U"world"s,
                    U"Hello"s
                }
            );
            tst::check(false, SL);
        }catch(std::invalid_argument&){
            thrown = true;
        }

        tst::check(thrown, SL);
    });

    suite.add("invalid_replacement_id_throws", [](){
        bool thrown = false;
        try{
            ruis::format(
                U"{0} {asd0}!"sv, //
                {
                    U"world"s,
                    U"Hello"s
                }
            );
            tst::check(false, SL);
        }catch(std::invalid_argument&){
            thrown = true;
        }
        
        tst::check(thrown, SL);
    });
});
}