#include <array>
#include <string>
#include <vector>

#if defined(__linux__)
#	include <unistd.h>
#endif

#include <fsif/native_file.hpp>
#include <ruis/context.hpp>
#include <ruis/widget/label/text.hpp>
#include <tst/check.hpp>
#include <tst/set.hpp>

#include "../../../../harness/util/dummy_context.hpp"

using namespace ruis;

namespace {
// A long run of short words whose total width is far greater than the narrow boxes used below.
constexpr auto many_words = U"aa bb cc dd ee ff gg hh ii jj kk ll mm nn oo pp qq rr ss tt";
// A single word with no spaces that is far wider than the narrow boxes used below.
constexpr auto long_word =
	U"wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww";

// The ruis resource pack directory, resolved relative to this test binary's own location so
// that the lookup does not depend on the working directory. The binary lives at
// <monorepo>/out/<preset>/exe/ruis-tests/ruis-tests and the pack at <monorepo>/repos/ruis/res/ruis_res/.
std::string res_pack_dir_from_exe()
{
#if defined(__linux__)
	std::array<char, 4096> buf{};
	const auto n = readlink("/proc/self/exe", buf.data(), buf.size() - 1);
	if (n <= 0) {
		return {};
	}
	buf[n] = '\0';
	auto path = std::string(buf.data());
	const auto slash = path.find_last_of('/');
	if (slash == std::string::npos) {
		return {};
	}
	return path.substr(0, slash) + "/../../../../repos/ruis/res/ruis_res/";
#else
	return {};
#endif
}

// Create a dummy context with the ruis resource pack mounted, so that the default font
// ('ruis_fnt_normal') is available and text widgets can measure text. The pack is located
// relative to the test binary first (working-directory independent), then in a few working-
// directory relative fallback locations.
utki::shared_ref<ruis::context> make_context_with_font()
{
	auto ctx = make_dummy_context();

	// fsif::is_dir() requires a trailing slash to detect a directory, and mount_res_pack() expects
	// a directory (it reads <dir>/main.res), so the candidates must end with a slash.
	std::vector<std::string> candidates;
	if (auto from_exe = res_pack_dir_from_exe(); !from_exe.empty()) {
		candidates.emplace_back(std::move(from_exe));
	}
	candidates.emplace_back("ruis_res/");
	candidates.emplace_back("../../../../repos/ruis/res/ruis_res/");
	candidates.emplace_back("../../res/ruis_res/");
	candidates.emplace_back("../res/ruis_res/");

	for (auto& p : candidates) {
		if (!fsif::is_dir(p)) {
			continue;
		}
		fsif::native_file fi(p);
		try {
			ctx.get().loader().mount_res_pack(fi);
			return ctx;
		} catch (...) { // NOLINT(bugprone-empty-catch)
			// try the next candidate
		}
	}

	return ctx;
}

// Create a text widget, resize it to the given size (which triggers a (re)layout and therefore
// recomputes the lines against the given width) and return it.
utki::shared_ref<ruis::text> make_resized_text(
	const utki::shared_ref<ruis::context>& c, //
	text::all_parameters params,
	std::u32string_view text,
	real w,
	real h
)
{
	auto t = ruis::make::text(c, std::move(params), std::u32string(text));
	t.get().resize(ruis::vec2(w, h));
	return t;
}
} // namespace

namespace {
const tst::set set("text_lines", [](tst::suite& suite) {
	suite.add("single_line__one_line_when_it_fits", [] {
		auto c = make_context_with_font();
		auto t = make_resized_text(c, {}, many_words, 100000, 40);
		tst::check_eq(t.get().get_lines().size(), size_t(1), SL);
		tst::check_eq(t.get().is_ellipsized(), false, SL);
	});

	suite.add("single_line__truncated_with_ellipsis_when_too_wide", [] {
		auto c = make_context_with_font();
		auto t = make_resized_text(c, {}, many_words, 40, 40);
		tst::check_eq(t.get().get_lines().size(), size_t(1), SL);
		tst::check_eq(t.get().is_ellipsized(), true, SL);
		// the surviving line is a strict prefix of the original text (some words dropped)
		auto full = std::u32string_view(t.get().get_string().get());
		const auto& line = t.get().get_lines().front().span;
		tst::check(line.size() < full.size(), SL);
		tst::check(full.substr(0, line.size()) == line, SL);
	});

	suite.add("single_line__single_long_word_is_truncated_in_place", [] {
		auto c = make_context_with_font();
		auto t = make_resized_text(c, {}, long_word, 40, 40);
		tst::check_eq(t.get().get_lines().size(), size_t(1), SL);
		tst::check_eq(t.get().is_ellipsized(), true, SL);
		auto full = std::u32string_view(t.get().get_string().get());
		const auto& line = t.get().get_lines().front().span;
		tst::check(line.size() < full.size(), SL);
		tst::check(full.substr(0, line.size()) == line, SL);
	});

	suite.add("multiline__one_line_when_it_fits", [] {
		auto c = make_context_with_font();
		auto t = make_resized_text(c, {.params{.specific{.multiline = true}}}, many_words, 100000, 40);
		tst::check_eq(t.get().get_lines().size(), size_t(1), SL);
		tst::check_eq(t.get().is_ellipsized(), false, SL);
	});

	suite.add("multiline__wraps_into_multiple_lines", [] {
		auto c = make_context_with_font();
		auto t = make_resized_text(c, {.params{.specific{.multiline = true}}}, many_words, 40, 40);
		tst::check(t.get().get_lines().size() > size_t(1), SL);
		tst::check_eq(t.get().is_ellipsized(), false, SL);
		// every line must be a contiguous substring of the widget's text (words are not broken)
		auto full = std::u32string_view(t.get().get_string().get());
		for (const auto& li : t.get().get_lines()) {
			const auto& line = li.span;
			const auto offset = line.data() - full.data();
			tst::check(0 <= offset, SL);
			tst::check(full.substr(offset, line.size()) == line, SL);
		}
	});

	suite.add("multiline__single_long_word_is_one_ellipsized_line", [] {
		auto c = make_context_with_font();
		auto t = make_resized_text(c, {.params{.specific{.multiline = true}}}, long_word, 40, 40);
		// a single word cannot be split, so it stays on one (truncated) line
		tst::check_eq(t.get().get_lines().size(), size_t(1), SL);
		tst::check_eq(t.get().is_ellipsized(), true, SL);
	});

	suite.add("single_line__very_narrow_keeps_one_char_and_ellipsis", [] {
		auto c = make_context_with_font();
		// A width so small that not even one character plus the ellipsis fits: the layout
		// must still keep at least one (possibly clipped) character before the ellipsis.
		auto t = make_resized_text(c, {}, long_word, 1, 40);
		tst::check_eq(t.get().get_lines().size(), size_t(1), SL);
		tst::check_eq(t.get().is_ellipsized(), true, SL);
		const auto& line = t.get().get_lines().front().span;
		tst::check(line.size() >= 1, SL);
		auto full = std::u32string_view(t.get().get_string().get());
		tst::check(full.substr(0, line.size()) == line, SL);
	});
});
} // namespace
