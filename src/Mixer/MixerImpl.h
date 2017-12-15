/*
 * MixerImpl.h
 *
 *  Created on: 15 de dez de 2017
 *      Author: rvelloso
 */

#ifndef MIXERIMPL_H_
#define MIXERIMPL_H_

namespace jukebox {

class MixerImpl {
public:
	MixerImpl();
	virtual ~MixerImpl();
	virtual int getVolume() = 0;
	virtual void setVolume(int vol) = 0;
};

} /* namespace jukebox */

#endif /* MIXERIMPL_H_ */
