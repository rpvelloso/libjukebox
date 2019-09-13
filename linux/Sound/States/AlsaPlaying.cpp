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
#include "AlsaStopped.h"

#ifndef ALSA_DEVICE
#define ALSA_DEVICE "sysdefault"
#endif

namespace jukebox {

class StatusGuard {
public:
	StatusGuard(AlsaHandle &alsa, std::atomic<PlayingStatus> &status) :
		alsa(alsa),
		status(status) {

		status = PlayingStatus::PLAYING;
	};

	~StatusGuard() {
		if (status != PlayingStatus::PAUSED) {
			while (!alsa.onStopStackEmpty()) {
				alsa.popOnStopCallback()();
			}
		}

		if (status == PlayingStatus::PLAYING) {
			alsa.setState(new AlsaStopped(alsa));
		}
	}
private:
	AlsaHandle &alsa;
	std::atomic<PlayingStatus> &status;
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

AlsaPlaying::AlsaPlaying(AlsaHandle &alsaRef) :
			AlsaState(alsaRef),
			handlePtr(nullptr, closeAlsaHandle),
			playingStatus(PlayingStatus::STOPPED) {

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

	playThread = std::thread([this]() {
		StatusGuard statusGuard(alsa, playingStatus);

		auto &decoder = alsa.getDecoder();
		std::unique_ptr<uint8_t[]> volBuf(new uint8_t[bufferSize*decoder.getBlockSize()]);

		do {
			size_t numFrames = decoder.getDataSize() / decoder.getBlockSize();

			while (numFrames > 0 && playingStatus == PlayingStatus::PLAYING) {
				auto frames = std::min(numFrames, bufferSize);
				alsa.processTimedEvents();
				auto bytes = decoder.getSamples(
						reinterpret_cast<char *>(volBuf.get()),
						alsa.getPosition(),
						frames*decoder.getBlockSize());

				if (bytes > 0) {
					applyVolume(alsa, volBuf.get(), alsa.getPosition(), bytes);

					auto n = snd_pcm_writei(handlePtr.get(), volBuf.get(), bytes / decoder.getBlockSize());
					if (n > 0) {
						numFrames -= n;
						alsa.setPosition(alsa.getPosition() + (n * decoder.getBlockSize()));
					} else
						break;
				} else
					break;
			}
			if (alsa.isLooping()) {
				alsa.setPosition(0);
			}
		} while (alsa.isLooping() && playingStatus == PlayingStatus::PLAYING);
		clearBuffer(handlePtr.get());
	});
	playThread.detach();
}

void AlsaPlaying::play() {
	return;
}

void AlsaPlaying::pause() {
	clearBuffer = snd_pcm_drop;
	playingStatus = PlayingStatus::PAUSED;
	alsa.setState(new AlsaPaused(alsa));
}

void AlsaPlaying::stop() {
	clearBuffer = snd_pcm_drop;
	playingStatus = PlayingStatus::STOPPED;
	alsa.setState(new AlsaStopped(alsa));
}

int AlsaPlaying::getVolume() const {
	return vol;
}

void AlsaPlaying::setVolume(int vol) {
	this->vol = vol;
}

bool AlsaPlaying::playing() const {
	return playingStatus == PlayingStatus::PLAYING;
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
