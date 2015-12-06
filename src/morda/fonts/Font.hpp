/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include <string>

#include <utki/Buf.hpp>
#include <unikod/utf8.hpp>

#include <kolme/Matrix4.hpp>
#include <kolme/Rectangle.hpp>

#include "../shaders/PosTexShader.hpp"

#include "../config.hpp"

namespace morda{


class Font{
	Font(const Font&);
	Font& operator=(const Font&);
	
protected:
	//Bounding box holds the dimensions of the largest loaded glyph.
	morda::Rectr boundingBox_var;
	
	Font(){}
	
	virtual real renderStringInternal(PosTexShader& shader, const morda::Matr4r& matrix, const std::u32string& str)const = 0;
	
	virtual real stringAdvanceInternal(const std::u32string& str)const = 0;
	
	virtual morda::Rectr stringBoundingBoxInternal(const std::u32string& str)const = 0;
public:
	virtual ~Font()noexcept{}
	
	//renders the string, returns resulting string advance
	real renderString(PosTexShader& shader, const morda::Matr4r& matrix, unikod::Utf8Iterator str)const{
		return this->renderStringInternal(shader, matrix, unikod::toUtf32(str));
	}
	
	real renderString(PosTexShader& shader, const morda::Matr4r& matrix, const std::u32string& str)const{
		return this->renderStringInternal(shader, matrix, str);
	}
	
	real renderString(PosTexShader& shader, const morda::Matr4r& matrix, const char* str)const{
		return this->renderString(shader, matrix, unikod::Utf8Iterator(str));
	}
	
	real renderString(PosTexShader& shader, const morda::Matr4r& matrix, const std::string& str)const{
		return this->renderString(shader, matrix, str.c_str());
	}
	
	
	
	real stringAdvance(unikod::Utf8Iterator str)const{
		return this->stringAdvanceInternal(unikod::toUtf32(str));
	}
	
	real stringAdvance(const std::u32string& str)const{
		return this->stringAdvanceInternal(str);
	}
	
	real stringAdvance(const char* str)const{
		return this->stringAdvance(unikod::Utf8Iterator(str));
	}
	
	real stringAdvance(const std::string& str)const{
		return this->stringAdvance(str.c_str());
	}
	
	
	
	virtual real charAdvance(std::uint32_t c)const = 0;
	
	
	
	morda::Rectr stringBoundingBox(unikod::Utf8Iterator str)const{
		return this->stringBoundingBoxInternal(unikod::toUtf32(str));
	}
	
	morda::Rectr stringBoundingBox(const std::u32string& str)const{
		return this->stringBoundingBoxInternal(str);
	}
	
	morda::Rectr stringBoundingBox(const char* str)const{
		return this->stringBoundingBox(unikod::Utf8Iterator(str));
	}
	
	morda::Rectr stringBoundingBox(const std::string& str)const{
		return this->stringBoundingBox(str.c_str());
	}
	
	const morda::Rectr& boundingBox()const noexcept{
		return this->boundingBox_var;
	}
	
private:

};

}
