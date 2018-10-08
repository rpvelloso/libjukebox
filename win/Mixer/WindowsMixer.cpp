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

#include <windows.h>
#include <stdexcept>
#include <exception>
#include <string>

#include "WindowsMixer.h"

namespace jukebox {

WindowsMixer::WindowsMixer(DWORD device) {
	auto res = mixerOpen(&hMixer, MIXER_OBJECTF_MIXER, 0, 0, 0);
	if (res != MMSYSERR_NOERROR)
		throw std::runtime_error("mixerOpen error " + std::to_string(res));

	MIXERLINE ml = {0};

	ml.cbStruct = sizeof(MIXERLINE);
	ml.dwComponentType = device;
	res = mixerGetLineInfo((HMIXEROBJ) hMixer, &ml, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (res != MMSYSERR_NOERROR)
		throw std::runtime_error("mixerGetLineInfo error " + std::to_string(res));

	MIXERLINECONTROLS mlc = {0};
	MIXERCONTROL mc = {0};

	mlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mlc.dwLineID = ml.dwLineID;
	mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mlc.cControls = 1;
	mlc.pamxctrl = &mc;
	mlc.cbmxctrl = sizeof(MIXERCONTROL);
	res = mixerGetLineControls((HMIXEROBJ) hMixer, &mlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
	if (res != MMSYSERR_NOERROR)
		throw std::runtime_error("mixerGetLineControls error " + std::to_string(res));

	controlId = mc.dwControlID;
}

WindowsMixer::~WindowsMixer() {
	mixerClose(hMixer);
}

int WindowsMixer::getVolume() {
	MIXERCONTROLDETAILS mcd = {0};
	MIXERCONTROLDETAILS_UNSIGNED mcdu;

	mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcd.hwndOwner = 0;
	mcd.dwControlID = controlId;
	mcd.paDetails = &mcdu;
	mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcd.cChannels = 1;
	auto res = mixerGetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_GETCONTROLDETAILSF_VALUE);
	if (res != MMSYSERR_NOERROR)
		throw std::runtime_error("mixerGetLineControls error " + std::to_string(res));

	// the volume is a number between 0 and 65535
	return static_cast<int>(100.0*static_cast<double>(mcdu.dwValue)/65535.0);
}

void WindowsMixer::setVolume(int vol) {
	MIXERCONTROLDETAILS mcd = {0};
	MIXERCONTROLDETAILS_UNSIGNED mcdu;

	// the volume is a number between 0 and 65535
	mcdu.dwValue = static_cast<DWORD>(65535.0*(static_cast<double>(vol)/100.0));
	mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcd.hwndOwner = 0;
	mcd.dwControlID = controlId;
	mcd.paDetails = &mcdu;
	mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcd.cChannels = 1;
	auto res = mixerSetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE);
	if (res != MMSYSERR_NOERROR)
		throw std::runtime_error("mixerGetLineControls error " + std::to_string(res));
}

namespace factory {
	MixerImpl &makeMixerImpl() {
		static WindowsMixer mixerImpl(MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT);
		return mixerImpl;
	}
}

} /* namespace jukebox */
