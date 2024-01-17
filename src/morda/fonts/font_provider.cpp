/*
morda - GUI framework

Copyright (C) 2012-2023  Ivan Gagis <igagis@gmail.com>

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

#include "font_provider.hpp"

using namespace morda;

utki::shared_ref<const font> font_provider::get(real size){
    auto i = this->cache.find(size);
    if(i != this->cache.end()){
        if(auto f = i->second.lock()){
            return utki::shared_ref<const font>(std::move(f));
        }else{
            this->cache.erase(i);
        }
    }

    auto f = this->create(size);

    this->cache.insert(std::make_pair(size, utki::make_weak(f)));

    return f;
}
