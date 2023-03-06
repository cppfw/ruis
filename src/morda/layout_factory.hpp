#pragma once

#include <map>
#include <functional>

#include <utki/shared_ref.hpp>
#include <treeml/tree.hpp>

namespace morda{

class layout;

class layout_factory{
public:
    using factory_type = std::function<utki::shared_ref<layout>(const tml::forest& desc)>;
private:
    std::map<std::string, factory_type, std::less<>> factories;
public:

    void add_factory(std::string name, factory_type&& factory);

    utki::shared_ref<layout> create(std::string_view name, const tml::forest& desc);

};

}

// include definitions for forward declared classes
#include "layout.hpp"
