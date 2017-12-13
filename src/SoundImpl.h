/*
 * SoundImpl.h
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#ifndef SOUNDIMPL_H_
#define SOUNDIMPL_H_

namespace jukebox {

class SoundImpl {
public:
	SoundImpl();
	virtual ~SoundImpl();
	virtual void play() = 0;
};

} /* namespace jukebox */

#endif /* SOUNDIMPL_H_ */
