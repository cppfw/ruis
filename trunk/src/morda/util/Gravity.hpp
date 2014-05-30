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

#include <stob/dom.hpp>

#include "../util/Vector2.hpp"
#include "../widgets/Widget.hpp"
#include "../widgets/PaddedWidget.hpp"


namespace morda{



class Gravity : public morda::Vec2f{
public:
	/**
	 * @brief Constructor.
	 * Creates uninitialized Gravity object.
     */
	inline Gravity(){}
	
	
	/**
	 * @brief Constructor.
	 * Creates a Gravity object initialized to given values.
	 * The values should be from range [0:1].
     * @param gravity - gravity values.
     */
	inline Gravity(const Vec2f& gravity) :
			Vec2f(gravity)
	{}
	
	/**
	 * @brief Get position for widget.
	 * Basing on gravity calculates the position for given widget within its parent.
	 * Widget and its parent should have the correct size already set.
	 * Parent's padding is taken into account.
     * @param w - widget to calculate the position for.
     * @return The position for the widget.
     */
	Vec2f PosForWidget(const Widget& w)const throw();
	
	/**
	 * @brief Get position for rectangle within given padded widget.
	 * Basing on gravity calculates the position for rectangle of given dimensions within
	 * the given padded widget.
	 * Widget should have the correct size already set.
	 * Takes widget padding into account.
     * @param w - padded widget within which the rectangle is to be placed.
     * @param dim - dimensions of the rectangle.
     * @return Position for the rectangle within widget.
     */
	Vec2f PosForRect(const PaddedWidget& w, const Vec2f& dim)const throw();
	
	/**
	 * @brief Get position for rectangle within given parent rectangle.
	 * Basing on gravity calculates the position for rectangle of given dimensions within
	 * the given parent rectangle.
     * @param parentDim - dimensions of parent rectangle.
	 * @param padding - padding to take into account.
     * @param dim - dimensions of the rectangle to place.
     * @return Position for the rectangle within parent rectangle.
     */
	Vec2f PosForRect(const Vec2f& parentDim, const LeftBottomRightTop& padding, const Vec2f& dim)const throw();
	
	/**
	 * @brief Parse the gravity property from STOB.
	 * Takes STOB node of the following form:
	 * @code
	 * gravity{left center}
	 * @endcode
	 * where first child node can have values: "left", "right", "center";
	 * and second child node can have values: "top", "bottom", "center";
	 * If one or both children are absent then default value is used which is "center".
	 * Value of the root node does not matter, it is ignored.
     * @param gravity - "gravity" STOB node.
     * @return Parsed Gravity object.
     */
	static Gravity FromSTOB(const stob::Node& gravity)throw();
	
	/**
	 * @brief Parse the gravity property from layout STOB.
	 * Parses gravity property from given layout properties.
	 * @code
	 * layout{
	 *     //...
	 *     gravity{right top}
	 *     //...
	 * }
	 * @endcode
	 * Value of the root node does not matter, it is ignored.
     * @param layout - layout properties STOB node.
     * @return Parsed Gravity object.
     */
	static Gravity FromLayout(const stob::Node& layout)throw();
	
	/**
	 * @brief Parse the gravity property from prop STOB.
	 * Parses gravity property from given prop properties.
	 * @code
	 * prop{
	 *     //...
	 *     layout{
	 *         //...
	 *         gravity{right top}
	 *         //...
	 *     }
	 *     //...
	 * }
	 * @endcode
	 * Value of the root node does not matter, it is ignored.
     * @param prop - prop properties STOB node.
     * @return Parsed Gravity object.
     */
	static Gravity FromPropLayout(const stob::Node& prop)throw();
	
	/**
	 * @brief Default gravity values.
     * @return Gravity object initialized to default gravity values.
     */
	static Gravity Default()throw(){
		return Gravity(Vec2f(0.5f));
	}
};



}//~namespace
