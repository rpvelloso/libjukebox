/*
 * FadedSoundImpl.h
 *
 *  Created on: 1 de ago de 2018
 *      Author: rvelloso
 */

#ifndef JUKEBOX_SOUND_DECORATORS_FADEDSOUNDIMPL_H_
#define JUKEBOX_SOUND_DECORATORS_FADEDSOUNDIMPL_H_

#include <memory>
#include "../SoundImpl.h"

namespace jukebox {

class FadedSoundImpl: public SoundImpl {
public:
	FadedSoundImpl(SoundImpl *, int, int);
	virtual ~FadedSoundImpl() = default;
	void play() override;
	void stop() override;
	int getVolume() override;
	void setVolume(int) override;
	void loop(bool) override;
private:
	std::unique_ptr<SoundImpl> impl;
};

} /* namespace jukebox */

#endif /* JUKEBOX_SOUND_DECORATORS_FADEDSOUNDIMPL_H_ */
