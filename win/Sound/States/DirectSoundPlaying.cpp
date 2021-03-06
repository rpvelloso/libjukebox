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

#include <cmath>
#include <windows.h>

#include "DirectSoundPlaying.h"
#include "DirectSoundPaused.h"
#include "DirectSoundStopped.h"

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
		hr = lpds->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);

		if (FAILED(hr))
			throw std::runtime_error("failed SetCooperativeLevel");
	}
};


void ReleaseBuffer(LPDIRECTSOUNDBUFFER pDsb) {
	if (pDsb != nullptr)
		pDsb->Release();
}

DirectSoundPlaying::DirectSoundPlaying(DirectSoundState &state) :
		DirectSoundState(state),
		pDsb(nullptr, &ReleaseBuffer),
		playingStatus(PlayingStatus::STOPPED) {

	memset((void *)&wfx, 0, sizeof(wfx));
	memset((void *)&dsbdesc, 0, sizeof(dsbdesc));

	wfx.wBitsPerSample = dsound.getDecoder().getBitsPerSample();
	wfx.nChannels = dsound.getDecoder().getNumChannels();
	wfx.nSamplesPerSec = dsound.getDecoder().getSampleRate();
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample/8)*wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags =
			DSBCAPS_CTRLPAN |
			DSBCAPS_CTRLVOLUME |
			DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_CTRLPOSITIONNOTIFY |
			DSBCAPS_GLOBALFOCUS;
	/*
	 * DSBCAPS_CTRL3D				The sound source can be moved in 3D space.
	 * DSBCAPS_CTRLFX				Effects can be added to the buffer.
	 * DSBCAPS_CTRLFREQUENCY		The frequency of the sound can be changed.
	 * DSBCAPS_CTRLPAN				The sound source can be moved from left to right.
	 * DSBCAPS_CTRLPOSITIONNOTIFY	Notification positions can be set on the buffer.
	 * DSBCAPS_CTRLVOLUME			The volume of the sound can be changed.
	 */

	dsbdesc.dwBufferBytes = wfx.nBlockAlign * 1024 * 4 * 2;
	dsbdesc.lpwfxFormat = &wfx;

	LPDIRECTSOUNDBUFFER bufPtr;
	auto hr = DSoundDevice::getDevice().CreateSoundBuffer(&dsbdesc, &bufPtr, NULL);

	if (FAILED(hr))
		throw std::runtime_error("failed CreateSoundBuffer");

	pDsb.reset(bufPtr);

	DWORD playFlags = startThread();

	setVolume(volume);
    hr = pDsb->Play(
        0,	// Unused.
        0,	// Priority for voice management.
		playFlags);	// Flags.

    if (FAILED(hr))
      throw std::runtime_error("failed Play");
}

void DirectSoundPlaying::play() {
	return;
}

void DirectSoundPlaying::pause() {
	playingStatus = PlayingStatus::PAUSED;
	pDsb->Stop();
}

void DirectSoundPlaying::stop() {
	playingStatus = PlayingStatus::STOPPED;
	pDsb->Stop();
}

/*
 * dB to linear scale and vice-versa:
 * https://stackoverflow.com/questions/36072054/get-the-inverse-of-a-function-millibels-to-percentage-percentage-to-millibels
 * */
int DirectSoundPlaying::getVolume() const {
	LONG vol;
	pDsb->GetVolume(&vol);

	double exponent = ((((double)vol) / 1000.0) + 10);
	double numerator = 100.0 * (std::pow(2, exponent) - 1);
	return numerator / 1023.0;
}

void DirectSoundPlaying::setVolume(int vol) {
	double attenuation = 1.0 / 1024.0 + ((double)vol) / 100.0 * 1023.0 / 1024.0;
	double db = 10 * std::log10(attenuation) / std::log10(2);
	pDsb->SetVolume(db * 100);
	volume = vol;
}

bool DirectSoundPlaying::playing() const {
	return true; //status() & DSBSTATUS_PLAYING;
}

DWORD DirectSoundPlaying::status() const {
	DWORD st;
	pDsb->GetStatus(&st);
	return st;
}

bool DirectSoundPlaying::fillBuffer(int offset, size_t size) {
	if (dsound.getPosition() >= dsound.getDecoder().getDataSize()) {
		return false;
	}

	// fill secondary buffer with wav/sound
	LPVOID bufAddr;
	DWORD bufLen;

	auto hr = pDsb->Lock(
	      offset,	// Offset at which to start lock.
	      size,		// Size of lock.
	      &bufAddr,	// Gets address of first part of lock.
	      &bufLen,	// Gets size of first part of lock.
	      NULL,		// Address of wraparound not needed.
	      NULL,		// Size of wraparound not needed.
	      0); // Flag.

	if (FAILED(hr))
		throw std::runtime_error("failed Lock");

	dsound.processTimedEvents();
	size_t len = dsound.getDecoder().getSamples((char *)bufAddr, dsound.getPosition(), bufLen);

	dsound.setPosition(dsound.getPosition() + len);
	if (len < bufLen)
		memset((char *)bufAddr+len, dsound.getDecoder().silenceLevel(), bufLen-len);

	pDsb->Unlock(
		bufAddr,	// Address of lock start.
		bufLen,		// Size of lock.
		NULL,		// No wraparound portion.
		0);			// No wraparound size.

	return dsound.getPosition() < dsound.getDecoder().getDataSize();
}

class HandleGuard {
public:
	HandleGuard(HANDLE handle, std::atomic<PlayingStatus> &status, DirectSoundBuffer &dsound) :
		handle(handle),
		status(status),
		dsound(dsound) {
	};
	~HandleGuard() {
		CloseHandle(handle);
		if (status != PlayingStatus::PAUSED) {
			if (dsound.isLooping()) {
				dsound.setPosition(0);
				dsound.setState<DirectSoundPlaying>();
			} else {
				while (!dsound.onStopStackEmpty()) {
					dsound.popOnStopCallback()();
				}
				dsound.setState<DirectSoundStopped>();
			}

		} else {
			dsound.setState<DirectSoundPaused>();
		}
	};
private:
	HANDLE handle;
	std::atomic<PlayingStatus> &status;
	DirectSoundBuffer &dsound;
};

/* this method returns the PLAYING FLAGS for DSOUND play.
 * These flags depend whether or not we'll be refilling the playing buffer.
 */
DWORD DirectSoundPlaying::startThread() {
	LPDIRECTSOUNDNOTIFY notifyIface;
	DSBPOSITIONNOTIFY notifyPos[3];

	auto hr = pDsb->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&notifyIface);
	if (FAILED(hr))
		throw std::runtime_error("failed QueryInterface");

	notifyPos[0].dwOffset = DSBPN_OFFSETSTOP; // sound stop event
	notifyPos[0].hEventNotify = CreateEvent(nullptr, false, false, nullptr);
	notifyPos[1].dwOffset = (dsbdesc.dwBufferBytes / 2) - 1; // half way buffer event
	notifyPos[1].hEventNotify = notifyPos[0].hEventNotify;
	notifyPos[2].dwOffset = dsbdesc.dwBufferBytes - 1; // end of buffer event
	notifyPos[2].hEventNotify = notifyPos[0].hEventNotify;

    bool reload = fillBuffer(0, dsbdesc.dwBufferBytes);

	hr = notifyIface->SetNotificationPositions(reload?3:1, notifyPos);
	notifyIface->Release();
	if (FAILED(hr)) {
		CloseHandle(notifyPos[0].hEventNotify);
		throw std::runtime_error("failed SetNotificationPositions");
	}

	playingStatus = PlayingStatus::PLAYING;

	loadBufferThread = std::thread(
		[this](auto event){
			HandleGuard handleGuard(event, playingStatus, dsound);

			size_t offsets[2] = {0, dsbdesc.dwBufferBytes / 2};

			bool offset = true;
			do {
				offset = !offset;
				WaitForSingleObject(event, INFINITE);
			} while (
				playingStatus == PlayingStatus::PLAYING &&
				fillBuffer(offsets[offset], dsbdesc.dwBufferBytes / 2));

			if (playingStatus == PlayingStatus::PLAYING) {
				WaitForSingleObject(event, INFINITE);
			}
			pDsb->Stop();
		},
		notifyPos[0].hEventNotify);

	loadBufferThread.detach();

	return reload?DSBPLAY_LOOPING:0;
}

} /* namespace jukebox */
