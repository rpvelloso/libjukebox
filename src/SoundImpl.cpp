/*
 * SoundImpl.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#include "SoundImpl.h"

namespace jukebox {

SoundImpl::SoundImpl(SoundFile &file) : soundFile(file) {
}

SoundImpl::~SoundImpl() {
}

SoundFile& SoundImpl::getSoundFile() {
	return soundFile;
}

} /* namespace jukebox */
