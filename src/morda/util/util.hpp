#pragma once

#include <tuple>

#include <utki/config.hpp>

#include <r4/vector2.hpp>
#include <r4/rectangle.hpp>

#include <puu/tree.hpp>

#include "../config.hpp"

#include "../render/renderer.hpp"
#include "../render/Texture2D.hpp"
#include "../render/RenderFactory.hpp"

#include "units.hpp"

namespace morda{


morda::Vec2r parse_vec2(puu::forest::const_iterator begin, puu::forest::const_iterator end);

/**
 * @brief Parse 2 values from puu as Vec2r.
 * @param desc - forest of at least two trees holding Vec2r values.
 *                If there are less than 2 trees in the forest then the rest of
 *                vector components will be filled with latest parsed value.
 * @return parsed Vec2r.
 */
inline morda::Vec2r parse_vec2(const puu::forest& desc){
    return parse_vec2(desc.begin(), desc.end());
}

/**
 * @brief Parse chain of 4 STOB nodes as Rectr.
 * @param desc - chain of at least four nodes holding Rectr values.
 *               If there are less than 4 nodes in the chain then the rest of
 *               rectangle components will be filled with latest parsed value.
 *               If zero pointer is passed the resulting rectangle will be filled with zeros.
 * @return parsed Rectr.
 */
morda::Rectr parse_rect(const puu::forest& desc);


/**
 * @brief Parse chain of 4 STOB nodes as Siders.
 * @param desc - chain of at least four nodes holding Siders values.
 *               If there are less than 4 nodes in the chain then the rest of
 *               Sidesr components will be filled with latest parsed value.
 *               If zero pointer is passed the resulting Sidesr will be filled with zeros.
 * @return parsed Sidesr.
 */
morda::Sidesr parse_sides(const puu::forest& desc);

/**
 * @brief Parse dimension value.
 * Parses value of 'pos' or 'dim' property from puu leaf.
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

/**
 * @brief Load texture from file.
 * @param r - renderer.
 * @param fi - file to load texture from.
 * @return Loaded texture.
 */
std::shared_ptr<Texture2D> loadTexture(renderer& r, const papki::file& fi);


/**
 * @brief Enable simple alpha blending to rendering context.
 * Enables simple alpha blending on the rendering context.
 * Blend factors are SRC_ALPHA and ONE_MINUS_SRC_ALPHA for source and destination RGB color components respectively.
 * And, ONE and ONE_MINUS_SRC_ALPHA for source and destination alpha components respectively.
 */
void applySimpleAlphaBlending(renderer& r);


morda::Texture2D::TexType_e numChannelsToTexType(unsigned numChannels);

r4::vec4f colorToVec4f(std::uint32_t color);

inline const puu::leaf& get_property_value(const puu::tree& p){
    if(p.children.size() != 1){
        throw std::invalid_argument("get_property_value(): property has no value");
    }
    return p.children.front().value;
}

}
