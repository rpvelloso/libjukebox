/*
 * FadeOnStopSoundImpl.h
 *
 *  Created on: 4 de fev de 2019
 *      Author: rvelloso
 */

#ifndef JUKEBOX_SOUND_DECORATORS_FADEONSTOPSOUNDIMPL_H_
#define JUKEBOX_SOUND_DECORATORS_FADEONSTOPSOUNDIMPL_H_

#include <memory>
#include "../SoundImpl.h"

namespace jukebox {

class FadeOnStopSoundImpl: public SoundImpl {
public:
	FadeOnStopSoundImpl(SoundImpl *impl, int);
	virtual ~FadeOnStopSoundImpl() = default;
	void play() override;
	void stop() override;
	int getVolume() const override;
	void setVolume(int) override;
	void loop(bool) override;
private:
	std::unique_ptr<SoundImpl> impl;
	int fadeOutSecs;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_DECORATORS_FADEONSTOPSOUNDIMPL_H_ */
