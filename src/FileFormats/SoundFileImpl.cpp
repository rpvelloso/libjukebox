/*
 * SoundFileImpl.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#include "SoundFileImpl.h"

namespace jukebox {

SoundFileImpl::SoundFileImpl(const std::string &filename) : filename(filename) {
}

SoundFileImpl::~SoundFileImpl() {
}

} /* namespace jukebox */

const std::string& jukebox::SoundFileImpl::getFilename() const {
	return filename;
}
