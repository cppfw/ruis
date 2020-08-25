#pragma once

#include <utki/config.hpp>

#include <r4/vector2.hpp>
#include <r4/rectangle.hpp>

#include <puu/tree.hpp>

#include "../config.hpp"

#include "../render/renderer.hpp"
#include "../render/texture_2d.hpp"
#include "../render/render_factory.hpp"

#include "units.hpp"

namespace morda{

morda::vector2 parse_vec2(puu::forest::const_iterator begin, puu::forest::const_iterator end);

/**
 * @brief Parse 2 values from puu as vector2.
 * @param desc - forest of at least two trees holding vector2 values.
 *                If there are less than 2 trees in the forest then the rest of
 *                vector components will be filled with latest parsed value.
 * @return parsed vector2.
 */
inline morda::vector2 parse_vec2(const puu::forest& desc){
    return parse_vec2(desc.begin(), desc.end());
}

/**
 * @brief Parse chain of 4 puu nodes as rectangle.
 * @param desc - chain of at least four nodes holding rectangle values.
 *               If there are less than 4 nodes in the chain then the rest of
 *               rectangle components will be filled with latest parsed value.
 *               If zero pointer is passed the resulting rectangle will be filled with zeros.
 * @return parsed rectangle.
 */
morda::rectangle parse_rect(const puu::forest& desc);

/**
 * @brief Parse chain of 4 puu nodes as sides.
 * @param desc - chain of at least four nodes holding sides values.
 *               If there are less than 4 nodes in the chain then the rest of
 *               sides<real> components will be filled with latest parsed value.
 *               If zero pointer is passed the resulting sides<real> will be filled with zeros.
 * @return parsed sides<real>.
 */
morda::sides<real> parse_sides(const puu::forest& desc);

/**
 * @brief Parse dimension value.
 * Parses value of dimension property from puu leaf.
 * In case the value is given in millimeters or points it will do the conversion.
 * @param l - puu leaf holding the value.
 * @param units - information about units. Can be obtained from context.
 * @return Parsed value in pixels.
 */
real parse_dimension_value(const puu::leaf& l, const morda::units& units);

/**
 * @brief Parse layout dimension value.
 * Parses value of dimension value of layout parameters from puu.
 * @param l - puu leaf holding the value.
 * @param units - information about units. Can be obtained from context.
 * @return Parsed value.
 */
real parse_layout_dimension_value(const puu::leaf& l, const morda::units& units);

bool is_property(const puu::tree& t);

bool is_leaf_property(const puu::leaf& l);

bool is_leaf_child(const puu::leaf& l);

/**
 * @brief Load texture from file.
 * @param r - renderer.
 * @param fi - file to load texture from.
 * @return Loaded texture.
 */
std::shared_ptr<texture_2d> load_texture(renderer& r, const papki::file& fi);

/**
 * @brief Set simple alpha blending to rendering context.
 * Enables and set simple alpha blending on the rendering context.
 * Blend factors are SRC_ALPHA and ONE_MINUS_SRC_ALPHA for source and destination RGB color components respectively.
 * And, ONE and ONE_MINUS_SRC_ALPHA for source and destination alpha components respectively.
 */
void set_simple_alpha_blending(renderer& r);

morda::texture_2d::type num_channels_to_texture_type(unsigned numChannels);

r4::vec4f color_to_vec4f(uint32_t color);

inline const puu::leaf& get_property_value(const puu::tree& p){
    if(p.children.size() != 1){
        throw std::invalid_argument("get_property_value(): property has no value");
    }
    return p.children.front().value;
}

}
