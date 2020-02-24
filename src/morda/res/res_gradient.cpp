#include <r4/vector4.hpp>

#include "res_gradient.hpp"

#include "../util/util.hpp"
#include "../context.hpp"

using namespace morda;

res_gradient::res_gradient(std::shared_ptr<morda::context> c, std::vector<std::tuple<real,std::uint32_t> >& stops, bool vertical) :
		resource(std::move(c))
{
	std::vector<r4::vec2f> vertices;
//	std::vector<std::uint32_t> colors;
	std::vector<r4::vec4f> colors;
	for(auto& s : stops){
		{
			auto c = std::get<1>(s);
			r4::vec4f clr(
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
			vertices.push_back(r4::vec2f(0, std::get<0>(s)));
			vertices.push_back(r4::vec2f(1, std::get<0>(s)));
		}else{ ASSERT(!vertical)
			vertices.push_back(r4::vec2f(std::get<0>(s), 1));
			vertices.push_back(r4::vec2f(std::get<0>(s), 0));
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



std::shared_ptr<res_gradient> res_gradient::load(morda::context& ctx, const puu::forest& desc, const papki::file& fi) {
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
	
	return std::make_shared<res_gradient>(ctx.shared_from_this(), stops, vertical);
}


void res_gradient::render(const morda::Matr4r& m) const {
	this->context->renderer->shader->pos_clr->render(m, *this->vao);
}

