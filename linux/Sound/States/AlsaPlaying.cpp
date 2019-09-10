/*
    Copyright 2019 Roberto Panerai Velloso.
    This file is part of libjukebox.
    libjukebox is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libjukebox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libjukebox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <algorithm>

#include "AlsaPlaying.h"
#include "AlsaPaused.h"

#ifndef ALSA_DEVICE
#define ALSA_DEVICE "sysdefault"
#endif

namespace jukebox {

class StatusGuard {
public:
	StatusGuard(
			std::atomic<bool> &status,
			bool entry,
			std::vector<std::function<void(void)>> &onStopStack) :
				status(status),
				exitStatus(!entry),
				onStopStack(onStopStack) {

		status = entry;
	};

	~StatusGuard() {
		while (!onStopStack.empty()) {
			onStopStack.back()();
			onStopStack.pop_back();
		}
		status = exitStatus;
	}
private:
	std::atomic<bool> &status;
	bool exitStatus;

	/* this NEEDS to be passed by ref, otherwise when the user
	 * changes the event the playing thread won't be notified
	 * because it has a COPY and not the actual event handler.
	 */
	std::vector<std::function<void(void)>> &onStopStack;
};

void closeAlsaHandle(snd_pcm_t *handle) {
	if (handle != nullptr) {
		snd_pcm_drop(handle);
		snd_pcm_close(handle);
	}
}

std::unordered_map<short, decltype(AlsaPlaying::applyVolume)> AlsaPlaying::applyVolumeFunc = {
		{ 8, &AlsaPlaying::_applyVolume<uint8_t>},
		{16, &AlsaPlaying::_applyVolume<int16_t>},
		{32, &AlsaPlaying::_applyVolume<int32_t>}
};

AlsaPlaying::AlsaPlaying(AlsaHandle &alsa) :
			AlsaState(alsa),
			handlePtr(nullptr, closeAlsaHandle) {

	snd_pcm_t *handle;
	auto res = snd_pcm_open(&handle, ALSA_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
	if (res != 0)
		throw std::runtime_error("snd_pcm_open error.");

	handlePtr.reset(handle);

	applyVolume = applyVolumeFunc[alsa.getDecoder().getBitsPerSample()];

	res = snd_pcm_set_params(
		handlePtr.get(),
		alsa.getDecoder().getBitsPerSample() == 32 ? SND_PCM_FORMAT_S32_LE :
				alsa.getDecoder().getBitsPerSample() == 16 ? SND_PCM_FORMAT_S16_LE :
				SND_PCM_FORMAT_U8,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		alsa.getDecoder().getNumChannels(),
		alsa.getDecoder().getSampleRate(),
		1,
		100000);

	if (res != 0)
		throw std::runtime_error("snd_pcm_set_params error.");

	snd_pcm_uframes_t period;
	snd_pcm_get_params(handlePtr.get(), &bufferSize, &period);

	clearBuffer = snd_pcm_drain;
	res = snd_pcm_prepare(handlePtr.get());
	if (res != 0)
		throw std::runtime_error("snd_pcm_prepare error.");

	if (playThread.joinable())
		playThread.join();

	isPlaying = true;

	playThread = std::thread([this, &alsa]() {
		StatusGuard statusGuard(
				isPlaying, true,
				alsa.getOnStopStack());

		std::unique_ptr<uint8_t[]> volBuf(new uint8_t[bufferSize*alsa.getDecoder().getBlockSize()]);

		do {
			size_t numFrames = alsa.getDecoder().getDataSize() / alsa.getDecoder().getBlockSize();

			while (numFrames > 0 && isPlaying) {
				auto frames = std::min(numFrames, bufferSize);
				alsa.processTimedEvents();
				auto bytes = alsa.getDecoder().getSamples(
						reinterpret_cast<char *>(volBuf.get()),
						alsa.getPosition(),
						frames*alsa.getDecoder().getBlockSize());

				if (bytes > 0) {
					applyVolume(alsa, volBuf.get(), alsa.getPosition(), bytes);

					auto n = snd_pcm_writei(handlePtr.get(), volBuf.get(), bytes / alsa.getDecoder().getBlockSize());
					if (n > 0) {
						numFrames -= n;
						alsa.setPosition(alsa.getPosition() + (n * alsa.getDecoder().getBlockSize()));
					} else
						break;
				} else
					break;
			}
			if (numFrames == 0)
				alsa.setPosition(0);
		} while (alsa.isLooping() && isPlaying);
		clearBuffer(handlePtr.get());
		clearBuffer = snd_pcm_drain;
	});
}

AlsaPlaying::~AlsaPlaying() {
	if (playThread.joinable())
		playThread.join();
}

void AlsaPlaying::play() {
	return;
}

void AlsaPlaying::pause() {
	clearBuffer = snd_pcm_drop;
	isPlaying = false;
	alsa.setState(new AlsaPaused(alsa));
}

int AlsaPlaying::getVolume() const {
	return vol;
}

void AlsaPlaying::setVolume(int vol) {
	this->vol = vol;
}

bool AlsaPlaying::playing() const {
	return isPlaying;
}

template<typename T>
void AlsaPlaying::_applyVolume(AlsaHandle &self, void *buf, int position, int len) {

	int offset = self.getDecoder().silenceLevel();
	T *beginIt = reinterpret_cast<T *>(buf);
	T *endIt = beginIt + (len/sizeof(T));

	std::for_each(beginIt, endIt, [&self, offset](T &c){
		c = static_cast<T>((static_cast<double>(self.getVolume())/100.0*static_cast<double>(c - offset)) + offset);
	});
}

} /* namespace jukebox */
