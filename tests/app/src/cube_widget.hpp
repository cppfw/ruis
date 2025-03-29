#pragma once

#include <ruis/updateable.hpp>
#include <ruis/widget/widget.hpp>
#include <ruis/res/texture_2d.hpp>

class cube_widget :
    public ruis::widget,
    public ruis::updateable
{
	std::shared_ptr<const ruis::res::texture_2d> tex;

	ruis::quaternion rot = ruis::quaternion().set_identity();
public:
	std::shared_ptr<ruis::render::vertex_array> cubeVAO;

    struct all_parameters{
        ruis::layout_parameters layout_params;
        ruis::widget::parameters widget_params;
    };

    cube_widget(
        utki::shared_ref<ruis::context> context, //
        all_parameters params
    );

	unsigned fps = 0;
	uint32_t fpsSecCounter = 0;

	void update(uint32_t dt) override;

	void render(const ruis::matrix4& matrix)const override;
};

namespace make{
inline utki::shared_ref<::cube_widget> cube_widget(
    utki::shared_ref<ruis::context> context,
    ::cube_widget::all_parameters params
)
{
    return utki::make_shared<::cube_widget>(
        std::move(context),
        std::move(params)
    );
}
}
