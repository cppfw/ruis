#include "tree_view_window.hpp"

#include <ruis/widget/group/tree_view.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/default_style.hpp>

using namespace std::string_literals;

using namespace ruis::length_literals;

namespace m{
using namespace ruis::make;
}

namespace{
class tree_view_items_provider : public ruis::tree_view::provider{
    tml::forest root;
public:

    // NOLINTNEXTLINE(modernize-pass-by-value)
    tree_view_items_provider(utki::shared_ref<ruis::context> context) :
            provider(std::move(context))
    {
        this->root = tml::read(R"qwertyuiop(
                root1{
                    subroot1{
                        subsubroot1
                        subsubroot2
                        subsubroot3
                        subsubroot4
                    }
                    subroot2
                    subroot3{
                        subsubroot0
                        subsubroot1{
                            subsubsubroot1
                            subsubsubroot2
                        }
                        subsubroot2
                    }
                }
                root22{
                    subsubroot1
                    subsubroot2{
                        trololo
                        "hello world!"
                    }
                }
                root333
                root4444
            )qwertyuiop");
    }

    tree_view_items_provider(const tree_view_items_provider&) = delete;
    tree_view_items_provider& operator=(const tree_view_items_provider&) = delete;

    tree_view_items_provider(tree_view_items_provider&&) = delete;
    tree_view_items_provider& operator=(tree_view_items_provider&&) = delete;

    ~tree_view_items_provider()override = default;

private:
    std::vector<size_t> selected_item;

    unsigned newItemNumber = 0;

    std::string generate_new_item_value(){
        std::stringstream ss;
        ss << "newItem" << newItemNumber;
        ++newItemNumber;
        return ss.str();
    }

public:
    void insert_before(){
        if(this->selected_item.size() == 0){
            return;
        }

        tml::forest* list = &this->root;
        tml::forest* parent_list = nullptr;

        for(auto& i : this->selected_item){
            parent_list = list;
            list = &(*list)[i].children;
        }

        if(!parent_list){
            return;
        }

        parent_list->insert(utki::next(parent_list->begin(), this->selected_item.back()), tml::leaf(this->generate_new_item_value()));

        this->notify_item_add(utki::make_span(this->selected_item));
        ++this->selected_item.back();
    }

    void insert_after(){
        if(this->selected_item.size() == 0){
            return;
        }

        tml::forest* list = &this->root;
        tml::forest* parent_list = nullptr;

        for(auto& i : this->selected_item){
            parent_list = list;
            list = &(*list)[i].children;
        }

        if(!parent_list){
            return;
        }

        parent_list->insert(utki::next(parent_list->begin(), this->selected_item.back() + 1), tml::leaf(this->generate_new_item_value()));

        ++this->selected_item.back();
        this->notify_item_add(utki::make_span(this->selected_item));
        --this->selected_item.back();
    }

    void insert_child(){
        if(this->selected_item.size() == 0){
            return;
        }

        tml::forest* list = &this->root;

        for(auto& i : this->selected_item){
            list = &(*list)[i].children;
        }

        list->emplace_back(this->generate_new_item_value());

        this->selected_item.push_back(list->size() - 1);
        this->notify_item_add(utki::make_span(this->selected_item));
        this->selected_item.pop_back();
    }

    utki::shared_ref<ruis::widget> get_widget(utki::span<const size_t> path, bool is_collapsed)override{
        ASSERT(!path.empty())

        auto list = &this->root;
        decltype(list) parent_list = nullptr;

        tml::tree* n = nullptr;

        for(const auto& i : path){
            n = &(*list)[i];
            parent_list = list;
            list = &n->children;
        }

        auto ret = ruis::make::row(this->get_context(), {});

        {
            // clang-format off
            auto v = m::pile(this->get_context(),
                {},
                {
                    m::rectangle(this->get_context(),
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            },
                            .widget_params{
                                .id = "selection"s,
                                .visible = false
                            },
                            .color_params{
                                .color = ruis::style::color_highlight
                            }
                        }
                    ),
                    m::text(this->get_context(),
                        {
                            .widget_params{
                                .id = "value"s
                            }
                        }
                    ),
                    m::mouse_proxy(this->get_context(),
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            },
                            .widget_params{
                                .id = "mouse_proxy"s
                            }
                        }
                    )
                }
            );
            // clang-format on

            {
                auto value = v.get().try_get_widget_as<ruis::text>("value");
                ASSERT(value)
                value->set_text(
                        n->value.string // NOLINT(clang-analyzer-core.CallAndMessage): due to ASSERT(!path.empty()) in the beginning of the function 'n' is not nullptr
                    );
            }
            {
                auto& color_label = v.get().get_widget_as<ruis::rectangle>("selection");

                color_label.set_visible(utki::deep_equals(path, utki::make_span(this->selected_item)));

                auto mp = v.get().try_get_widget_as<ruis::mouse_proxy>("mouse_proxy");
                ASSERT(mp)
                mp->mouse_button_handler = [this, path = utki::make_vector(path)](ruis::mouse_proxy&, const ruis::mouse_button_event& e) -> bool{
                    if(!e.is_down || e.button != ruis::mouse_button::left){
                        return false;
                    }

                    this->selected_item = path;
#ifdef DEBUG
                    LOG([](auto&o){o << " selected item = ";})
                    for(const auto& k : this->selected_item){
                        LOG([&](auto&o){o << k << ", ";})
                    }
                    LOG([](auto&o){o << std::endl;})
#endif
                    this->notify_item_change();

                    return true;
                };
            }

            ret.get().push_back(v);
        }

        {
            // clang-format off
            auto b = m::push_button(this->get_context(),
                {},
                {
                    m::rectangle(this->get_context(),
                        {
                            .layout_params{
                                .dims{ruis::length::make_pp(5), ruis::length::make_pp(2)}
                            },
                            .color_params{
                                .color = 0xff0000ff
                            }
                        }
                    )
                }
            );
            // clang-format on

            b.get().click_handler = [this, path = utki::make_vector(path), parent_list](ruis::push_button& button){
                ASSERT(parent_list)
                parent_list->erase(utki::next(parent_list->begin(), path.back()));
                this->notify_item_remove(utki::make_span(path));
            };
            ret.get().push_back(b);
        }

        return ret;
    }

    size_t count(utki::span<const size_t> path) const noexcept override{
        auto children = &this->root;

        for(auto& i : path){
            children = &(*children)[i].children;
        }

        return children->size();
    }

};    
}

utki::shared_ref<ruis::window> make_tree_view_window(
    utki::shared_ref<ruis::context> c, //
    ruis::vec2_length pos
)
{
    // clang-format off
    auto w = m::window(c,
        {
            .widget_params{
                .rectangle{
                    {
                        pos.x().get(c.get()),
                        pos.y().get(c.get())
                    },
                    {
                        ruis::length::make_pp(300).get(c.get()),
                        ruis::length::make_pp(200).get(c.get())
                    }
                }
            },
            .container_params{
                .layout = ruis::layout::column
            },
            .title = U"TreeView"s
        },
        {
            m::row(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::fill},
                        .weight = 1
                    }
                },
                {
                    m::pile(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::max},
                                .weight = 1
                            }
                        },
                        {
                            m::tree_view(c,
                                {
                                    .layout_params{
                                        .dims{ruis::dim::fill, ruis::dim::fill}
                                    },
                                    .widget_params{
                                        .id = "treeview_widget"s,
                                        .clip = true
                                    }
                                }
                            )
                        }
                    ),
                    m::scroll_bar(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::min, ruis::dim::max}
                            },
                            .widget_params{
                                .id = "treeview_vertical_slider"s
                            },
                            .oriented_params{
                                .vertical = true
                            }
                        }
                    )
                }
            ),
            m::scroll_bar(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    },
                    .widget_params{
                        .id = "treeview_horizontal_slider"s
                    },
                    .oriented_params{
                        .vertical = false
                    }
                }
            ),
            m::row(c,
                {
                    .layout_params{
                        .dims{ruis::dim::max, ruis::dim::min}
                    }
                },
                {
                    m::text(c, {}, U"Insert:"s),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "insert_before"s
                            }
                        },
                        {
                            m::text(c, {}, U"before"s)
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "insert_after"s
                            }
                        },
                        {
                            m::text(c, {}, U"after"s)
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget_params{
                                .id = "insert_child"s
                            }
                        },
                        {
                            m::text(c, {}, U"child"s)
                        }
                    )
                }
            )
        }
    );
    // clang-format on

    auto& treeview = w.get().get_widget_as<ruis::tree_view>("treeview_widget");

    auto provider = std::make_shared<tree_view_items_provider>(w.get().context);
    treeview.set_provider(provider);
    auto tv = utki::make_weak_from(treeview);

    auto& vertical_slider = w.get().get_widget_as<ruis::fraction_band_widget>("treeview_vertical_slider");
    auto vs = utki::make_weak_from(vertical_slider);

    vertical_slider.fraction_change_handler = [tv](ruis::fraction_widget& slider){
        if(auto t = tv.lock()){
            t->set_vertical_scroll_factor(slider.get_fraction());
        }
    };

    auto& horizontal_slider = w.get().get_widget_as<ruis::fraction_band_widget>("treeview_horizontal_slider");
    auto hs = utki::make_weak_from(horizontal_slider);

    horizontal_slider.fraction_change_handler = [tv](ruis::fraction_widget& slider){
        if(auto t = tv.lock()){
            t->set_horizontal_scroll_factor(slider.get_fraction());
        }
    };

    treeview.scroll_change_handler = [
            hs = utki::make_weak_from(horizontal_slider),
            vs = utki::make_weak_from(vertical_slider)
        ](ruis::tree_view& tw)
    {
        auto f = tw.get_scroll_factor();
        auto b = tw.get_scroll_band();
        if(auto h = hs.lock()){
            h->set_band_fraction(b.x());
            h->set_fraction(f.x(), false);
        }
        if(auto v = vs.lock()){
            v->set_band_fraction(b.y());
            v->set_fraction(f.y(), false);
        }
    };

    auto insert_before_button = w.get().try_get_widget_as<ruis::push_button>("insert_before");
    auto insert_after_button = w.get().try_get_widget_as<ruis::push_button>("insert_after");
    auto insert_child = w.get().try_get_widget_as<ruis::push_button>("insert_child");

    auto prvdr = utki::make_weak(provider);
    insert_before_button->click_handler = [prvdr](ruis::push_button& b){
        if(auto p = prvdr.lock()){
            p->insert_before();
        }
    };

    insert_after_button->click_handler = [prvdr](ruis::push_button& b){
        if(auto p = prvdr.lock()){
            p->insert_after();
        }
    };

    insert_child->click_handler = [prvdr](ruis::push_button& b){
        if(auto p = prvdr.lock()){
            p->insert_child();
        }
    };

    return w;
}
