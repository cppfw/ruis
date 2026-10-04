#include "tree_view_window.hpp"

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/group/scroll_area.hpp>
#include <ruis/widget/group/tree_view.hpp>
#include <ruis/widget/slider/scroll_bar.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

namespace m {
using namespace ruis::make;
} // namespace m

namespace {
class tree_view_items_model
{
public:
	tml::forest root = tml::read(R"qwertyuiop(
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

	/**
	 * @brief New item added notification handler.
	 * @param index - index path of the item before which a new item has been added.
	 */
	std::function<void(utki::span<const size_t> index)> item_added_handler;

	/**
	 * @brief Item removed notification handler.
	 * @param index - index path of the removed item.
	 */
	std::function<void(utki::span<const size_t> index)> item_removed_handler;

	/**
	 * @brief Item changed notification handler.
	 */
	std::function<void()> item_changed_handler;

private:
	std::vector<size_t> selected_item;

	unsigned newItemNumber = 0;

	std::string generate_new_item_value()
	{
		std::stringstream ss;
		ss << "newItem" << this->newItemNumber;
		++this->newItemNumber;
		return ss.str();
	}

public:
	bool is_selected(utki::span<const size_t> index) const noexcept
	{
		return utki::deep_equals(index, utki::make_span(this->selected_item));
	}

	void select(utki::span<const size_t> index)
	{
		this->selected_item = utki::make_vector(index);

		utki::log_debug([&](auto& o) {
			o << " selected item = ";
			for (const auto& k : this->selected_item) {
				o << k << ", ";
			}
			o << std::endl;
		});

		if (this->item_changed_handler) {
			this->item_changed_handler();
		}
	}

	void insert_before()
	{
		if (this->selected_item.size() == 0) {
			return;
		}

		tml::forest* list = &this->root;
		tml::forest* parent_list = nullptr;

		for (auto& i : this->selected_item) {
			parent_list = list;
			list = &(*list)[i].children;
		}

		if (!parent_list) {
			return;
		}

		parent_list->insert(
			utki::next(parent_list->begin(), this->selected_item.back()),
			tml::leaf(this->generate_new_item_value())
		);

		if (this->item_added_handler) {
			this->item_added_handler(utki::make_span(this->selected_item));
		}
		++this->selected_item.back();
	}

	void insert_after()
	{
		if (this->selected_item.size() == 0) {
			return;
		}

		tml::forest* list = &this->root;
		tml::forest* parent_list = nullptr;

		for (auto& i : this->selected_item) {
			parent_list = list;
			list = &(*list)[i].children;
		}

		if (!parent_list) {
			return;
		}

		parent_list->insert(
			utki::next(parent_list->begin(), this->selected_item.back() + 1),
			tml::leaf(this->generate_new_item_value())
		);

		++this->selected_item.back();
		if (this->item_added_handler) {
			this->item_added_handler(utki::make_span(this->selected_item));
		}
		--this->selected_item.back();
	}

	void insert_child()
	{
		if (this->selected_item.size() == 0) {
			return;
		}

		tml::forest* list = &this->root;

		for (auto& i : this->selected_item) {
			list = &(*list)[i].children;
		}

		list->emplace_back(this->generate_new_item_value());

		this->selected_item.push_back(list->size() - 1);
		if (this->item_added_handler) {
			this->item_added_handler(utki::make_span(this->selected_item));
		}
		this->selected_item.pop_back();
	}

	void remove_item(utki::span<const size_t> index)
	{
		auto list = &this->root;
		tml::forest* parent_list = nullptr;

		for (auto& i : index) {
			parent_list = list;
			list = &(*list)[i].children;
		}

		utki::assert(parent_list, SL);
		parent_list->erase(utki::next(parent_list->begin(), index.back()));

		if (this->item_removed_handler) {
			this->item_removed_handler(index);
		}
	}
};
} // namespace

namespace {
class tree_view_items_provider : public ruis::tree_view::provider
{
	const utki::shared_ref<tree_view_items_model> model;

public:
	// NOLINTNEXTLINE(modernize-pass-by-value)
	tree_view_items_provider(
		const utki::shared_ref<ruis::context>& context, //
		utki::shared_ref<tree_view_items_model> model
	) :
		provider(context),
		model(std::move(model))
	{}

	tree_view_items_provider(const tree_view_items_provider&) = delete;
	tree_view_items_provider& operator=(const tree_view_items_provider&) = delete;

	tree_view_items_provider(tree_view_items_provider&&) = delete;
	tree_view_items_provider& operator=(tree_view_items_provider&&) = delete;

	~tree_view_items_provider() override = default;

	utki::shared_ref<ruis::widget> get_widget(utki::span<const size_t> path) override
	{
		utki::assert(!path.empty());

		auto list = &this->model.get().root;

		tml::tree* n = nullptr;

		for (const auto& i : path) {
			n = &(*list)[i];
			list = &n->children;
		}

		auto& c = this->context;
		auto model = this->model;

		auto ret = ruis::make::row(c, {});

		{
			// clang-format off
            auto v = m::pile(c,
                {},
                {
                    m::rectangle(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            },
                            .widget{
                                .id = "selection"s,
                                .visible = false
                            },
                            .params{
                                .specific{
                                    .fill_color = c.get().style().get_color_highlight()
                                }
                            }
                        }
                    ),
                    m::text(c,
                        {
                            .widget{
                                .id = "value"s
                            }
                        }
                    ),
                    m::mouse_proxy(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            },
                            .widget{
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
				value->set_string(
					n->value
						.string // NOLINT(clang-analyzer-core.CallAndMessage): due to ASSERT(!path.empty()) in the beginning of the function 'n' is not nullptr
				);
			}
			{
				auto& color_label = v.get().get_widget_as<ruis::rectangle>("selection");

				color_label.set_visible(model.get().is_selected(path));

				auto mp = v.get().try_get_widget_as<ruis::mouse_proxy>("mouse_proxy");
				utki::assert(mp, SL);
				mp->mouse_button_handler = [model, path = utki::make_vector(path)](
											   ruis::mouse_proxy&, //
											   const ruis::mouse_button_event& e
										   ) {
					if (e.action == ruis::button_action::release || e.button != ruis::mouse_button::left) {
						return ruis::event_status::propagate;
					}

					model.get().select(utki::make_span(path));

					return ruis::event_status::consumed;
				};
			}

			ret.get().push_back(v);
		}

		{
			// clang-format off
            auto b = m::push_button(c,
                {},
                {
                    m::rectangle(c,
                        {
                            .layout_params{
                                .dims{ruis::length::make_pp(5), ruis::length::make_pp(2)}
                            },
                            .params{
                                .specific{
                                    .fill_color = 0xff0000ff
                                }
                            }
                        }
                    )
                }
            );
			// clang-format on

			b.get().click_handler = [model, path = utki::make_vector(path)](ruis::push_button& button) {
				model.get().remove_item(utki::make_span(path));
			};
			ret.get().push_back(b);
		}

		return ret;
	}

	size_t count(utki::span<const size_t> path) const noexcept override
	{
		auto children = &this->model.get().root;

		for (auto& i : path) {
			children = &(*children)[i].children;
		}

		return children->size();
	}
};
} // namespace

utki::shared_ref<ruis::window> make_tree_view_window(
	const utki::shared_ref<ruis::context>& c, //
	ruis::vec2_length pos
)
{
	auto model = utki::make_shared<tree_view_items_model>();

	// clang-format off
    auto w = m::window(c,
        {
            .widget{
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
            .params{
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
                    m::scroll_area(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill},
                                .weight = 1
                            },
                            .widget{
                                .id = "tree_view_scroll_area"s
                            }
                        },
                        m::tree_view(c,
                            {
                                .layout_params{
                                    .dims{ruis::dim::min, ruis::dim::fill}
                                },
                                .widget{
                                    .id = "treeview_widget"s,
                                    .clip = true
                                },
                                .tree_view_params{
                                    .provider = utki::make_unique<tree_view_items_provider>(c, model)
                                }
                            }
                        )
                    ),
                    m::scroll_bar(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::min, ruis::dim::max}
                            },
                            .widget{
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
                    .widget{
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
                            .widget{
                                .id = "insert_before"s
                            }
                        },
                        {
                            m::text(c, {}, U"before"s)
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget{
                                .id = "insert_after"s
                            }
                        },
                        {
                            m::text(c, {}, U"after"s)
                        }
                    ),
                    m::push_button(c,
                        {
                            .widget{
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

	auto tv = utki::make_weak_from(treeview);

	model.get().item_added_handler = [tv](utki::span<const size_t> index) {
		if (auto w = tv.lock()) {
			w->notify_item_added(index);
		}
	};

	model.get().item_removed_handler = [tv](utki::span<const size_t> index) {
		if (auto w = tv.lock()) {
			w->notify_item_removed(index);
		}
	};

	model.get().item_changed_handler = [tv]() {
		if (auto w = tv.lock()) {
			w->notify_item_changed();
		}
	};

	auto& vertical_slider = w.get().get_widget_as<ruis::fraction_band_widget>("treeview_vertical_slider");
	auto vs = utki::make_weak_from(vertical_slider);

	vertical_slider.fraction_change_handler = [tv](ruis::fraction_widget& slider) {
		if (auto t = tv.lock()) {
			t->set_scroll_factor(slider.get_fraction());
		}
	};

	auto& tvsa = w.get().get_widget_as<ruis::scroll_area>("tree_view_scroll_area"sv);

	auto& horizontal_slider = w.get().get_widget_as<ruis::fraction_band_widget>("treeview_horizontal_slider");
	auto hs = utki::make_weak_from(horizontal_slider);

	horizontal_slider.fraction_change_handler = [tvsa = utki::make_weak_from(tvsa)](ruis::fraction_widget& slider) {
		if (auto sa = tvsa.lock()) {
			sa->set_scroll_factor({slider.get_fraction(), 0});
		}
	};

	tvsa.scroll_change_handler = [hs = utki::make_weak_from(horizontal_slider)](ruis::scroll_area& sa) {
		if (auto h = hs.lock()) {
			h->set_band_fraction(sa.get_visible_area_fraction().x());
			h->set_fraction(sa.get_scroll_factor().x(), false);
		}
	};

	treeview.scroll_change_handler = [vs = utki::make_weak_from(vertical_slider)](ruis::tree_view& tw) {
		auto f = tw.get_scroll_factor();
		auto b = tw.get_scroll_band();
		if (auto v = vs.lock()) {
			v->set_band_fraction(b);
			v->set_fraction(f, false);
		}
	};

	auto insert_before_button = w.get().try_get_widget_as<ruis::push_button>("insert_before");
	auto insert_after_button = w.get().try_get_widget_as<ruis::push_button>("insert_after");
	auto insert_child = w.get().try_get_widget_as<ruis::push_button>("insert_child");

	insert_before_button->click_handler = [model](ruis::push_button& b) {
		model.get().insert_before();
	};

	insert_after_button->click_handler = [model](ruis::push_button& b) {
		model.get().insert_after();
	};

	insert_child->click_handler = [model](ruis::push_button& b) {
		model.get().insert_child();
	};

	return w;
}
