#include "Texture2D.hpp"

#include "../Exc.hpp"


using namespace morda;




void Texture2D::Constructor(kolme::Vec2ui d, unsigned numChannels, const utki::Buf<std::uint8_t> data, Render::ETexFilter minFilter, Render::ETexFilter magFilter) {
	this->dim_var = d.to<real>();

	this->tex = Render::create2DTexture(d, numChannels, data, minFilter, magFilter);
}



Texture2D::Texture2D(unsigned width, std::vector<std::uint32_t> rgbaPixels, Render::ETexFilter minFilter, Render::ETexFilter magFilter){
	if(width == 0){
		throw morda::Exc("Texture2D: zero size width requested");
	}
	this->Constructor(
			kolme::Vec2ui(width, unsigned(rgbaPixels.size() / width)),
			4,
			utki::Buf<std::uint8_t>(
					reinterpret_cast<std::uint8_t*>(&*rgbaPixels.begin()),
					rgbaPixels.size() * sizeof(*rgbaPixels.begin())
				),
			minFilter,
			magFilter
		);
}
