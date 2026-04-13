#include "tab_button.hpp"

#include "../../label/padding.hpp"

using namespace ruis::touch;

using namespace ruis::length_literals;

tab_button::tab_button(
    utki::shared_ref<ruis::context> context, //
    all_parameters params,
    ruis::string text
) :
    widget(
        std::move(context), //
        std::move(params.layout_params),
        std::move(params.widget_params)
    ),
    button(
        this->context,
        {
}
    ),
    toggle_button(this->context),
    choice_button(this->context),
    // clang-format off
    ruis::container(
        this->context, //
        {
            .container_params = {
                .layout = ruis::layout::pile
            }
        },
        {
            ruis::make::padding(
                this->context,
                {
                    .layout_params = {
                        .dims = {ruis::dim::fill, ruis::dim::fill}
                    },
                    .container_params = {
                        .layout = ruis::layout::column
                    },
                    .padding_params = {
                        .borders = {10_pp}
                    }
                },
                {
                    ruis::make::image(this->context,
                        {
                            .layout_params = {
                                .dims = {ruis::dim::min, ruis::dim::fill},
                                .weight = 1
                            },
                            .image_params = std::move(params.image_params)
                        }
                    ),
                    ruis::make::text(this->context,
                        {
                            .layout_params = {
                                .dims = {ruis::dim::min, ruis::dim::min},
                                .align = {ruis::align::center, ruis::align::center}
                            }
                        },
                        std::move(text)
                    )
                }
            )
        }
    )
// clang-format on
{}

ruis::event_status tab_button::on_mouse_button(const ruis::mouse_button_event& event)
{
    return this->choice_button::on_mouse_button(event);
}

utki::shared_ref<ruis::touch::tab_button> ruis::touch::make::tab_button(
	utki::shared_ref<ruis::context> context, //
	ruis::touch::tab_button::all_parameters params,
    ruis::string text
)
{
	return utki::make_shared<ruis::touch::tab_button>(
        std::move(context),
        std::move(params),
        std::move(text)
    );
}
