#include "Texture2D.hpp"



using namespace morda;




void Texture2D::Constructor(Vec2ui d, unsigned numChannels, ting::Buffer<const std::uint8_t> data, Render::ETexFilter minFilter, Render::ETexFilter magFilter) {
	this->dim = d.ConvertTo<real>();

	this->tex = Render::create2DTexture(d, numChannels, data, minFilter, magFilter);
}
