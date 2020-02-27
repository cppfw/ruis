#pragma once

#include "choice_group.hpp"

namespace morda{
class Tabs :
		virtual public widget,
		private choice_group
{
	std::shared_ptr<res_image> filler;
	std::shared_ptr<const res_image::texture> fillerTexture;

public:
	Tabs(std::shared_ptr<morda::context> c, const puu::forest& desc);

	Tabs(const Tabs&) = delete;
	Tabs& operator=(const Tabs&) = delete;

	void setFiller(std::shared_ptr<res_image> filler);

	morda::Vec2r measure(const morda::Vec2r& quotum) const override;

	void lay_out() override;

	void render(const morda::Matr4r& matrix) const override;
};
}
