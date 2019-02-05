/*
 * FileWriterSoundImpl.h
 *
 *  Created on: 5 de fev de 2019
 *      Author: rvelloso
 */

#ifndef JUKEBOX_SOUND_FILEWRITERSOUNDIMPL_H_
#define JUKEBOX_SOUND_FILEWRITERSOUNDIMPL_H_

#include "SoundImpl.h"

namespace jukebox {

class FileWriterSoundImpl: public SoundImpl {
public:
	FileWriterSoundImpl(Decoder *, std::string filename);
	virtual ~FileWriterSoundImpl() = default;
	void play() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	void loop(bool) override;
private:
	std::string filename;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_FILEWRITERSOUNDIMPL_H_ */
