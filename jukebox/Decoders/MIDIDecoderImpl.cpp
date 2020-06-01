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

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <functional>
#include <iostream>
#include "fluidsynth/soundfont.h"
#include "jukebox/FileFormats/MIDIFileImpl.h"
#include "MIDIDecoderImpl.h"
#include "MIDIConfigurator.h"

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

class SoundFontMemFile {
public:
	SoundFontMemFile() {}
	~SoundFontMemFile() = default;

	static void *mem_sf_open(const char *filename) {
		return (void *)new SoundFontMemFile();
	}

	static int mem_sf_read(void *buf, int count, void *handle) {
		if (
			!buf || 
			!handle || 
			count <= 0) {

			return FLUID_FAILED;
		}

		SoundFontMemFile *sffile = (SoundFontMemFile *)handle;
		auto pos = sffile->getPos();
		
		if (pos + count > sf_len) {
			return FLUID_FAILED;
		}

		memcpy(buf, &sf[pos], count);
		sffile->setPos(pos + count);

		return FLUID_OK;
	}

	static int mem_sf_seek(void *handle, long offset, int origin) {
		if (!handle) {
			return FLUID_FAILED;
		}

		SoundFontMemFile *sffile = (SoundFontMemFile *)handle;
		auto pos = sffile->getPos();
		long newPos;

		switch (origin) {
			case SEEK_SET:
				newPos = offset;
				break;
			case SEEK_CUR:
				newPos = pos + offset;
				break;
			case SEEK_END:
				newPos = sf_len - offset;
				break;
			default:
				return FLUID_FAILED;
		}

		if (newPos < 0 || newPos > sf_len) {
			return FLUID_FAILED;
		}

		sffile->setPos(newPos);
		return FLUID_OK;
	}

	static int mem_sf_close(void *handle) {
		if (handle) {
			delete ((SoundFontMemFile *)handle);
			return FLUID_OK;
		}
		return FLUID_FAILED;
	}

	static long mem_sf_tell(void *handle) {
		if (handle) {
			return ((SoundFontMemFile *)handle)->getPos();
		}
		return 0;
	}

	static void add_mem_sf_loader(fluid_settings_t *settings, fluid_synth_t *synth) {
		fluid_sfloader_t *mem_sf_sfloader = new_fluid_defsfloader(settings);
		fluid_sfloader_set_callbacks(
			mem_sf_sfloader,
			SoundFontMemFile::mem_sf_open,
			SoundFontMemFile::mem_sf_read,
			SoundFontMemFile::mem_sf_seek,
			SoundFontMemFile::mem_sf_tell,
			SoundFontMemFile::mem_sf_close);
		fluid_synth_add_sfloader(synth, mem_sf_sfloader);
	}

	void setPos(long pos) {
		mem_pos = pos;
	}

	long getPos() const {
		return mem_pos;
	}
private:
	long mem_pos = 0;
};

/* Soundfonts:
 * GeneralUser GS v1.471.sf2 - 30MB -> https://www.dropbox.com/s/4x27l49kxcwamp5/GeneralUser_GS_1.471.zip?dl=1
 * FluidR3_GM.sf2 - 140MB -> https://pt.osdn.net/projects/sfnet_androidframe/downloads/soundfonts/FluidR3_GM.sf2/
 * Musyng.sf2 - 1.6GB :O -> https://drive.google.com/file/d/0B6caGN_QlJVEUzVtX0tKOWVoWXc/edit
 * GXSCC_gm_033.sf2 - 126kb 8 bit style o/ -> https://musical-artifacts.com/artifacts/9
 * MEMORY EMBEDDED SF: Famicon.sf2, 53kb, 8 bit style, https://www.woolyss.com/chipmusic-soundfonts.php
 * */

const std::string &MIDIConfigurator::getSoundFont() const {
	return soundFontPath;
}

void MIDIConfigurator::setSoundFont(const std::string &sfPath) {
	soundFontPath = sfPath;
}

MIDIConfigurator &MIDIConfigurator::getInstance() {
	if (instance.get() == nullptr)
		instance.reset(new MIDIConfigurator());
	return *instance;
}

MIDIConfigurator::MIDIConfigurator() {
	fluid_set_log_function(FLUID_WARN, dummy_fluid_log_function, nullptr);
}

std::unique_ptr<MIDIConfigurator> MIDIConfigurator::instance(nullptr);


MIDIDecoderImpl::MIDIDecoderImpl(MIDIFileImpl& fileImpl) :
		DecoderImpl(fileImpl),
		fileImpl(fileImpl),
		settings(new_fluid_settings(), freeFluidSynthSettings),
		synth(new_fluid_synth(settings.get()), freeFluidSynthSynth),
		player(new_fluid_player(synth.get()), freeFluidSynthPlayer) {

	auto &midiConfig = MIDIConfigurator::getInstance();

	if (fluid_synth_get_sfont(synth.get(), 0) == nullptr) {
		auto soundFont = midiConfig.getSoundFont();
		if (!fluid_is_soundfont(soundFont.c_str())) { // invalid SF or not set by the user
			SoundFontMemFile::add_mem_sf_loader(settings.get(), synth.get()); // load embedded SF from memory
		}

		if (fluid_synth_sfload(synth.get(), soundFont.c_str(), 1) == FLUID_FAILED) {
			throw std::runtime_error("unable to load soundfont");
		}
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
	fluid_player_add_mem(player.get(), fileImpl.getMemoryBuffer(), fileImpl.getBufferSize());
    fluid_player_play(player.get());
}

} /* namespace jukebox */
