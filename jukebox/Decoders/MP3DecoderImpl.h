/*
 * MP3DecoderImpl.h
 *
 *  Created on: 1 de ago de 2018
 *      Author: Benutzer
 */

#ifndef JUKEBOX_DECODERS_MP3DECODERIMPL_H_
#define JUKEBOX_DECODERS_MP3DECODERIMPL_H_

#include "DecoderImpl.h"
#include "jukebox/FileFormats/MP3FileImpl.h"
#include "jukebox/Decoders/minimp3/minimp3.h"

namespace jukebox {

class MP3DecoderImpl: public DecoderImpl {
public:
	MP3DecoderImpl(MP3FileImpl &fileImpl);
	virtual ~MP3DecoderImpl() = default;
	int getSamples(char *buf, int pos, int len) override;
private:
	MP3FileImpl &fileImpl;
	short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
	int pcmPos = 0, samples = 0;
	int offset = 0;
	uint8_t *fileBuffer;
	int fileSize;
	mp3dec_t mp3d;
	mp3dec_frame_info_t info;

	void reset();
	void positionMP3Stream(int pos);
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_MP3DECODERIMPL_H_ */
