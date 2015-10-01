#include "Font.hpp"


using namespace morda;


real morda::Font::RenderString(PosTexShader& shader, const morda::Matr4r& matrix, unikod::Utf8Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->RenderStringInternal(shader, matrix, utki::wrapBuf(&*buf.begin(), unikod::fillBuffer(utki::wrapBuf(buf), str)));
}



real morda::Font::StringAdvance(unikod::Utf8Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->StringAdvanceInternal(utki::wrapBuf(&*buf.begin(), unikod::fillBuffer(utki::wrapBuf(buf), str)));
}



morda::Rectr morda::Font::StringBoundingBox(unikod::Utf8Iterator str)const{
	std::array<std::uint32_t, 4096> buf;
	return this->StringBoundingBoxInternal(utki::Buf<std::uint32_t>(&*buf.begin(), unikod::fillBuffer(utki::wrapBuf(buf), str)));
}
