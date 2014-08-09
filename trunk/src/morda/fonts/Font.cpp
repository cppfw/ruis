#include "Font.hpp"

#include <ting/ArrayAdaptor.hpp>

using namespace morda;


float Font::RenderString(const morda::Matr4f& matrix, ting::utf8::Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->RenderStringInternal(matrix, ting::ArrayAdaptor<std::uint32_t>(buf.begin(), ting::utf8::FillBuffer(buf, str)));
}



float Font::StringAdvance(ting::utf8::Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->StringAdvanceInternal(ting::ArrayAdaptor<std::uint32_t>(buf.begin(), ting::utf8::FillBuffer(buf, str)));
}



morda::Rect2f Font::StringBoundingBox(ting::utf8::Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->StringBoundingBoxInternal(ting::ArrayAdaptor<std::uint32_t>(buf.begin(), ting::utf8::FillBuffer(buf, str)));
}
