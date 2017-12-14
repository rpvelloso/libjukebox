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

#include "AlsaHandle.h"
#include "AlsaMixer.h"

namespace jukebox {

AlsaMixer alsaMixer("default", "PCM");

void closeAlsaHandle(snd_pcm_t *handle) {
	if (handle != nullptr) {
		snd_pcm_drain(handle);
		snd_pcm_close(handle);
	}
}

AlsaHandle::AlsaHandle(SoundFile &file) :
	SoundImpl(file),
	handlePtr(nullptr, closeAlsaHandle) {

	snd_pcm_t *handle;
	auto res = snd_pcm_open(&handle, "plug:dmix", SND_PCM_STREAM_PLAYBACK, 0);
	if (res != 0)
		throw std::runtime_error("snd_pcm_open error.");

	handlePtr.reset(handle);
	config();
	prepare();
}

class StatusGuard {
public:
  StatusGuard(std::atomic<bool> &status, bool entry) :
	  status(status),
	  exitStatus(!entry) {

	  status = entry;
  };
  ~StatusGuard() {
	  status = exitStatus;
  }
private:
  std::atomic<bool> &status;
  bool exitStatus;
};

void AlsaHandle::play() {
	static size_t minFrames = 100;

	playThread = std::thread([this]() {
		StatusGuard statusGuard(playing, true);

		size_t frameSize = (soundFile.getBitsPerSample()/8) * soundFile.getNumChannels();
		size_t numFrames = soundFile.getDataSize() / frameSize;
		const char *buf = soundFile.getData();

		while (numFrames > 0 && playing) {
			auto n = snd_pcm_writei(handlePtr.get(), buf, std::min(numFrames, minFrames));
			if (n > 0) {
				numFrames -= n;
				buf += (n * frameSize);
			} else
				throw std::runtime_error("snd_pcm_writei error.");
		}
	});
}

void AlsaHandle::stop() {
	if (playing)
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
}

void AlsaHandle::prepare() {
	auto res = snd_pcm_prepare(handlePtr.get());
	if (res != 0)
		throw std::runtime_error("snd_pcm_prepare error.");

	playing = false;
}

int AlsaHandle::getVolume() {
	return alsaMixer.getVolume();
}

void AlsaHandle::setVolume(int vol) {
	alsaMixer.setVolume(vol);
}

namespace factory {

Sound makeSound(SoundFile& file) {
	return Sound(new AlsaHandle(file));
}

}

} /* namespace jukebox */
