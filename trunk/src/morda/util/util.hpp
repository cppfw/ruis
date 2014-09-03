/* The MIT License:

Copyright (c) 2012-2014 Ivan Gagis <igagis@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// Home page: http://morda.googlecode.com

/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Vector2.hpp"
#include "Rectangle2.hpp"

#include <tuple>

#include <stob/dom.hpp>
#include <ting/math.hpp>


namespace morda{



/**
 * @brief Parse chain of 2 STOB nodes as Vec2r.
 * @param chain - chain of at least two nodes holding Vec2r values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with zeros.
 * @return parsed Vec2r.
 */
morda::Vec2r Vec2rFromSTOB(const stob::Node* chain);

/**
 * @brief Parse chain of 4 STOB nodes as Rect2r.
 * @param chain - chain of at least four nodes holding Rect2r values.
 *                If there are less than 4 nodes in the chain then the rest of
 *                rectangle components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting rectangle will be filled with zeros.
 * @return parsed Rect2r.
 */
morda::Rect2r Rect2rFromSTOB(const stob::Node* chain);



/**
 * @brief Parse dimension Vec2r from STOB.
 * Same as Vec2rFromSTOB but using DimValue() to parse values.
 * @param chain - chain of at least two nodes holding Vec2r values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with zeroes.
 * @return parsed Vec2r.
 */
morda::Vec2r DimVec2r(const stob::Node* chain);


morda::Vector2<bool> TwoBoolsFromSTOB(const stob::Node* chain);


/**
 * @brief Round each component of Vec2r.
 * Call ting::math::Round() for each component of given Vec2r.
 * @param v - Vec2r to round.
 * @return Rounded Vec2r.
 */
inline morda::Vec2r RoundVec(const Vec2r& v){
	return Vec2r(ting::math::Round(v.x), ting::math::Round(v.y));
}



/**
 * @brief Parse dimension value.
 * Parses value of 'pos' or 'dim' property from STOB.
 * In case the value is given in millimeters it will do the conversion.
 * @param n - stob node holding the value.
 * @return Parsed value in pixels.
 */
float DimValue(const stob::Node& n);



/**
 * @brief Resolve includes in STOB document.
 * @param fi - file interface set to the original STOB document. Because resolving include paths is done relatively to original STOB document path.
 * @param begin - first node of the original STOB document.
 * @return New first node and pointer to the last child node of the script.
 */
std::tuple<std::unique_ptr<stob::Node>, stob::Node*> ResolveIncludes(ting::fs::File& fi, std::unique_ptr<stob::Node> begin);



inline bool NodeHoldsFractionValue(const stob::Node& node)NOEXCEPT{
	size_t len = node.ValueLength();
	return len != 0 && node.Value()[len - 1] == '%';
}


}//~namespace
