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
