/**
 * @author Ivan Gagis <igagis@gmail.com>
 */

#pragma once

#include "Vector2.hpp"
#include "Rectangle2.hpp"

#include <tuple>

#include <stob/dom.hpp>


namespace morda{



/**
 * @brief Parse chain of 2 STOB nodes as Vec2r.
 * @param chain - chain of at least two nodes holding Vec2r values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with zeros.
 * @return parsed Vec2r.
 */
morda::Vec2r Vec2rFromSTOB(const stob::Node* chain);

/**
 * @brief Parse chain of 4 STOB nodes as Rect2r.
 * @param chain - chain of at least four nodes holding Rect2r values.
 *                If there are less than 4 nodes in the chain then the rest of
 *                rectangle components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting rectangle will be filled with zeros.
 * @return parsed Rect2r.
 */
morda::Rect2r Rect2rFromSTOB(const stob::Node* chain);



/**
 * @brief Parse dimension Vec2r from STOB.
 * Same as Vec2rFromSTOB but using DimValue() to parse values.
 * @param chain - chain of at least two nodes holding Vec2r values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with zeroes.
 * @return parsed Vec2r.
 */
morda::Vec2r DimVec2rFromSTOB(const stob::Node* chain);



/**
 * @brief Parse chain of 2 STOB nodes as Vec2b.
 * @param chain - chain of at least two nodes holding Vec2b values.
 *                If there are less than 2 nodes in the chain then the rest of
 *                vector components will be filled with latest parsed value.
 *                If zero pointer is passed the resulting vector will be filled with false's.
 * @return parsed Vec2b.
 */
morda::Vector2<bool> Vec2bFromSTOB(const stob::Node* chain);



/**
 * @brief Parse dimension value.
 * Parses value of 'pos' or 'dim' property from STOB.
 * In case the value is given in millimeters it will do the conversion.
 * @param n - stob node holding the value.
 * @return Parsed value in pixels.
 */
float DimValueFromSTOB(const stob::Node& n);



/**
 * @brief Resolve includes in STOB document.
 * @param fi - file interface set to the original STOB document. Because resolving include paths is done relatively to original STOB document path.
 * @param begin - first node of the original STOB document.
 * @return New first node and pointer to the last child node of the script.
 */
std::tuple<std::unique_ptr<stob::Node>, stob::Node*> ResolveIncludes(ting::fs::File& fi, std::unique_ptr<stob::Node> begin);



inline bool NodeHoldsFractionValue(const stob::Node& node)noexcept{
	size_t len = node.ValueLength();
	return len != 0 && node.Value()[len - 1] == '%';
}


inline const stob::Node* GetProperty(const stob::Node* chain, const char* property){
	if(!chain){
		return nullptr;
	}
	auto n = chain->ThisOrNext(property).node();
	return n ? n->Child() : nullptr;
}


}//~namespace
