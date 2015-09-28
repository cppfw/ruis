/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <utki/Buf.hpp>
#include <unikod/utf8.hpp>

#include "../util/Matrix4.hpp"
#include "../util/Rectangle2.hpp"

#include "../shaders/PosTexShader.hpp"

#include <string>

namespace morda{


class Font{
	Font(const Font&);
	Font& operator=(const Font&);
	
protected:
	//Bounding box holds the dimensions of the largest loaded glyph.
	morda::Rect2r boundingBox;
	
	Font(){}
	
	virtual real RenderStringInternal(PosTexShader& shader, const morda::Matr4r& matrix, ting::Buffer<const std::uint32_t> utf32str)const = 0;
	
	virtual real StringAdvanceInternal(ting::Buffer<const std::uint32_t> utf32str)const = 0;
	
	virtual morda::Rect2r StringBoundingBoxInternal(ting::Buffer<const std::uint32_t> utf32str)const = 0;
public:
	virtual ~Font()noexcept{}
	
	//renders the string, returns resulting string advance
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, ting::utf8::Iterator str)const;
	
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, ting::Buffer<const std::uint32_t> utf32str)const{
		return this->RenderStringInternal(shader, matrix, utf32str);
	}
	
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, const char* str)const{
		return this->RenderString(shader, matrix, ting::utf8::Iterator(str));
	}
	
	real RenderString(PosTexShader& shader, const morda::Matr4r& matrix, const std::string& str)const{
		return this->RenderString(shader, matrix, str.c_str());
	}
	
	
	
	real StringAdvance(ting::utf8::Iterator str)const;
	
	real StringAdvance(ting::Buffer<const std::uint32_t> utf32str)const{
		return this->StringAdvanceInternal(utf32str);
	}
	
	real StringAdvance(const char* str)const{
		return this->StringAdvance(ting::utf8::Iterator(str));
	}
	
	real StringAdvance(const std::string& str)const{
		return this->StringAdvance(str.c_str());
	}
	
	
	
	virtual real CharAdvance(std::uint32_t c)const = 0;
	
	
	
	morda::Rect2r StringBoundingBox(ting::utf8::Iterator str)const;
	
	morda::Rect2r StringBoundingBox(ting::Buffer<const std::uint32_t> utf32str)const{
		return this->StringBoundingBoxInternal(utf32str);
	}
	
	morda::Rect2r StringBoundingBox(const char* str)const{
		return this->StringBoundingBox(ting::utf8::Iterator(str));
	}
	
	morda::Rect2r StringBoundingBox(const std::string& str)const{
		return this->StringBoundingBox(str.c_str());
	}
	
	const morda::Rect2r& BoundingBox()const noexcept{
		return this->boundingBox;
	}
	
private:

};

}
