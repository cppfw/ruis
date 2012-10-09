/* The MIT License:

Copyright (c) 2012 Ivan Gagis <igagis@gmail.com>

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



namespace morda{



class Gravity{
public:
	enum E_Gravity{
		LEFT,
		BOTTOM = LEFT,
		CENTER,
		RIGHT,
		TOP = RIGHT
	};
	
	E_Gravity hori;
	E_Gravity vert;
	
	inline Gravity(){}
	
	inline Gravity(E_Gravity hori, E_Gravity vert) :
			hori(hori),
			vert(vert)
	{}
	
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
     * @param prop - prop properties STOB node.
     * @return Parsed Gravity object.
     */
	static Gravity FromPropLayout(const stob::Node& prop)throw();
	
	/**
	 * @brief Default gravity values.
     * @return Gravity object initialized to default gravity values.
     */
	static Gravity Default()throw(){
		return Gravity(CENTER, CENTER);
	}
};



}//~namespace
