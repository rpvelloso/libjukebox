/*
 * AlsaHandle.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#include "AlsaHandle.h"

namespace jukebox {

void closeAlsaHandle(snd_pcm_t *handle) {
	if (handle != nullptr) {
		snd_pcm_drain(handle);
		snd_pcm_close(handle);
	}
}

AlsaHandle::AlsaHandle(SoundFile &file) : soundFile(file), handlePtr(nullptr, closeAlsaHandle) {
	snd_pcm_t *handle;
	auto res = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (res != 0)
		throw std::runtime_error("snd_pcm_open error.");

	handlePtr.reset(handle);
	prepare();
}

void AlsaHandle::play() {
	playThread = std::thread([this]() {
		size_t frameSize = (soundFile.getBitsPerSample()/8) * soundFile.getNumChannels();
		size_t numFrames = soundFile.getDataSize() / frameSize;
		const char *buf = soundFile.getData();

		while (numFrames > 0) {
			auto n = snd_pcm_writei(handlePtr.get(), buf, numFrames);
			if (n > 0) {
				numFrames -= n;
				buf += (n * frameSize);
			} else
				throw std::runtime_error("snd_pcm_writei error.");
		}
	});
}

AlsaHandle::~AlsaHandle() {
	if (playThread.joinable())
		playThread.join();
};

void AlsaHandle::prepare() {
	snd_pcm_format_t format = SND_PCM_FORMAT_S8;
	if (soundFile.getBitsPerSample() == 16)
		format = SND_PCM_FORMAT_S16_LE;

	auto res = snd_pcm_set_params(
		handlePtr.get(),
		format,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		soundFile.getNumChannels(),
		soundFile.getSampleRate(),
		1,
		500000);
	if (res != 0)
		throw std::runtime_error("snd_pcm_set_params error.");

	res = snd_pcm_prepare(handlePtr.get());
	if (res != 0)
		throw std::runtime_error("snd_pcm_prepare error.");
}

namespace factory {

Sound makeSound(SoundFile& file) {
	return Sound(new AlsaHandle(file));
}

}

} /* namespace jukebox */
