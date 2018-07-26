/*
 * FadedSoundFileImpl.h
 *
 *  Created on: 26 de jul de 2018
 *      Author: rvelloso
 */

#ifndef JUKEBOX_FILEFORMATS_FADEDSOUNDFILEIMPL_H_
#define JUKEBOX_FILEFORMATS_FADEDSOUNDFILEIMPL_H_

#include <memory>
#include "SoundFileImpl.h"

namespace jukebox {

class FadedSoundFileImpl: public SoundFileImpl {
public:
	FadedSoundFileImpl(SoundFileImpl *impl, int fadeInSecs, int fadeOutSecs);
	virtual ~FadedSoundFileImpl() = default;
	short getNumChannels() const override;
	int getSampleRate() const override;
	short getBitsPerSample() const override;
	int getDataSize() const override;
	const std::string &getFilename() const override;
	int read(char *buf, int pos, int len) override;
private:
	template<typename T>
	void fadeIn(char* buf, int pos, int len);

	template<typename T>
	void fadeOut(char* buf, int pos, int len);

	std::unique_ptr<SoundFileImpl> impl;
	int fadeInSecs, fadeOutSecs;
	int fadeInEndPos, fadeOutStartPos;
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_FADEDSOUNDFILEIMPL_H_ */
