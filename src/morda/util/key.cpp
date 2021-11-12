/*
morda - GUI framework

Copyright (C) 2012-2021  Ivan Gagis <igagis@gmail.com>

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

#include "key.hpp"

using namespace morda;

key_modifier morda::to_key_modifier(morda::key key){
    switch(key){
        case key::left_shift:
            return key_modifier::left_shift;
        case key::right_shift:
            return key_modifier::right_shift;
        case key::left_alt:
            return key_modifier::left_alt;
        case key::right_alt:
            return key_modifier::right_alt;
        case key::left_control:
            return key_modifier::left_control;
        case key::right_control:
            return key_modifier::right_control;
        case key::left_command:
            return key_modifier::left_command;
        case key::right_command:
            return key_modifier::right_command;
        default:
            return key_modifier::unknown;
    }
}
