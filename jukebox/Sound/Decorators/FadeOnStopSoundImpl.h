/*
 * FadeOnStopSound.h
 *
 *  Created on: 27 de jul de 2018
 *      Author: rvelloso
 */

#ifndef JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_
#define JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_

#include "../SoundImpl.h"

namespace jukebox {

class FadeOnStopSoundImpl: public SoundImpl {
public:
	FadeOnStopSoundImpl(SoundImpl *, int);
	virtual ~FadeOnStopSoundImpl() = default;
	void play() override;
	void stop() override;
	int getVolume() override;
	void setVolume(int) override;
	void loop(bool) override;
private:
	std::unique_ptr<SoundImpl> impl;
	int fadeOutSecs;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_FADEONSTOPSOUNDIMPL_H_ */
