/*
 * MIDIDecoderImpl.cpp
 *
 *  Created on: 3 de ago de 2018
 *      Author: rvelloso
 */

#include "jukebox/FileFormats/MIDIFileImpl.h"
#include "MIDIDecoderImpl.h"

namespace jukebox {

const std::string soundFont = "C:\\mingw-w64\\home\\rvelloso\\FluidR3_GM.sf2";

void freeFluidSynthSettings(fluid_settings_t *settings) {
	if (settings)
		delete_fluid_settings(settings);
}

void freeFluidSynthSynth(fluid_synth_t *synth) {
	if (synth)
		delete_fluid_synth(synth);
}

void freeFluidSynthPlayer(fluid_player_t *player) {
	if (player)
		delete_fluid_player(player);
}

MIDIDecoderImpl::MIDIDecoderImpl(MIDIFileImpl& fileImpl) :
		DecoderImpl(fileImpl),
		fileImpl(fileImpl),
		settings(new_fluid_settings(), freeFluidSynthSettings),
		synth(new_fluid_synth(settings.get()), freeFluidSynthSynth),
		player(new_fluid_player(synth.get()), freeFluidSynthPlayer) {

    if (fluid_is_soundfont(soundFont.c_str()))
    	fluid_synth_sfload(synth.get(), soundFont.c_str(), 1);
    else
    	throw std::invalid_argument(soundFont + " is not a sound font file.");
    fluid_player_add_mem(player.get(), fileImpl.getFileBuffer(), fileImpl.getFileSize());
    fluid_player_play(player.get());
}

int MIDIDecoderImpl::getSamples(char* buf, int pos, int len) {
	if (pos == 0)
		reset();

	fluid_synth_write_s16(
			synth.get(), len/4, // 16 bit Stereo
			buf, 0, 2,
			buf, 1, 2);

	return len;
}

void MIDIDecoderImpl::reset() {
	fluid_player_join(player.get());
    fluid_player_play(player.get());
}

} /* namespace jukebox */
