#pragma once

#include "choice_group.hpp"

#include "../../res/image.hpp"

namespace morda{

class tab_group :
		virtual public widget,
		private choice_group
{
	std::shared_ptr<res::image> filler;
	std::shared_ptr<const res::image::texture> fillerTexture;

public:
	tab_group(std::shared_ptr<morda::context> c, const treeml::forest& desc);

	tab_group(const tab_group&) = delete;
	tab_group& operator=(const tab_group&) = delete;

	void set_filler(std::shared_ptr<res::image> filler);

	morda::vector2 measure(const morda::vector2& quotum) const override;

	void lay_out() override;

	void render(const morda::matrix4& matrix) const override;
};

}
