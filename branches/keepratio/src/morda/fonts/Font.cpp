#include "Font.hpp"


using namespace morda;


real morda::Font::RenderString(PosTexShader& shader, const morda::Matr4r& matrix, ting::utf8::Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->RenderStringInternal(shader, matrix, ting::Buffer<std::uint32_t>(&*buf.begin(), ting::utf8::FillBuffer(buf, str)));
}



real morda::Font::StringAdvance(ting::utf8::Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->StringAdvanceInternal(ting::Buffer<std::uint32_t>(&*buf.begin(), ting::utf8::FillBuffer(buf, str)));
}



morda::Rect2r morda::Font::StringBoundingBox(ting::utf8::Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->StringBoundingBoxInternal(ting::Buffer<std::uint32_t>(&*buf.begin(), ting::utf8::FillBuffer(buf, str)));
}
