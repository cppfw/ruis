/*
morda GUI framework

Copyright (C) 2021  Ivan Gagis <igagis@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#include "vertex_array.hpp"

#include <utki/debug.hpp>

using namespace morda;

vertex_array::vertex_array(decltype(buffers)&& buffers, std::shared_ptr<morda::index_buffer> indices, mode rendering_mode) :
		buffers(buffers),
		indices(std::move(indices)),
		rendering_mode(rendering_mode)
{
	if(this->buffers.empty()){
		throw std::invalid_argument("no vertex buffers passed to vertex array");
	}
	ASSERT(this->buffers.front())
	auto s = this->buffers.front()->size;
	for(auto& b : this->buffers){
		if(b->size != s){
			throw std::invalid_argument("vertex buffers of different size passed in to vertex array, should all be of the same size");
		}
	}
}
