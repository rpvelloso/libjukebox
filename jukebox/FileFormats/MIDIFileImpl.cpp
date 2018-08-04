/*
 * MIDIFileImpl.cpp
 *
 *  Created on: 3 de ago de 2018
 *      Author: rvelloso
 */

#include <iostream>
#include <fstream>
#include <fluidsynth.h>
#include "jukebox/FileFormats/SoundFile.h"
#include "MIDIFileImpl.h"
#include "jukebox/Decoders/MIDIDecoderImpl.h"

namespace jukebox {

MIDIFileImpl::MIDIFileImpl(const std::string &filename) :
	SoundFileImpl(),
	filename(filename) {

	auto inp = std::fstream(this->filename, std::ios::binary|std::ios::in);
	load(inp);
}

MIDIFileImpl::MIDIFileImpl(std::istream& inp) :
	SoundFileImpl(),
	filename(":stream:") {

	load(inp);
}

short MIDIFileImpl::getNumChannels() const {
	return 2;
}

int MIDIFileImpl::getSampleRate() const {
	return 44100;
}

short MIDIFileImpl::getBitsPerSample() const {
	return 16;
}

int MIDIFileImpl::getDataSize() const {
	return dataSize;
}

const std::string& MIDIFileImpl::getFilename() const {
	return filename;
}

std::unique_ptr<Decoder> MIDIFileImpl::makeDecoder() {
	return std::make_unique<Decoder>(new MIDIDecoderImpl(*this));
}

extern void freeFluidSynthSettings(fluid_settings_t *);
extern void freeFluidSynthSynth(fluid_synth_t *);
extern void freeFluidSynthPlayer(fluid_player_t *);

void MIDIFileImpl::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	fileBuffer.reset(new unsigned char[fileSize]);
	inp.read((char *)fileBuffer.get(), fileSize);

    std::unique_ptr<fluid_settings_t, decltype(&freeFluidSynthSettings)> settings(new_fluid_settings(), freeFluidSynthSettings);
    std::unique_ptr<fluid_synth_t, decltype(&freeFluidSynthSynth)> synth(new_fluid_synth(settings.get()), freeFluidSynthSynth);
    std::unique_ptr<fluid_player_t, decltype(&freeFluidSynthPlayer)> player(new_fluid_player(synth.get()), freeFluidSynthPlayer);

    fluid_player_add_mem(player.get(), fileBuffer.get(), fileSize);
    //fluid_player_play(player.get());

    std::cout << fluid_player_get_total_ticks(player.get()) << " " << fluid_player_get_bpm(player.get()) << std::endl;
    std::getchar();

	dataSize =
			fluid_player_get_total_ticks(player.get())*
			fluid_player_get_bpm(player.get())*60*
			getSampleRate()*getNumChannels()*(getBitsPerSample()/8);
	dataSize = 4*10000000;
}

uint8_t* MIDIFileImpl::getFileBuffer() {
	return fileBuffer.get();
}

int MIDIFileImpl::getFileSize() {
	return fileSize;
}

namespace factory {
SoundFile loadMIDIFile(const std::string &filename) {
	return SoundFile(new MIDIFileImpl(filename));
}

SoundFile loadMIDIStream(std::istream &inp) {
	return SoundFile(new MIDIFileImpl(inp));
}
};

} /* namespace jukebox */
