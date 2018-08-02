/*
 * MP3DecoderImpl.cpp
 *
 *  Created on: 1 de ago de 2018
 *      Author: Benutzer
 */

#include <string.h>
#include <algorithm>
#include "MP3DecoderImpl.h"

namespace jukebox {

jukebox::MP3DecoderImpl::MP3DecoderImpl(MP3FileImpl& fileImpl) :
	DecoderImpl(fileImpl),
	fileImpl(fileImpl),
	fileBuffer(fileImpl.getFileBuffer()),
	fileSize(fileImpl.getFileSize()) {

}

int jukebox::MP3DecoderImpl::getSamples(char* buf, int pos, int len) {
	int bytesRead = 0;

	if (pos == 0)
		reset();

	while ((len > bytesRead) && (samples > 0)) {
		if (pcmPos < samples) {
			int siz = std::min(samples - pcmPos, (len - bytesRead) / 2);
			memcpy(&buf[bytesRead], &pcm[pcmPos], siz * 2);
			pcmPos += siz;
			bytesRead += (siz * 2);
		}

		if (pcmPos >= samples) {
			pcmPos = 0;
			//std::cout << offset << std::endl;
			samples = mp3dec_decode_frame(&mp3d, fileBuffer + offset, fileSize - offset, pcm, &info)*info.channels;
			offset += info.frame_bytes;
		}
	}

	return bytesRead;
}

void MP3DecoderImpl::reset() {
	pcmPos = 0;
	mp3dec_init(&mp3d);
	samples = mp3dec_decode_frame(&mp3d, fileBuffer, fileSize, pcm, &info)*info.channels;
	offset = info.frame_bytes;
}

void MP3DecoderImpl::positionMP3Stream(int pos) {
	class comp {
	public:
		bool operator()(const std::pair<int,int> &a, const std::pair<int,int> &b) const { return a.first < b.first;};
	};

	auto &frameIndex = fileImpl.getIndex();
	auto frameIt = std::lower_bound(frameIndex.begin(), frameIndex.end(), std::make_pair(pos, 0), comp());

	if (frameIt != frameIndex.end()) {
		auto frameEndPos = frameIt->first;
		auto frameOffset = frameIt->second;

		if (frameOffset != offset) {
			offset = frameOffset;
			samples = mp3dec_decode_frame(&mp3d, fileBuffer + frameOffset, fileSize - frameOffset, pcm, &info)*info.channels;
		}
		pcmPos = (pos - (frameEndPos - ((samples*2)-1))) / 2;
		//std::cout << pos << " " << frameOffset << " " << pcmPos << " " <<  samples << std::endl;
	}
}

} /* namespace jukebox */
