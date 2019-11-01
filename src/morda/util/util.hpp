#pragma once

#include <tuple>

#include <utki/config.hpp>

#include <r4/vector2.hpp>
#include <r4/rectangle.hpp>

#include <puu/dom.hpp>

#include "../config.hpp"

#include "../render/Texture2D.hpp"
#include "../render/RenderFactory.hpp"

namespace morda{



/**
 * @brief Parse chain of 2 STOB nodes as Vec2r.
 * @param chain - chain of at least two nodes holding Vec2r values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with zeros.
 * @return parsed Vec2r.
 */
morda::Vec2r makeVec2rFromSTOB(const stob::Node* chain);

/**
 * @brief Parse chain of 4 STOB nodes as Rectr.
 * @param chain - chain of at least four nodes holding Rectr values.
 *                If there are less than 4 nodes in the chain then the rest of
 *                rectangle components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting rectangle will be filled with zeros.
 * @return parsed Rectr.
 */
morda::Rectr makeRectrFromSTOB(const stob::Node* chain);

/**
 * @brief Parse chain of 4 STOB nodes as Siders.
 * @param chain - chain of at least four nodes holding Siders values.
 *                If there are less than 4 nodes in the chain then the rest of
 *                Sidesr components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting Sidesr will be filled with zeros.
 * @return parsed Sidesr.
 */
morda::Sidesr makeSidesrFromSTOB(const stob::Node* chain);


/**
 * @brief Parse dimension Vec2r from STOB.
 * Same as Vec2rFromSTOB but using DimValue() to parse values.
 * @param chain - chain of at least two nodes holding Vec2r values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with zeroes.
 * @return parsed Vec2r.
 */
morda::Vec2r dimVec2rFromSTOB(const stob::Node* chain);



/**
 * @brief Parse chain of 2 STOB nodes as Vec2b.
 * @param chain - chain of at least two nodes holding Vec2b values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with false's.
 * @return parsed Vec2b.
 */
r4::vec2b makeVec2bFromSTOB(const stob::Node* chain);



/**
 * @brief Parse dimension value.
 * Parses value of 'pos' or 'dim' property from STOB.
 * In case the value is given in millimeters or points it will do the conversion.
 * @param n - stob node holding the value.
 * @return Parsed value in pixels.
 */
float dimValueFromSTOB(const stob::Node& n);



/**
 * @brief Parse layout dimension value.
 * Parses value of 'dim' value of layout parameters from STOB.
 * @param n - stob node holding the value.
 * @return Parsed value.
 */
float dimValueFromLayoutStob(const stob::Node& n);



/**
 * @brief Resolve includes in STOB document.
 * @param fi - file interface set to the original STOB document. Because resolving include paths is done relatively to original STOB document path.
 * @param begin - first node of the original STOB document.
 * @return New first node and pointer to the last child node of the script.
 */
std::tuple<std::unique_ptr<stob::Node>, stob::Node*> resolveIncludes(papki::File& fi, std::unique_ptr<stob::Node> begin);



/**
 * @brief Get property by name from STOB chain.
 * @param chain - STOB chain of properties.
 * @param property - property name to look for.
 * @return Pointer to property value if property was found in the STOB chain.
 * @return nullptr if property was not found or if the property has no value assigned.
 */
const stob::Node* getProperty(const stob::Node* chain, const char* property);

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

}
