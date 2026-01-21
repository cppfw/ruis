/*
ruis - GUI framework

Copyright (C) 2012-2026  Ivan Gagis <igagis@gmail.com>

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

#include "list_page.hpp"

namespace{
class list_page : public ruis::page{
public:
    list_page(utki::shared_ref<ruis::context> c) :
        ruis::widget(std::move(c), {}, {}),
        ruis::page(this->context, {})
    {}
};

}

utki::shared_ref<ruis::page> make_list_page(utki::shared_ref<ruis::context> c)
{
    return utki::make_shared<list_page>(std::move(c));
}
