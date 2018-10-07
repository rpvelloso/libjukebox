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

/* Soundfonts:
 * GeneralUser GS v1.471.sf2 - 30MB -> https://www.dropbox.com/s/4x27l49kxcwamp5/GeneralUser_GS_1.471.zip?dl=1
 * FluidR3_GM.sf2 - 140MB -> https://pt.osdn.net/projects/sfnet_androidframe/downloads/soundfonts/FluidR3_GM.sf2/
 * Musyng.sf2 - 1.6GB :O -> https://drive.google.com/file/d/0B6caGN_QlJVEUzVtX0tKOWVoWXc/edit
 * GXSCC_gm_033.sf2 - 126kb 8 bit style o/ -> https://musical-artifacts.com/artifacts/9
 * */

class FluidSynthInitialization {
friend class MIDIDecoderImpl;
public:
	static const std::string &getSoundFontFilename() {
		static std::string soundFont = "../jukebox_test/data/GeneralUser GS v1.471.sf2";
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
