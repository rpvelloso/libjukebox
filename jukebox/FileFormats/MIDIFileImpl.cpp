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

#include <iostream>
#include <fstream>
#include "jukebox/FileFormats/SoundFile.h"
#include "MIDIFileImpl.h"
#include "jukebox/Decoders/MIDIDecoderImpl.h"
#include "midi/MidiFile.h"

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

const std::string& MIDIFileImpl::getFilename() const {
	return filename;
}

std::unique_ptr<Decoder> MIDIFileImpl::makeDecoder() {
	return std::make_unique<Decoder>(new MIDIDecoderImpl(*this));
}

void MIDIFileImpl::load(std::istream& inp) {
	auto fileStart = inp.tellg();
	inp.seekg(0, std::ios::end);
	fileSize = inp.tellg() - fileStart;
	inp.seekg(fileStart, std::ios::beg);

	fileBuffer.reset(new unsigned char[fileSize]);
	inp.read((char *)fileBuffer.get(), fileSize);

	inp.seekg(0, std::ios::beg);
	smf::MidiFile midiFile(inp);
	midiFile.sortTracks();

	dataSize = midiFile.getFileDurationInSeconds()*
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
