#pragma once

#include <utki/debug.hpp>
#include <papki/file.hpp>

#include <memory>


namespace morda{

/**
 * @brief Undocumented.
 */
class ZipFile : public papki::file{
	std::unique_ptr<papki::file> zipFile;
	
	void* handle = nullptr;
public:
	ZipFile(std::unique_ptr<papki::file> zipFile, const std::string& path = std::string());

	~ZipFile()noexcept;


	void open_internal(papki::File::E_Mode mode) override;
	void close_internal()const noexcept override;
	size_t read_internal(utki::span<std::uint8_t> buf)const override;
	bool exists() const override;
	std::vector<std::string> list_dir(size_t maxEntries = 0)const override;
	
	std::unique_ptr<papki::File> spawn()override{
		std::unique_ptr<papki::File> zf = this->zipFile->spawn();
		zf->setPath(this->zipFile->path());
		return utki::make_unique<ZipFile>(std::move(zf));
	}
};



}
