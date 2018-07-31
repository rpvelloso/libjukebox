/*
 * MP3FileImpl.h
 *
 *  Created on: 30 de jul de 2018
 *      Author: rvelloso
 */

#ifndef JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_
#define JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_

#include <string>
#include <memory>
#include "SoundFileImpl.h"
#include "minimp3/minimp3.h"

namespace jukebox {

class MP3FileImpl : public SoundFileImpl {
public:
	MP3FileImpl(const std::string &filename);
	MP3FileImpl(std::istream& inp);
	virtual ~MP3FileImpl();
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	const std::string &getFilename() const override;
	int read(char *buf, int pos, int len) override;
private:
	std::string filename;
	mp3dec_t mp3d;
	mp3dec_frame_info_t info;
	short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
	int pcmPos = 0, samples = 0;
	std::unique_ptr<uint8_t> mp3;
	int fileSize = 0;
	int offset = 0;
	int dataSize = 0;

	std::vector<std::pair<long, long>> frameIndex; // <endPos, FramePos>
	void positionMP3Stream(int pos);
	void load(std::istream& inp);
	void reset();
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_MP3FILEIMPL_H_ */
