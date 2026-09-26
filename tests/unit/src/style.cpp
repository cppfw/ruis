#include <fsif/span_file.hpp>
#include <ruis/render/null/context.hpp>
#include <ruis/res/tml.hpp>
#include <ruis/style/style_provider.hpp>
#include <ruis/util/color.hpp>
#include <ruis/util/length.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace {
utki::shared_ref<ruis::style_provider> make_style_provider()
{
	auto ren_ctx = utki::make_shared<ruis::render::null::context>();
	auto res_loader = utki::make_shared<ruis::resource_loader>(
		ren_ctx, //
		utki::make_shared<ruis::render::renderer::objects>(ren_ctx)
	);
	return utki::make_shared<ruis::style_provider>(res_loader);
}
} // namespace

namespace {
const tst::set set("style", [](tst::suite& suite) {
	// test style_sheet loading and getting style value descriptions
	suite.add("style_sheet__basic", []() {
		auto desc = tml::read(R"qwertyuiop(
                version{1}
                ruis{
                    color_background{0xff353535}
                    color_primary{0xff505050}

                    color_text{0xffffffff}
                    color_text_special{0xff0000ff}

                    color_highlight{0xffad9869}

                    len_indent{17pp}
                    len_border{1pp}
                }
                user{}
            )qwertyuiop"s);

		ruis::style_sheet ss(std::move(desc));

		auto& color_bg = ss.get(ruis::style::color_background);
		tst::check_eq(tml::to_string(color_bg), "0xff353535"s);

		auto& color_tv_special = ss.get(ruis::style::color_text_special);
		tst::check_eq(tml::to_string(color_tv_special), "0xff0000ff"s, SL);

		auto& length_tv_indent = ss.get(ruis::style::len_indent);
		tst::check_eq(tml::to_string(length_tv_indent), "17pp"s, SL);

		auto& length_tv_border = ss.get(ruis::style::len_border);
		tst::check_eq(tml::to_string(length_tv_border), "1pp"s, SL);

		auto non_existent = ss.get("non_existent_id"sv);
		tst::check(!non_existent, SL);
	});

	// test that style values can be obtained from style
	suite.add("style__basic", []() {
		auto desc = tml::read(R"qwertyuiop(
            version{1}
            ruis{
                color_background{0xff353535}
                color_primary{0xff505050}
                color_text_special{0xff00ff11}

                color_highlight{0xffad9869}

                len_indent{17pp}
                len_gap{15pp}
                len_border{1pp}
            }
            user{
                real_style_value{13}
            }
        )qwertyuiop"s);

		auto ss = utki::make_shared<ruis::style_sheet>(std::move(desc));

		auto style_provider = make_style_provider();
		auto& s = style_provider.get();

		s.set(std::move(ss));

		auto color_bg = s.get_color_background();
		tst::check_eq(color_bg.get(), ruis::color(0xff353535), SL);

		auto color_tv_special = s.get_color_text_special();
		tst::check_eq(color_tv_special.get(), ruis::color(0xff00ff11), SL);

		auto length_ident = s.get_len_indent();
		tst::check_eq(length_ident.get(), ruis::length::make_pp(17), SL);

		auto length_border = s.get_len_border();
		tst::check_eq(length_border.get(), ruis::length::make_pp(1), SL);

		auto length_gap = s.get_len_gap();
		tst::check_eq(length_gap.get(), ruis::length::make_pp(15), SL);

		auto length_small_gap = s.get_len_gap_small();
		tst::check_eq(length_small_gap.get(), ruis::length::make_pp(4), SL);

		auto length_big_gap = s.get_len_gap_big();
		tst::check_eq(length_big_gap.get(), ruis::length::make_pp(16), SL);
	});

	// test that ruis::real values can be obtained from style
	suite.add("style__basic__real", []() {
		auto ss_desc = tml::read(R"qwertyuiop(
            version{1}
            user{
                real_style_value{13}
            }
        )qwertyuiop"s);

		auto ren_ctx = utki::make_shared<ruis::render::null::context>();
		auto res_loader = utki::make_shared<ruis::resource_loader>(
			ren_ctx, //
			utki::make_shared<ruis::render::renderer::objects>(ren_ctx)
		);
		ruis::style_provider s(res_loader);

		s.set(utki::make_shared<ruis::style_sheet>(std::move(ss_desc)));

		auto real_val = s.get<ruis::real>("real_style_value"sv);
		tst::check_eq(real_val.get(), ruis::real(13), SL);
	});

	// test that ruis::align values can be obtained from style
	suite.add("style__basic__align", []() {
		auto ss_desc = tml::read(R"qwertyuiop(
            version{1}
            user{
                align_value{back}
            }
        )qwertyuiop"s);

		auto style_provider = make_style_provider();
		auto& s = style_provider.get();

		s.set(utki::make_shared<ruis::style_sheet>(std::move(ss_desc)));

		auto val = s.get<ruis::align>("align_value"sv);
		tst::check(val.get() == ruis::align::back, SL);
	});

	// test that style values are updated when style sheet is changed
	suite.add("style__values_are_updated_when_sheet_changes", []() {
		auto style_provider = make_style_provider();
		auto& s = style_provider.get();

		auto ss1 = utki::make_shared<ruis::style_sheet>(tml::read(
			R"qwertyuiop(
                    version{1}
                    ruis{
                        color_background{0xff353535}
                        len_indent{17pp}
                    }
                )qwertyuiop"s
		));

		s.set(ss1);

		auto color_bg = s.get_color_background();
		tst::check_eq(color_bg.get(), ruis::color(0xff353535), SL);

		auto length_ident = s.get_len_indent();
		tst::check_eq(length_ident.get(), ruis::length::make_pp(17));

		auto ss2 = utki::make_shared<ruis::style_sheet>(tml::read(
			R"qwertyuiop(
                    version{1}
                    ruis{
                        color_background{0xff00ff00}
                        len_indent{13pp}
                    }
                )qwertyuiop"s
		));

		s.set(ss2);

		tst::check_eq(color_bg.get(), ruis::color(0xff00ff00), SL);
		tst::check_eq(length_ident.get(), ruis::length::make_pp(13));

		{
			auto color_bg = s.get_color_background();
			tst::check_eq(color_bg.get(), ruis::color(0xff00ff00), SL);
		}

		{
			auto length_ident = s.get_len_indent();
			tst::check_eq(length_ident.get(), ruis::length::make_pp(13));
		}
	});

	// test resource type style values
	suite.add("style__resource_values", []() {
		auto res_pack_desc = R"qwertyuiop(
            tml_resource1{
                forest{Hello world!}
            }
            tml_resource2{
                forest{World, hello!}
            }
        )qwertyuiop"s;

		auto style_provider = make_style_provider();
		auto& s = style_provider.get();
		s.res_loader.get().mount_res_pack(fsif::span_file(utki::make_span(res_pack_desc)));

		auto ss1 = utki::make_shared<ruis::style_sheet>(tml::read(
			R"qwertyuiop(
                    version{1}
                    user{
                        tml_style_value{tml_resource1}
                    }
                )qwertyuiop"s
		));

		s.set(ss1);

		auto tml_sv = s.get<ruis::res::tml>("tml_style_value"sv);
		tst::check(tml_sv.get(), SL);
		tst::check_eq(tml_sv.get()->forest, tml::read("Hello world!"s), SL);

		auto ss2 = utki::make_shared<ruis::style_sheet>(tml::read(
			R"qwertyuiop(
                    version{1}
                    user{
                        tml_style_value{tml_resource2}
                    }
                )qwertyuiop"s
		));

		s.set(ss2);

		tst::check_eq(tml_sv.get()->forest, tml::read("World, hello!"s), SL);

		{
			auto tml_sv = s.get<ruis::res::tml>("tml_style_value"sv);
			tst::check(tml_sv.get(), SL);
			tst::check_eq(tml_sv.get()->forest, tml::read("World, hello!"s), SL);
		}
	});

	// test that default fallback values are returned when style values are missing from the style sheet
	suite.add("style_sheet__default_fallback_values", []() {
		auto desc = tml::read(R"qwertyuiop(
            version{1}
            ruis{}
            user{}
        )qwertyuiop"s);

		ruis::style_sheet ss(std::move(desc));

		tst::check_eq(tml::to_string(ss.get(ruis::style::color_background)), "0xff101010"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_panel)), "0xff424242"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_special)), "0xffff8080"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_primary)), "0xff505050"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_secondary)), "0xff303030"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_highlight)), "0xffad9869"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_dimmed)), "0xb0000000"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_text)), "0xffffffff"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_text_secondary)), "0xffa0a0a0"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_text_special)), "0xff80ff80"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_indent)), "17pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_gap_small)), "4pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_gap)), "8pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_gap_big)), "16pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_border)), "1pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_size_primary)), "14pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_size_secondary)), "12pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_size_title)), "22pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_face_primary)), "ruis_fnt_normal"s, SL);
	});

	// test that when a style value is present in the style sheet, the sheet value is returned (not the default)
	suite.add("style_sheet__explicit_value_overrides_default", []() {
		auto desc = tml::read(R"qwertyuiop(
            version{1}
            ruis{
                color_background{0xff123456}
                color_panel{0xff234567}
                color_special{0xff345678}
                color_primary{0xff456789}
                color_secondary{0xff56789a}
                color_highlight{0xff6789ab}
                color_dimmed{0xff789abc}
                color_text{0xff89abcd}
                color_text_secondary{0xff9abcde}
                color_text_special{0xffabcdef}
                len_indent{20pp}
                len_gap_small{6pp}
                len_gap{12pp}
                len_gap_big{30pp}
                len_border{2pp}
                font_size_primary{14pp}
                font_size_secondary{10pp}
                font_size_title{26pp}
                font_face_primary{custom_font}
            }
            user{}
        )qwertyuiop"s);

		ruis::style_sheet ss(std::move(desc));

		tst::check_eq(tml::to_string(ss.get(ruis::style::color_background)), "0xff123456"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_panel)), "0xff234567"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_special)), "0xff345678"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_primary)), "0xff456789"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_secondary)), "0xff56789a"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_highlight)), "0xff6789ab"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_dimmed)), "0xff789abc"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_text)), "0xff89abcd"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_text_secondary)), "0xff9abcde"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::color_text_special)), "0xffabcdef"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_indent)), "20pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_gap_small)), "6pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_gap)), "12pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_gap_big)), "30pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::len_border)), "2pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_size_primary)), "14pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_size_secondary)), "10pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_size_title)), "26pp"s, SL);
		tst::check_eq(tml::to_string(ss.get(ruis::style::font_face_primary)), "custom_font"s, SL);
	});

	// test that parsing fails when a style value name is present but its value is empty
	suite.add("style_sheet__empty_value_fails", []() {
		auto desc = tml::read(R"qwertyuiop(
            version{1}
            ruis{
                color_background{}
            }
            user{}
        )qwertyuiop"s);

		bool threw = false;
		try {
			ruis::style_sheet ss(std::move(desc));
		} catch (const std::invalid_argument&) {
			threw = true;
		}
		tst::check(threw, SL);
	});

	suite.add("styled__default_constructor", []() {
		ruis::styled<ruis::real> r;
		tst::check_eq(r.get(), ruis::real(0), SL);

		ruis::styled<ruis::color> c;
		tst::check(c.get().is_undefined(), SL);

		ruis::styled<ruis::length> l;
		tst::check(l.get().is_undefined(), SL);

		ruis::styled<ruis::dimension> d;
		tst::check(d.get().is_undefined(), SL);

		ruis::styled<ruis::res::tml> tml;
		tst::check(tml.get() == nullptr, SL);
	});

	suite.add("styled__value_constructor", []() {
		ruis::styled<ruis::real> r = 13;
		tst::check_eq(r.get(), ruis::real(13), SL);

		ruis::styled<ruis::color> c = 0xff00ff00;
		tst::check_eq(c.get().to_uint32_t(), uint32_t(0xff00ff00), SL);

		ruis::styled<ruis::length> l = ruis::length::make_pp(10);
		tst::check_eq(l.get(), ruis::length::make_pp(10), SL);

		ruis::styled<ruis::dimension> d = ruis::length::make_pp(10);
		tst::check_eq(d.get(), ruis::dimension(ruis::length::make_pp(10)), SL);

		ruis::styled<ruis::res::tml> tml = std::make_shared<ruis::res::tml>(tml::read("Hello{world!}"));
		tst::check(tml.get() != nullptr, SL);
		tst::check_eq(tml.get()->forest, tml::read("Hello{world!}"), SL);
	});
});
} // namespace
