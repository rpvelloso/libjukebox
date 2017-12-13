/*
 * SoundFile.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#include "SoundFile.h"

namespace jukebox {

SoundFile::SoundFile(SoundFileImpl *impl) : impl(impl) {};

short SoundFile::getNumChannels() const {
	return impl->getNumChannels();
};

int SoundFile::getSampleRate() const {
	return impl->getSampleRate();
};

short SoundFile::getBitsPerSample() const {
	return impl->getBitsPerSample();
};

const char *SoundFile::getData() const {
	return impl->getData();
};

int SoundFile::getDataSize() const {
	return impl->getDataSize();
};

} /* namespace jukebox */

const std::string& jukebox::SoundFile::getFilename() const {
	return impl->getFilename();
}
