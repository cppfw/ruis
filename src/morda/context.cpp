#include "context.hpp"

using namespace morda;


context::context(
		std::shared_ptr<morda::Renderer> r,
		std::shared_ptr<morda::updater> u,
		std::function<void(std::function<void()>&&)>&& run_from_ui_thread_function,
		real dots_per_inch,
		real dots_per_dp
	) :
		renderer(std::move(r)),
		updater(std::move(u)),
		run_from_ui_thread(std::move(run_from_ui_thread_function)),
		loader(*this),
		units(dots_per_inch, dots_per_dp)
{
	if(!this->renderer){
		throw std::invalid_argument("context::context(): passed in renderer pointer is null");
	}
	if(!this->run_from_ui_thread){
		throw std::invalid_argument("context::context(): no post to UI thread function provided");
	}
}

void context::set_focused_widget(std::shared_ptr<widget> w){
	if(auto prev = this->focused_widget.lock()){
		prev->isFocused_v = false;
		prev->on_focus_changed();
	}

	this->focused_widget = w;

	if(w){
		w->isFocused_v = true;
		w->on_focus_changed();
	}
}
