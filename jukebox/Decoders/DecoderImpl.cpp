/*
 * DecoderImpl.cpp
 *
 *  Created on: 1 de ago de 2018
 *      Author: Benutzer
 */

#include "DecoderImpl.h"
#include "jukebox/FileFormats/SoundFileImpl.h"

namespace jukebox {

DecoderImpl::DecoderImpl(SoundFileImpl& fileImpl) :
	blockSize(fileImpl.getNumChannels() * fileImpl.getBitsPerSample()/8) {
}
int DecoderImpl::getBlockSize() const {
	return blockSize;
}

}

