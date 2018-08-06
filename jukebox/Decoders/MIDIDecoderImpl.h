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

#ifndef JUKEBOX_DECODERS_MIDIDECODERIMPL_H_
#define JUKEBOX_DECODERS_MIDIDECODERIMPL_H_

#include <fluidsynth.h>
#include "DecoderImpl.h"

namespace jukebox {

class MIDIFileImpl;

extern void freeFluidSynthSettings(fluid_settings_t *);
extern void freeFluidSynthSynth(fluid_synth_t *);
extern void freeFluidSynthPlayer(fluid_player_t *);

class MIDIDecoderImpl: public DecoderImpl {
public:
	MIDIDecoderImpl(MIDIFileImpl &fileImpl);
	int getSamples(char *buf, int pos, int len) override;
	virtual ~MIDIDecoderImpl() = default;
private:
	MIDIFileImpl &fileImpl;
    std::unique_ptr<fluid_settings_t, decltype(&freeFluidSynthSettings)> settings;
    std::unique_ptr<fluid_synth_t, decltype(&freeFluidSynthSynth)> synth;
    std::unique_ptr<fluid_player_t, decltype(&freeFluidSynthPlayer)> player;

    void reset();
};

} /* namespace jukebox */

#endif /* JUKEBOX_DECODERS_MIDIDECODERIMPL_H_ */
