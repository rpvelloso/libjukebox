/*
 * SoundTransformation.h
 *
 *  Created on: 27 de jul de 2018
 *      Author: rvelloso
 */

#ifndef WIN_SOUND_SOUNDTRANSFORMATION_H_
#define WIN_SOUND_SOUNDTRANSFORMATION_H_

#include "jukebox/FileFormats/SoundFile.h"

namespace jukebox {

class SoundTransformation {
public:
	SoundTransformation(SoundFile &soundFile) : soundFile(soundFile) {};
	virtual ~SoundTransformation() = default;
	virtual void operator()(void *, int, int) = 0;
protected:
	SoundFile &soundFile;
};

} /* namespace jukebox */

#endif /* WIN_SOUND_SOUNDTRANSFORMATION_H_ */
