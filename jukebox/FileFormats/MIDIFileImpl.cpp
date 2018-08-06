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

// empty log function to remove warning messages from console
void dummy_fluid_log_function(int level, char *	message,void * data){}

class FluidSynthInitialization {
friend class MIDIFileImpl;
public:
private:
	FluidSynthInitialization() {
		fluid_set_log_function(FLUID_WARN, dummy_fluid_log_function, nullptr);
	};
};

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

const std::string& MIDIFileImpl::getFilename() const {
	return filename;
}

std::unique_ptr<Decoder> MIDIFileImpl::makeDecoder() {
	return std::make_unique<Decoder>(new MIDIDecoderImpl(*this));
}

extern void freeFluidSynthSettings(fluid_settings_t *);
extern void freeFluidSynthSynth(fluid_synth_t *);
extern void freeFluidSynthPlayer(fluid_player_t *);

#define MAX_NUMBER_OF_TRACKS 128

struct __fluid_player_t
{
    int status;
    int ntracks;
    void *track[MAX_NUMBER_OF_TRACKS];
    fluid_synth_t *synth;
    void *system_timer;
    void *sample_timer;

    int loop; /* -1 = loop infinitely, otherwise times left to loop the playlist */
    void *playlist; /* List of fluid_playlist_item* objects */
    void *currentfile; /* points to an item in files, or NULL if not playing */

    char send_program_change; /* should we ignore the program changes? */
    char use_system_timer;   /* if zero, use sample timers, otherwise use system clock timer */
    char reset_synth_between_songs; /* 1 if system reset should be sent to the synth between songs. */
    int seek_ticks;           /* new position in tempo ticks (midi ticks) for seeking */
    int start_ticks;          /* the number of tempo ticks passed at the last tempo change */
    int cur_ticks;            /* the number of tempo ticks passed */
    int begin_msec;           /* the time (msec) of the beginning of the file */
    int start_msec;           /* the start time of the last tempo change */
    int cur_msec;             /* the current time */
    int miditempo;            /* as indicated by MIDI SetTempo: n 24th of a usec per midi-clock. bravo! */
    double deltatime;         /* milliseconds per midi tick. depends on set-tempo */
    unsigned int division;

    handle_midi_event_func_t playback_callback; /* function fired on each midi event as it is played */
    void *playback_userdata; /* pointer to user-defined data passed to playback_callback function */
};


void MIDIFileImpl::load(std::istream& inp) {
	static FluidSynthInitialization fluidInit;

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
    fluid_player_play(player.get());
    char tmp[4];
	fluid_synth_write_s16(synth.get(), 1, tmp, 0, 2, tmp, 1, 2); // initializes the sequence

    //TODO: very ugly!!! Encapsulation breach! But there is no interface to access this field
    auto deltatime = ((__fluid_player_t *)(player.get()))->deltatime;
	dataSize = ((fluid_player_get_total_ticks(player.get())*deltatime)/1000)*
			getSampleRate()*getNumChannels()*(getBitsPerSample()/8);
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
