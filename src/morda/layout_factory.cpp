#include "layout_factory.hpp"

using namespace morda;

void layout_factory::add_factory(std::string name, factory_type&& factory){
    this->factories.emplace(std::move(name), std::move(factory));
}

utki::shared_ref<layout> layout_factory::create(std::string_view name, const tml::forest& desc){
    auto i = this->factories.find(name);
    if(i == this->factories.end()){
        std::stringstream ss;
        ss << "layout_factory::create(" << name << "): layout factory not found";
        throw std::logic_error(ss.str());
    }

    return i->second(desc);
}
