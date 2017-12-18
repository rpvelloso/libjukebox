/*
 * DirectSoundBuffer.cpp
 *
 *  Created on: 13 de dez de 2017
 *      Author: rvelloso
 */

#include <iostream>
#include <string.h>
#include <cmath>

#include "Sound/DirectSoundBuffer.h"

namespace jukebox {

void ReleaseBuffer(LPDIRECTSOUNDBUFFER pDsb) {
	if (pDsb != nullptr)
		pDsb->Release();
};

DirectSoundBuffer::DirectSoundBuffer(SoundFile &file) :
	SoundImpl(file),
	pDsb(nullptr, ReleaseBuffer) {

	prepare();
}

void DirectSoundBuffer::play() {
  DWORD status;

  pDsb->GetStatus(&status);

  if (status != DSBSTATUS_PLAYING) {
    // rewind the sound
    pDsb->SetCurrentPosition(0);

    auto hr = pDsb->Play(
        0,	// Unused.
        0,	// Priority for voice management.
        0);	// Flags.

    if (FAILED(hr))
      throw std::runtime_error("failed Play");
  }
}

void DirectSoundBuffer::stop() {
	auto hr = pDsb->Stop();
	if (FAILED(hr))
		throw std::runtime_error("failed Stop");
}

void DirectSoundBuffer::prepare() {
	memset((void *)&wfx, 0, sizeof(wfx));
	memset((void *)&dsbdesc, 0, sizeof(dsbdesc));

	wfx.wBitsPerSample = soundFile.getBitsPerSample();
	wfx.nChannels = soundFile.getNumChannels();
	wfx.nSamplesPerSec = soundFile.getSampleRate();
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nBlockAlign = (wfx.wBitsPerSample/8)*wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags =
			DSBCAPS_CTRLPAN |
			DSBCAPS_CTRLVOLUME |
			DSBCAPS_CTRLFREQUENCY |
			DSBCAPS_GLOBALFOCUS;
	/*
	 * DSBCAPS_CTRL3D				The sound source can be moved in 3D space.
	 * DSBCAPS_CTRLFX				Effects can be added to the buffer.
	 * DSBCAPS_CTRLFREQUENCY		The frequency of the sound can be changed.
	 * DSBCAPS_CTRLPAN				The sound source can be moved from left to right.
	 * DSBCAPS_CTRLPOSITIONNOTIFY	Notification positions can be set on the buffer.
	 * DSBCAPS_CTRLVOLUME			The volume of the sound can be changed.
	 */

	dsbdesc.dwBufferBytes = soundFile.getDataSize();
	dsbdesc.lpwfxFormat = &wfx;

	LPDIRECTSOUNDBUFFER bufPtr;
	auto hr = DSoundDevice::getDevice().CreateSoundBuffer(&dsbdesc, &bufPtr, NULL);

	if (FAILED(hr))
		throw std::runtime_error("failed CreateSoundBuffer");

	pDsb.reset(bufPtr);

	// fill secondary buffer with wav/sound
	LPVOID bufAddr;
	DWORD bufLen;

	hr = pDsb->Lock(
	      0,		// Offset at which to start lock.
	      0,		// Size of lock; ignored because of flag.
	      &bufAddr,	// Gets address of first part of lock.
	      &bufLen,	// Gets size of first part of lock.
	      NULL,		// Address of wraparound not needed.
	      NULL,		// Size of wraparound not needed.
	      DSBLOCK_ENTIREBUFFER); // Flag.

	if (FAILED(hr))
		throw std::runtime_error("failed Lock");

	memcpy((void *)bufAddr, soundFile.getData(), (size_t)bufLen);
	pDsb->Unlock(
		bufAddr,	// Address of lock start.
		bufLen,		// Size of lock.
		NULL,		// No wraparound portion.
		0);			// No wraparound size.
}

/*
 * dB to linear scale and vice-versa:
 * https://stackoverflow.com/questions/36072054/get-the-inverse-of-a-function-millibels-to-percentage-percentage-to-millibels
 * */

int DirectSoundBuffer::getVolume() {
	LONG vol;
	pDsb->GetVolume(&vol);

	double exponent = ((((double)vol) / 1000.0) + 10);
	double numerator = 100.0 * (std::pow(2, exponent) - 1);
	return numerator / 1023.0;
}

void DirectSoundBuffer::setVolume(int vol) {
	double attenuation = 1.0 / 1024.0 + ((double)vol) / 100.0 * 1023.0 / 1024.0;
	double db = 10 * std::log10(attenuation) / std::log10(2);
	std::cout << db * 100 << std::endl;
	pDsb->SetVolume(db * 100);
}

namespace factory {

Sound makeSound(SoundFile& file) {
	return Sound(new DirectSoundBuffer(file));
}

}

} /* namespace jukebox */
