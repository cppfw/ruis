/**
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <ting/util.hpp>
#include "Vector3.hpp"

namespace morda{

template <class T> class Mesh{
public:
	Mesh(){}
	Mesh(const Mesh&) = default;
	
	virtual ~Mesh()noexcept;

private:

};

}
