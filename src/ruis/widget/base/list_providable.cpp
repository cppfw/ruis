#include "list_providable.hpp"

using namespace ruis;

list_provider::list_provider(utki::shared_ref<ruis::context> context) :
	context(std::move(context))
{}

void list_provider::notify_model_change()
{
	this->model_change_signal.emit();
}

void list_providable::set_provider(std::shared_ptr<list_provider> provider)
{
	if (this->params.provider) {
		this->provider_signal_connection = //
			this->params.provider->model_change_signal.connect( //
				[this]() {
					this->handle_model_change();
				}
			);
	}
	this->params.provider = std::move(provider);
	if (this->params.provider) {
		this->params.provider->model_change_signal.disconnect(this->provider_signal_connection);
	}
	this->handle_model_change();
}
