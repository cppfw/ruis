/**
 * @file Zip file reader
 * @author Ivan Gagis <igagis@gmail.com>
 */


#pragma once

#include <utki/debug.hpp>
#include <papki/File.hpp>

#include <memory>


namespace morda{


class ZipFile : public papki::File{
	std::unique_ptr<papki::File> zipFile;
	
	void* handle = nullptr;
public:
	ZipFile(std::unique_ptr<papki::File> zipFile, const std::string& path = std::string());

	~ZipFile()noexcept;


	void openInternal(papki::File::E_Mode mode) override;
	void closeInternal()const noexcept override;
	size_t readInternal(utki::Buf<std::uint8_t> buf)const override;
	bool exists() const override;
	std::vector<std::string> listDirContents(size_t maxEntries = 0)const override;
	
	std::unique_ptr<papki::File> spawn()override{
		std::unique_ptr<papki::File> zf = this->zipFile->spawn();
		zf->setPath(this->zipFile->path());
		return utki::makeUnique<ZipFile>(std::move(zf));
	}
};



}
