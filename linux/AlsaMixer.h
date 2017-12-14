/*
 * AlsaMixer.h
 *
 *  Created on: 14 de dez de 2017
 *      Author: rvelloso
 */

#ifndef LINUX_ALSAMIXER_H_
#define LINUX_ALSAMIXER_H_

#include <memory>
#include <alsa/asoundlib.h>

namespace jukebox {

void closeMixer(snd_mixer_t *handle);

class AlsaMixer {
public:
	AlsaMixer(const std::string &devName, const std::string &element);
	int getVolume();
	void setVolume(int vol);
private:
	std::unique_ptr<snd_mixer_t, decltype(&closeMixer)> handlePtr;
	snd_mixer_elem_t* element_handle = nullptr;
	long minVolume, maxVolume;
};

} /* namespace jukebox */

#endif /* LINUX_ALSAMIXER_H_ */
