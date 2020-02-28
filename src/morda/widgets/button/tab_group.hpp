#pragma once

#include "choice_group.hpp"

namespace morda{

class tab_group :
		virtual public widget,
		private choice_group
{
	std::shared_ptr<res_image> filler;
	std::shared_ptr<const res_image::texture> fillerTexture;

public:
	tab_group(std::shared_ptr<morda::context> c, const puu::forest& desc);

	tab_group(const tab_group&) = delete;
	tab_group& operator=(const tab_group&) = delete;

	void set_filler(std::shared_ptr<res_image> filler);

	morda::Vec2r measure(const morda::Vec2r& quotum) const override;

	void lay_out() override;

	void render(const morda::matrix4& matrix) const override;
};

}
