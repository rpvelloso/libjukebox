/*
    Copyright 2017 Roberto Panerai Velloso.
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
#include <cstdint>
#include <unordered_map>
#include "AlsaHandle.h"
#include "jukebox/Sound/Sound.h"

#ifndef ALSA_DEVICE
#define ALSA_DEVICE "sysdefault"
#endif

#ifndef ALSA_MIN_FRAMES
#define ALSA_MIN_FRAMES (8*1024)
#endif

namespace {

constexpr size_t minFrames = ALSA_MIN_FRAMES;

class StatusGuard {
public:
  StatusGuard(
		  std::atomic<bool> &status,
		  bool entry,
		  std::function<void(void)> &onStop) :
	  status(status),
	  exitStatus(!entry),
	  onStop(onStop) {

	  status = entry;
  };

  ~StatusGuard() {
	  onStop();
	  status = exitStatus;
  }
private:
  std::atomic<bool> &status;
  bool exitStatus;

  /* this NEEDS to be passed by ref, otherwise when the user
	 * changes the event the playing thread won't be notified
	 * because it has a COPY and not the actual event handler.
	 */
  std::function<void(void)> &onStop;
};

}

namespace jukebox {

void closeAlsaHandle(snd_pcm_t *handle) {
	if (handle != nullptr) {
		snd_pcm_drain(handle);
		snd_pcm_close(handle);
	}
}

// AlsaHandle

AlsaHandle::AlsaHandle(Decoder *decoder) :
	SoundImpl(decoder),
	handlePtr(nullptr, closeAlsaHandle) {

	snd_pcm_t *handle;
	auto res = snd_pcm_open(&handle, ALSA_DEVICE, SND_PCM_STREAM_PLAYBACK, 0);
	if (res != 0)
		throw std::runtime_error("snd_pcm_open error.");

	handlePtr.reset(handle);
	config();
	prepare();

	applyVolume = applyVolumeFunc[decoder->getBitsPerSample()];
}

void AlsaHandle::play() {
	stop();

	playThread = std::thread([this]() {
		StatusGuard statusGuard(
			playing, true,
			onStop);

		size_t frameSize = (decoder->getBitsPerSample()/8) * decoder->getNumChannels();
		std::unique_ptr<uint8_t[]> volBuf(new uint8_t[minFrames*frameSize]);

		do {
			position = 0;
			size_t numFrames = decoder->getDataSize() / frameSize;

			while (numFrames > 0 && playing) {
				size_t numFrames = decoder->getDataSize() / frameSize;

				auto frames = std::min(numFrames, minFrames);
				auto bytes = decoder->getSamples(reinterpret_cast<char *>(volBuf.get()), position, frames*frameSize);

				if (bytes > 0) {
					applyVolume(*this, volBuf.get(), position, bytes);

					auto n = snd_pcm_writei(handlePtr.get(), volBuf.get(), bytes / frameSize);
					if (n > 0) {
						numFrames -= n;
						position += n * frameSize;
					} else {
						throw std::runtime_error("snd_pcm_writei error." + std::string(snd_strerror(n)));
					}
				} else
					break;
			}
		} while (looping && playing);
		snd_pcm_drain(handlePtr.get());
	});
}

std::unordered_map<short, decltype(AlsaHandle::applyVolume)> AlsaHandle::applyVolumeFunc = {
		{ 8, &AlsaHandle::_applyVolume<uint8_t>},
		{16, &AlsaHandle::_applyVolume<int16_t>},
		{32, &AlsaHandle::_applyVolume<int32_t>}
};

template<typename T>
void AlsaHandle::_applyVolume(AlsaHandle &self, void *buf, int position, int len) {

	int offset = self.decoder->silenceLevel();
	T *beginIt = reinterpret_cast<T *>(buf);
	T *endIt = beginIt + (len/sizeof(T));

	std::for_each(beginIt, endIt, [&self, offset](T &c){
		c = static_cast<T>((static_cast<double>(self.vol)/100.0*static_cast<double>(c - offset)) + offset);
	});
}

void AlsaHandle::stop() {
	playing = false;

	if (playThread.joinable())
		playThread.join();

	snd_pcm_drop(handlePtr.get());
	prepare();
}

AlsaHandle::~AlsaHandle() {
	stop();
};

void AlsaHandle::config() {
  auto res = snd_pcm_set_params(
    handlePtr.get(),
	decoder->getBitsPerSample() == 32 ? SND_PCM_FORMAT_S32_LE :
	decoder->getBitsPerSample() == 16 ? SND_PCM_FORMAT_S16_LE :
	SND_PCM_FORMAT_U8,
	SND_PCM_ACCESS_RW_INTERLEAVED,
	decoder->getNumChannels(),
	decoder->getSampleRate(),
    1,
    500000);
  if (res != 0)
    throw std::runtime_error("snd_pcm_set_params error.");
}

void AlsaHandle::loop(bool l) {
	looping = l;
}

void AlsaHandle::prepare() {
	auto res = snd_pcm_prepare(handlePtr.get());
	if (res != 0)
		throw std::runtime_error("snd_pcm_prepare error.");

	playing = false;
}

int AlsaHandle::getVolume() const {
	return vol;
}

void AlsaHandle::setVolume(int vol) {
	this->vol = vol;
}

namespace factory {

SoundImpl *makeSoundImpl(Decoder *decoder) {
	return new AlsaHandle(decoder);
}

}

} /* namespace jukebox */
