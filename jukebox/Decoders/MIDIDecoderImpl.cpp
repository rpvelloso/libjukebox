/*
 * MIDIDecoderImpl.cpp
 *
 *  Created on: 3 de ago de 2018
 *      Author: rvelloso
 */

#include <cstring>
#include "jukebox/FileFormats/MIDIFileImpl.h"
#include "MIDIDecoderImpl.h"

namespace jukebox {

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

// empty log function to remove warning messages from console
void dummy_fluid_log_function(int level, char *	message,void * data){}

class FluidSynthInitialization {
friend class MIDIDecoderImpl;
public:
	static const std::string &getSoundFontFilename() {
		static std::string soundFont = "GeneralUser GS v1.471.sf2"; //"FluidR3_GM.sf2";
		return soundFont;
	}
private:
	FluidSynthInitialization() {
		fluid_set_log_function(FLUID_WARN, dummy_fluid_log_function, nullptr);
	};
};


MIDIDecoderImpl::MIDIDecoderImpl(MIDIFileImpl& fileImpl) :
		DecoderImpl(fileImpl),
		fileImpl(fileImpl),
		settings(new_fluid_settings(), freeFluidSynthSettings),
		synth(new_fluid_synth(settings.get()), freeFluidSynthSynth),
		player(new_fluid_player(synth.get()), freeFluidSynthPlayer) {

	static FluidSynthInitialization libfluidInit;

	if (fluid_synth_get_sfont(synth.get(), 0) == nullptr) {
		auto soundFont = libfluidInit.getSoundFontFilename();
		if (fluid_is_soundfont(soundFont.c_str()))
			fluid_synth_sfload(synth.get(), soundFont.c_str(), 1);
		else
			throw std::invalid_argument(soundFont + " is not a sound font file.");
	}
}

int MIDIDecoderImpl::getSamples(char* buf, int pos, int len) {
	if (pos == 0)
		reset();

	if (pos >= fileImpl.getDataSize())
		return 0;

	memset(buf, 0, len);

	if (pos + len > fileImpl.getDataSize())
		len = fileImpl.getDataSize() - pos;

	fluid_synth_write_s16(
			synth.get(), len/4, // 16 bit Stereo
			buf, 0, 2,
			buf, 1, 2);

	return len;
}

void MIDIDecoderImpl::reset() {
	fluid_player_stop(player.get());
	player.reset(new_fluid_player(synth.get()));
	fluid_player_add_mem(player.get(), fileImpl.getFileBuffer(), fileImpl.getFileSize());
    fluid_player_play(player.get());
}

} /* namespace jukebox */
