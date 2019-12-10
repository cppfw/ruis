#pragma once

#include "ChoiceGroup.hpp"

namespace morda{
class Tabs :
		virtual public Widget,
		private ChoiceGroup
{
	std::shared_ptr<ResImage> filler;
	std::shared_ptr<const ResImage::QuadTexture> fillerTexture;

public:
	Tabs(const stob::Node* chain);

	Tabs(const Tabs&) = delete;
	Tabs& operator=(const Tabs&) = delete;

	void setFiller(std::shared_ptr<ResImage> filler);

	morda::Vec2r measure(const morda::Vec2r& quotum) const override;

	void lay_out() override;

	void render(const morda::Matr4r& matrix) const override;
};
}
