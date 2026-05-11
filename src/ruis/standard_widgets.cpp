#include "standard_widgets.hpp"

#include "res/tml.hpp"

using namespace std::string_view_literals;

using namespace ruis;

void ruis::init_standard_widgets(
	ruis::context& context, //
	const fsif::file& fi,
	theme th
)
{
	// mount default resource pack

	std::vector<std::string> paths;

	if (!fi.path().empty()) {
		paths.push_back(fi.path());
	}

	paths.emplace_back("ruis_res/");

#if (M_OS == M_OS_LINUX && M_OS_NAME != M_OS_NAME_ANDROID) || (M_OS == M_OS_MACOSX && M_OS_NAME != M_OS_NAME_IOS) || \
	(M_OS == M_OS_UNIX)

	unsigned soname =
#	include "../soname.txt"
		;

	paths.push_back(utki::cat("/usr/local/share/ruis/res"sv, soname));
	paths.push_back(utki::cat("/usr/share/ruis/res"sv, soname));
#endif

	bool mounted = false;
	for (const auto& s : paths) {
		try {
			fi.set_path(s);
			context.loader().mount_res_pack(fi);
		} catch (std::runtime_error&) {
			continue;
		}

		mounted = true;
		break;
	}

	if (!mounted) {
		throw std::runtime_error("init_standard_widgets(): could not mount default resource pack");
	}

	auto theme_resource_id = [&]() {
		switch (th) {
			using enum theme;
			default:
			case dark:
				return "ruis_tml_theme_dark"sv;
			case light:
				return "ruis_tml_theme_light"sv;
		}
	}();

	auto style_res = context.loader().load<ruis::res::tml>(theme_resource_id);
	context.style().set(utki::make_shared<style_sheet>(style_res.get().forest));
}
