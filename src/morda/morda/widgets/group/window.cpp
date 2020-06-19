#include "window.hpp"

#include "../../context.hpp"

#include "../../util/util.hpp"

using namespace morda;

namespace{

const auto windowDesc_c = puu::read(R"qwertyuiop(
		@column{
			layout{
				dx{max} dy{max}
			}

			@row{
				layout{dx{fill}}

				@mouse_proxy{
					id{morda_lt_proxy}
				}

				@mouse_proxy{
					id{morda_t_proxy}
					layout{
						dx{0} dy{fill} weight{1}
					}
				}

				@mouse_proxy{
					id{morda_rt_proxy}
				}
			}

			@row{
				layout{
					weight{1}
					dx{max}
				}

				@mouse_proxy{
					id{morda_l_proxy}
					layout{dy{fill}}
				}

				// middle
				@column{
					layout{
						weight{1}
						dy{max}
					}

					// caption
					@pile{
						layout{
							dx{max}
						}

						clip{true}

						@mouse_proxy{
							id{morda_caption_proxy}
							layout{
								dx{max} dy{max}
							}
						}

						@color{
							id{morda_window_title_bg}
							layout{
								dx{max} dy{max}
							}
						}

						@row{
							layout{
								dx{max} dy{max}
							}

							@margins{
								left{3dp}
								top{2dp}
								bottom{2dp}

								layout{
									dx{0}
									weight{1}
								}

								@text{
									id{morda_title}
								}
							}
						}
					}

					@pile{
						id{morda_content}
						clip{true}
						layout{
							dx{fill} dy{0}
							weight{1}
						}
					}
				}

				@mouse_proxy{
					id{morda_r_proxy}
					layout{dy{fill}}
				}
			}

			@row{
				layout{
					dx{fill}
				}
				@mouse_proxy{
					id{morda_lb_proxy}
				}

				@mouse_proxy{
					id{morda_b_proxy}
					layout{
						dy{fill}
						dx{0}
						weight{1}
					}
				}

				@mouse_proxy{
					id{morda_rb_proxy}
				}
			}
		}
	)qwertyuiop");

}

void morda::window::set_background(std::shared_ptr<widget> w){
	ASSERT(this->children().size() == 1 || this->children().size() == 2)
	if(this->children().size() == 2){
		this->erase(this->children().begin());
	}

	ASSERT(this->children().size() == 1)

	if(w){
		this->insert(std::move(w), this->children().begin());
	}
}

morda::window::window(std::shared_ptr<morda::context> c, const puu::forest& desc) :
		widget(std::move(c), desc),
		pile(nullptr, windowDesc_c)
{
	this->setupWidgets();

	sides<real> borders(0);

	for(const auto& p : desc){
		if(!is_property(p)){
			continue;
		}

		if(p.value == "title"){
			this->set_title(get_property_value(p).to_string());
		}else if(p.value == "look"){
			for(const auto& pp : p.children){
				if(!is_property(pp)){
					continue;
				}

				if(pp.value == "title_color_active"){
					this->titleBgColorTopmost = get_property_value(pp).to_uint32();
				}else if(pp.value == "title_color_inactive"){
					this->titleBgColorNonTopmost = get_property_value(pp).to_uint32();
				}else if(pp.value == "background"){
					this->set_background(this->context->inflater.inflate(pp.children));
				}else if(pp.value == "left"){
					borders.left() = parse_dimension_value(get_property_value(pp), this->context->units);
				}else if(pp.value == "top"){
					borders.top() = parse_dimension_value(get_property_value(pp), this->context->units);
				}else if(pp.value == "right"){
					borders.right() = parse_dimension_value(get_property_value(pp), this->context->units);
				}else if(pp.value == "bottom"){
					borders.bottom() = parse_dimension_value(get_property_value(pp), this->context->units);
				}
			}
		}
	}
	this->set_borders(borders);

	// this should go after initializing borders
	this->emptyMinDim = this->measure(vector2(-1));

	this->contentArea->inflate_push_back(desc);
}

void morda::window::setupWidgets(){
	this->contentArea = this->try_get_widget_as<pile>("morda_content");
	ASSERT(this->contentArea)

	this->title = this->try_get_widget_as<text>("morda_title");
	ASSERT(this->title)

	this->titleBg = this->try_get_widget_as<color>("morda_window_title_bg");
	ASSERT(this->titleBg);

	std::function<decltype(mouse_proxy::mouse_button_handler)(bool&)> getButtonFunc = [this](bool& flag){
		return decltype(mouse_proxy::mouse_button_handler)([this, &flag](widget& widget, bool isDown, const morda::vector2& pos, mouse_button button, unsigned pointerId){
			if(button != mouse_button::left){
				return false;
			}

			if(isDown){
				flag = true;
				this->capturePoint = pos;
				return true;
			}
			flag = false;
			return false;
		});
	};

	{
		auto caption = this->try_get_widget_as<mouse_proxy>("morda_caption_proxy");
		ASSERT(caption)

		caption->mouse_button_handler = getButtonFunc(this->captionCaptured);

		caption->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->captionCaptured){
				this->move_by(pos - this->capturePoint);
				return true;
			}
			return false;
		};
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_lt_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->leftTopResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->leftTopResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->move_by(d);
				this->resize_by(-d);
			}
			return false;
		};
		this->ltBorder = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_lb_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->leftBottomResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->leftBottomResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->move_by(morda::vector2(d.x, 0));
				this->resize_by(morda::vector2(-d.x, d.y));
			}
			return false;
		};
		this->lbBorder = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_rt_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->rightTopResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->rightTopResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->move_by(morda::vector2(0, d.y));
				this->resize_by(morda::vector2(d.x, -d.y));
			}
			return false;
		};
		this->rtBorder = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_rb_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->rightBottomResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->rightBottomResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resize_by(d);
			}
			return false;
		};
		this->rbBorder = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_l_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->leftResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->leftResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampTop(d.x, this->rect().d.x - this->emptyMinDim.x);
				this->move_by(morda::vector2(d.x, 0));
				this->resize_by(morda::vector2(-d.x, 0));
			}
			return false;
		};
		this->lBorder = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_r_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->rightResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->rightResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampBottom(d.x, -(this->rect().d.x - this->emptyMinDim.x));
				this->resize_by(morda::vector2(d.x, 0));
			}
			return false;
		};
		this->rBorder = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_t_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->topResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->topResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampTop(d.y, this->rect().d.y - this->emptyMinDim.y);
				this->move_by(morda::vector2(0, d.y));
				this->resize_by(morda::vector2(0, -d.y));
			}
			return false;
		};
		this->tBorder = w;
	}

	{
		auto w = this->try_get_widget_as<mouse_proxy>("morda_b_proxy");
		ASSERT(w)
		w->mouse_button_handler = getButtonFunc(this->bottomResizeCaptured);
		w->mouse_move_handler = [this](widget& widget, const morda::vector2& pos, unsigned pointerId){
			if(this->bottomResizeCaptured){
				morda::vector2 d = pos - this->capturePoint;
				utki::clampBottom(d.y, -(this->rect().d.y - this->emptyMinDim.y));
				this->resize_by(morda::vector2(0, d.y));
			}
			return false;
		};
		this->bBorder = w;
	}
}

void morda::window::set_title(const std::string& str){
	this->title->set_text(utki::to_utf32(str));
}

void morda::window::set_borders(sides<real> borders) {
	this->lBorder->get_layout_params().dims.x = borders.left();
	this->tBorder->get_layout_params().dims.y = borders.top();
	this->rBorder->get_layout_params().dims.x = borders.right();
	this->bBorder->get_layout_params().dims.y = borders.bottom();

	this->lbBorder->get_layout_params().dims.x = borders.left();
	this->lbBorder->get_layout_params().dims.y = borders.bottom();

	this->rbBorder->get_layout_params().dims.x = borders.right();
	this->rbBorder->get_layout_params().dims.y = borders.bottom();

	this->ltBorder->get_layout_params().dims.x = borders.left();
	this->ltBorder->get_layout_params().dims.y = borders.top();

	this->rtBorder->get_layout_params().dims.x = borders.right();
	this->rtBorder->get_layout_params().dims.y = borders.top();
}

bool morda::window::on_mouse_button(bool isDown, const morda::vector2& pos, mouse_button button, unsigned pointerId){
	if(isDown && !this->is_topmost()){
		this->context->run_from_ui_thread(
				[this](){
					this->move_to_top();
					this->focus();
				}
			);
	}

	this->container::on_mouse_button(isDown, pos, button, pointerId);

	return true;
}

bool morda::window::on_mouse_move(const morda::vector2& pos, unsigned pointerId){
	this->container::on_mouse_move(pos, pointerId);
	return true;
}

void window::move_to_top(){
	if(!this->parent()){
		return;
	}

	ASSERT(this->parent()->children().size() != 0)

	if(this->parent()->children().size() == 1){
		return;
	}

	auto prevTopmost = this->parent()->children().rbegin()->get();
	ASSERT(prevTopmost)
	if(prevTopmost == this){
		return;//already topmost
	}

	container* p = this->parent();

	p->change_child_z_position(p->find(this), p->children().end());

	this->updateTopmost();

	if(auto pt = dynamic_cast<window*>(prevTopmost)){
		pt->updateTopmost();
	}
}

bool window::is_topmost()const noexcept{
	if(!this->parent()){
		return false;
	}

	ASSERT(this->parent()->children().size() != 0)

	return this->parent()->children().back().get() == this;
}

void window::updateTopmost(){
	this->titleBg->set_color(
			this->is_topmost() ? this->titleBgColorTopmost : this->titleBgColorNonTopmost
		);
}

void window::lay_out(){
	this->updateTopmost();
	this->pile::lay_out();
}
