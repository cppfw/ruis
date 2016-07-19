#pragma once

#include "../ResourceManager.hpp"


namespace morda{

/**
 * @brief Resource holding a STOB chain.
 * 
 * %Resource description:
 * 
 * @param file - file to read the STOB from.
 * 
 * Example:
 * @code
 * stb_sample_stob{
 *     file{sample.stob}
 * }
 * @endcode
 */
class ResSTOB : public Resource{
	friend class ResourceManager;
	
	std::unique_ptr<stob::Node> s;
public:
	/**
	 * @brief Create STOB resource.
	 * @param s - stob chain to initialize the resource with.
	 */
	ResSTOB(std::unique_ptr<stob::Node> s);
	
	ResSTOB(const ResSTOB&) = delete;
	ResSTOB& operator=(const ResSTOB&) = delete;
	
	/**
	 * @brief Get STOB chain.
	 * @return STOB chain.
	 */
	const stob::Node* chain()const{
		return this->s.get();
	}
	
private:
	static std::shared_ptr<ResSTOB> load(const stob::Node& chain, const papki::File& fi);
};

}
