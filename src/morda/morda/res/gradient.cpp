#include <r4/vector4.hpp>

#include "gradient.hpp"

#include "../util/util.hpp"
#include "../context.hpp"
#include "../config.hpp"

using namespace morda::res;

morda::res::gradient::gradient(std::shared_ptr<morda::context> c, std::vector<std::tuple<real,std::uint32_t> >& stops, bool vertical) :
		resource(std::move(c))
{
	std::vector<r4::vector2<float>> vertices;
//	std::vector<std::uint32_t> colors;
	std::vector<r4::vector4<float>> colors;
	for(auto& s : stops){
		{
			auto c = std::get<1>(s);
			r4::vector4<float> clr(
					float(c & 0xff) / 255,
					float((c >> 8) & 0xff) / 255,
					float((c >> 16) & 0xff) / 255,
					float((c >> 24) & 0xff) / 255
				);

			colors.push_back(clr);
			colors.push_back(clr);
		}
		
//		TRACE(<< "put color = " << std::hex << colors.back() << std::endl)
		
		if(vertical){
			vertices.push_back(r4::vector2<float>(0, std::get<0>(s)));
			vertices.push_back(r4::vector2<float>(1, std::get<0>(s)));
		}else{ ASSERT(!vertical)
			vertices.push_back(r4::vector2<float>(std::get<0>(s), 1));
			vertices.push_back(r4::vector2<float>(std::get<0>(s), 0));
		}
//		TRACE(<< "put pos = " << vertices.back() << std::endl)
	}
	ASSERT(vertices.size() == colors.size())
	
	std::vector<std::uint16_t> indices;
	for(size_t i = 0; i != vertices.size(); ++i){
		indices.push_back(std::uint16_t(i));
	}
	
	auto& r = *this->context->renderer;
	this->vao = r.factory->create_vertex_array(
			{
				r.factory->create_vertex_buffer(utki::make_span(vertices)),
				r.factory->create_vertex_buffer(utki::make_span(colors))
			},
			r.factory->create_index_buffer(utki::make_span(indices)),
			vertex_array::mode::triangle_strip
		);
}



std::shared_ptr<gradient> gradient::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi) {
	bool vertical = false;

	std::vector<std::tuple<real,std::uint32_t>> stops;

	const char* stop_c = "Stop";

	for(auto& p : desc){
		if(p.value == "vertical"){
			vertical = get_property_value(p).to_bool();
		}else if(p.value == stop_c){
			real pos = 0;
			uint32_t color = 0xffffffff;
			for(auto& pp : p.children){
				if(pp.value == "pos"){
					pos = get_property_value(pp).to_float();
				}else if(pp.value == "color"){
					color = get_property_value(pp).to_uint32();
				}
			}
			stops.push_back(std::make_tuple(pos, color));
		}
	}
	
	return std::make_shared<gradient>(utki::make_shared_from(ctx), stops, vertical);
}


void gradient::render(const morda::matrix4& m) const {
	this->context->renderer->shader->pos_clr->render(m, *this->vao);
}

