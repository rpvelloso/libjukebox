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

#ifndef LIBJUKEBOX_DIRECTSOUNDBUFFER_2017_12_17_H_
#define LIBJUKEBOX_DIRECTSOUNDBUFFER_2017_12_17_H_

#include <dsound.h>
#include <mmsystem.h>

#include "jukebox/FileFormats/SoundFile.h"
#include "jukebox/Sound/SoundImpl.h"

namespace jukebox {

class DSoundDevice {
public:
	static struct IDirectSound &getDevice() {
		static DSoundDevice instance;
		return *(instance.lpds);
	}
private:
	LPDIRECTSOUND lpds;

	DSoundDevice() {
		// create device
		auto hr = DirectSoundCreate(NULL, &lpds, NULL);

		if (FAILED(hr))
			throw std::runtime_error("failed DirectSoundCreate");

		// set cooperative level to priority
		hr = lpds->SetCooperativeLevel(GetConsoleWindow(), DSSCL_PRIORITY);

		if (FAILED(hr))
			throw std::runtime_error("failed SetCooperativeLevel");
	}
};

extern void ReleaseBuffer(LPDIRECTSOUNDBUFFER);

class DirectSoundBuffer: public SoundImpl {
public:
	DirectSoundBuffer(SoundFile &file);
	void play() override;
	void stop() override;
	int getVolume() override;
	void setVolume(int) override;
private:
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbdesc;
	std::unique_ptr<struct IDirectSoundBuffer, decltype(&ReleaseBuffer)> pDsb;

	void prepare();
};

} /* namespace jukebox */

#endif
