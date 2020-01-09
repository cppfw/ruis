#pragma once

#include <tuple>

#include <utki/config.hpp>

#include <r4/vector2.hpp>
#include <r4/rectangle.hpp>

#include <puu/dom.hpp>
#include <puu/tree.hpp>

#include "../config.hpp"

#include "../render/Texture2D.hpp"
#include "../render/RenderFactory.hpp"

namespace morda{



/**
 * @brief Parse 2 values from puu as Vec2r.
 * @param chain - chain of at least two nodes holding Vec2r values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with zeros.
 * @return parsed Vec2r.
 */
morda::Vec2r parse_vec2(puu::forest::const_iterator begin, puu::forest::const_iterator end);

inline morda::Vec2r parse_vec2(const puu::forest& desc){
    return parse_vec2(desc.begin(), desc.end());
}

/**
 * @brief Parse chain of 4 STOB nodes as Rectr.
 * @param chain - chain of at least four nodes holding Rectr values.
 *                If there are less than 4 nodes in the chain then the rest of
 *                rectangle components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting rectangle will be filled with zeros.
 * @return parsed Rectr.
 */
morda::Rectr parse_rect(const puu::forest& desc);


/**
 * @brief Parse chain of 4 STOB nodes as Siders.
 * @param chain - chain of at least four nodes holding Siders values.
 *                If there are less than 4 nodes in the chain then the rest of
 *                Sidesr components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting Sidesr will be filled with zeros.
 * @return parsed Sidesr.
 */
morda::Sidesr parse_sides(const puu::forest& desc);

/**
 * @brief Parse dimension value.
 * Parses value of 'pos' or 'dim' property from STOB.
 * In case the value is given in millimeters or points it will do the conversion.
 * @param n - stob node holding the value.
 * @return Parsed value in pixels.
 */
real dimValueFromSTOB(const stob::Node& n);

real parse_dimension_value(const puu::leaf& l);

/**
 * @brief Parse layout dimension value.
 * Parses value of dimension value of layout parameters from puu.
 * @param l - puu leaf holding the value.
 * @return Parsed value.
 */
real parse_layout_dimension_value(const puu::leaf& l);



/**
 * @brief Resolve includes in STOB document.
 * @param fi - file interface set to the original STOB document. Because resolving include paths is done relatively to original STOB document path.
 * @param begin - first node of the original STOB document.
 * @return New first node and pointer to the last child node of the script.
 */
std::tuple<std::unique_ptr<stob::Node>, stob::Node*> resolveIncludes(const papki::File& fi, std::unique_ptr<stob::Node> begin);



/**
 * @brief Get property by name from STOB chain.
 * @param chain - STOB chain of properties.
 * @param property - property name to look for.
 * @return Pointer to property value if property was found in the STOB chain.
 * @return nullptr if property was not found or if the property has no value assigned.
 */
const stob::Node* getProperty(const stob::Node* chain, const char* property);

bool is_property(const puu::tree& t);

bool is_leaf_property(const puu::leaf& l);

/**
 * @brief Load texture from file.
 * @param fi - file to load texture from.
 * @return Loaded texture.
 */
std::shared_ptr<Texture2D> loadTexture(const papki::File& fi);


/**
 * @brief Enable simple alpha blending to rendering context.
 * Enables simple alpha blending on the rendering context.
 * Blend factors are SRC_ALPHA and ONE_MINUS_SRC_ALPHA for source and destination RGB color components respectively.
 * And, ONE and ONE_MINUS_SRC_ALPHA for source and destination alpha components respectively.
 */
void applySimpleAlphaBlending();


morda::Texture2D::TexType_e numChannelsToTexType(unsigned numChannels);

r4::vec4f colorToVec4f(std::uint32_t color);

inline const puu::leaf& get_property_value(const puu::tree& p){
    if(p.children.size() != 1){
        throw std::invalid_argument("get_property_value(): property has no value");
    }
    return p.children.front().value;
}

}
