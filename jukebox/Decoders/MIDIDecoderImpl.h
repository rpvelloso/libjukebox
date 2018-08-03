/*
 * MIDIDecoderImpl.h
 *
 *  Created on: 3 de ago de 2018
 *      Author: rvelloso
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
