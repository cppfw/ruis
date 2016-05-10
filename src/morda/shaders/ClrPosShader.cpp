/* 
 * File:   ClrPosShader.cpp
 * Author: ivan
 * 
 * Created on May 10, 2016, 2:03 PM
 */

#include "ClrPosShader.hpp"


using namespace morda;



ClrPosShader::ClrPosShader() :
		Shader(
				R"qwertyuiop(
						UNIFORM_BEGIN
							UNIFORM(MAT4F, matrix)
						UNIFORM_END

						ATTRIB_BEGIN
							ATTRIB(VEC4F, pos)
							ATTRIB(VEC4F, clr)
						ATTRIB_END

						VARYING_BEGIN
							VARYING(VEC4F, color_varying)
							VARYING_POS
						VARYING_END

						VERTEX_MAIN_BEGIN
							OUT_POS = matrix * pos;
							OUT(color_varying) = clr;
						VERTEX_MAIN_END
					)qwertyuiop",
				R"qwertyuiop(		
						VARYING_BEGIN
							VARYING(VEC4F, color_varying)
						VARYING_END
		
						FRAG_MAIN_BEGIN
							OUT_FRAGCOLOR = color_varying;
						FRAG_MAIN_END
					)qwertyuiop"
			),
		colorAttr(this->getAttribute("clr"))
{
}

