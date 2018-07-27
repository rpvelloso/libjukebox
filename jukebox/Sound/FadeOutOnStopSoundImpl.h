/*
 * FadeOutOnStopSoundImpl.h
 *
 *  Created on: 27 de jul de 2018
 *      Author: rvelloso
 */

#ifndef JUKEBOX_SOUND_FADEOUTONSTOPSOUNDIMPL_H_
#define JUKEBOX_SOUND_FADEOUTONSTOPSOUNDIMPL_H_

#include <memory>
#include "SoundImpl.h"

namespace jukebox {

class FadeOutOnStopSoundImpl: public SoundImpl {
public:
	FadeOutOnStopSoundImpl(SoundImpl *impl);
	virtual ~FadeOutOnStopSoundImpl() = default;
private:
	std::unique_ptr<SoundImpl> impl;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_FADEOUTONSTOPSOUNDIMPL_H_ */
