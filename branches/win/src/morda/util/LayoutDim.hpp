/* The MIT License:

Copyright (c) 2014 Ivan Gagis

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

#include "../widgets/Widget.hpp"
#include "../widgets/PaddedWidget.hpp"


namespace morda{



class LayoutDim{
public:
	static const char* D_Dim()throw(){
		return "dim";
	}

	enum E_Unit{
		PIXEL,
		FRACTION, //means that the value is a fraction from parent size
		MIN //means wrap content, value should be taken from min size
	};

	struct Value{
		float value;
		E_Unit unit;
	}x, y;

	Value& operator[](size_t i)throw(){
		ASSERT(i < 2)
		return reinterpret_cast<Value*>(this)[i];
	}

	const Value& operator[](size_t i)const throw(){
		ASSERT(i < 2)
		return reinterpret_cast<const Value*>(this)[i];
	}

	/**
	 * @brief Get actual dimensions for given widget.
	 * Get resulting dimensions for given Widget basing on dimension description
	 * provided by this object. 
	 * @param parent - widget's parent.
	 * @param w - widget to get dimensions for.
	 * @return Resulting dimensions.
	 */
	Vec2f ForWidget(const PaddedWidget& parent, const Widget& w)const throw();

	/**
	 * @brief Parse from STOB.
	 * Parse from STOB of the form:
	 * @code
	 * dim{min 13%}
	 * @endcode
	 * The value of the root node does not matter, it is ignored.
	 * @param node - dim node.
	 * @return Parsed Dim object.
	 */
	static LayoutDim FromSTOB(const stob::Node& node)throw();

	/**
	 * @brief Parse from STOB.
	 * Parse from STOB of the form:
	 * @code
	 * dim{min 13%}
	 * @endcode
	 * The value of the root node does not matter, it is ignored.
	 * @param node - dim node. If 0 pointer is passed then return default Dim.
	 * @return Parsed Dim object.
	 */
	static LayoutDim FromSTOB(const stob::Node* node)throw(){
		if(!node){
			return Default();
		}
		return FromSTOB(*node);
	}

	/**
	 * @brief Parse from layout properties STOB.
	 * Parse from STOB of the form:
	 * @code
	 * layout{
	 *     //...
	 *     dim{134 100%}
	 *     //...
	 * }
	 * @endcode
	 * The value of the root node does not matter, it is ignored.
	 * @param layout - layout node.
	 * @return Parsed Dim object.
	 */
	static LayoutDim FromLayout(const stob::Node& layout)throw();

	/**
	 * @brief Parse from properties STOB.
	 * Parse from STOB of the form:
	 * @code
	 * prop{
	 *     //...
	 *     layout{
	 *         //...
	 *         dim{134 100%}
	 *         //...
	 *     }
	 *     //...
	 * }
	 * @endcode
	 * The value of the root node does not matter, it is ignored.
	 * @param prop - prop node.
	 * @return Parsed Dim object.
	 */
	static LayoutDim FromPropLayout(const stob::Node& prop)throw();

	static LayoutDim Default()throw(){
		LayoutDim ret;
		ret.x.unit = MIN;
		ret.y.unit = MIN;
		return ret;
	}
};


}
