#include "Font.hpp"

#include <ting/Buffer.hpp>

using namespace morda;


float Font::RenderString(const morda::Matr4f& matrix, ting::utf8::Iterator str)const{
	ting::StaticBuffer<ting::u32, 4096> buf;

	ting::u32* p = buf.Begin();
	for(; p != buf.End() && str.IsNotEnd(); ++p, ++str){
		*p = str.Char();
	}
		
	ASSERT(buf.Overlaps(p) || p == buf.End())
	
	return this->RenderStringInternal(matrix, ting::Buffer<const ting::u32>(buf.Begin(), p - buf.Begin()));
}



float Font::StringAdvance(ting::utf8::Iterator str)const{
	ting::StaticBuffer<ting::u32, 4096> buf;

	ting::u32* p = buf.Begin();
	for(; p != buf.End() && str.IsNotEnd(); ++p, ++str){
		*p = str.Char();
	}
		
	ASSERT(buf.Overlaps(p) || p == buf.End())
	
	return this->StringAdvanceInternal(ting::Buffer<const ting::u32>(buf.Begin(), p - buf.Begin()));
}



morda::Rect2f Font::StringBoundingBox(ting::utf8::Iterator str)const{
	ting::StaticBuffer<ting::u32, 4096> buf;

	ting::u32* p = buf.Begin();
	for(; p != buf.End() && str.IsNotEnd(); ++p, ++str){
		*p = str.Char();
	}
		
	ASSERT(buf.Overlaps(p) || p == buf.End())
	
	return this->StringBoundingBoxInternal(ting::Buffer<const ting::u32>(buf.Begin(), p - buf.Begin()));
}
