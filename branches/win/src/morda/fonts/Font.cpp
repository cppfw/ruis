#include "Font.hpp"

#include <ting/Buffer.hpp>

using namespace morda;


float Font::RenderString(const morda::Matr4f& matrix, ting::utf8::Iterator str)const{
	ting::StaticBuffer<ting::u32, 4096> buf;
	return this->RenderStringInternal(matrix, ting::Buffer<const ting::u32>(buf.Begin(), ting::utf8::FillBuffer(buf, str)));
}



float Font::StringAdvance(ting::utf8::Iterator str)const{
	ting::StaticBuffer<ting::u32, 4096> buf;
	return this->StringAdvanceInternal(ting::Buffer<const ting::u32>(buf.Begin(), ting::utf8::FillBuffer(buf, str)));
}



morda::Rect2f Font::StringBoundingBox(ting::utf8::Iterator str)const{
	ting::StaticBuffer<ting::u32, 4096> buf;
	return this->StringBoundingBoxInternal(ting::Buffer<const ting::u32>(buf.Begin(), ting::utf8::FillBuffer(buf, str)));
}
