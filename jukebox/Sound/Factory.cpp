#include "Factory.h"
#include <exception>
#include <algorithm>

#include "jukebox/Sound/FileWriterSoundImpl.h"
#include "jukebox/FileFormats/MP3FileImpl.h"
#include "jukebox/FileFormats/VorbisFileImpl.h"
#include "jukebox/FileFormats/WaveFileImpl.h"
#include "jukebox/FileFormats/FLACFileImpl.h"
#include "jukebox/FileFormats/MIDIFileImpl.h"

namespace jukebox {
namespace factory {

// makeSoundImpl() is defined by the respective win/linux implementations

Sound makeSound(SoundFile &file) {
	return Sound(makeSoundImpl(new Decoder(file)));
}

Sound makeSound(const std::string &filename) {
	return Sound(makeSoundImpl(new Decoder(std::shared_ptr<SoundFile>(new SoundFile(loadFile(filename))))));
}

Sound makeSoundOutputToFile(SoundFile &file, const std::string &filename) {
	return Sound(new FileWriterSoundImpl(new Decoder(file), filename));
}

Sound makeSoundOutputToFile(const std::string  &inputFile, const std::string &filename) {
	return Sound(new FileWriterSoundImpl(new Decoder(std::shared_ptr<SoundFile>(new SoundFile(loadFile(inputFile)))), filename));
}

// TODO: add more extensions and/or a way to autodetect the file format
SoundFile loadFile(const std::string &filename, bool onMemory)
{
    auto fileExtension = [](std::string const & filename) {
        auto p = filename.rfind('.');
        std::string extension = p != std::string::npos ? filename.substr(p+1) : std::string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        return extension;
    };

    auto ext = fileExtension(filename);
    if (ext == "ogg") return loadVorbisFile(filename, onMemory);
    if (ext == "mp3") return loadMP3File(filename, onMemory);
    if (ext == "flac") return loadFLACFile(filename, onMemory);
    if (ext == "mid") return loadMIDIFile(filename);
    if (ext == "wav") return loadWaveFile(filename, onMemory);

    throw std::runtime_error("error loading " + filename + ". invalid extension " + ext);
}

SoundFile loadWaveFile(const std::string &filename, bool onMemory)
{
    return SoundFile(new WaveFileImpl(filename, onMemory));
}

SoundFile loadWaveStream(std::istream &inp, bool onMemory)
{
    return SoundFile(new WaveFileImpl(inp, onMemory));
}

SoundFile loadVorbisFile(const std::string &filename, bool onMemory)
{
    return SoundFile(new VorbisFileImpl(filename, onMemory));
}

SoundFile loadVorbisStream(std::istream &inp, bool onMemory)
{
    return SoundFile(new VorbisFileImpl(inp, onMemory));
}

SoundFile loadMP3File(const std::string &filename, bool onMemory)
{
    return SoundFile(new MP3FileImpl(filename, onMemory));
}

SoundFile loadMP3Stream(std::istream &inp, bool onMemory)
{
    return SoundFile(new MP3FileImpl(inp, onMemory));
}

SoundFile loadFLACFile(const std::string &filename, bool onMemory)
{
    return SoundFile(new FLACFileImpl(filename, onMemory));
}

SoundFile loadFLACStream(std::istream &inp, bool onMemory)
{
    return SoundFile(new FLACFileImpl(inp, onMemory));
}

SoundFile loadMIDIFile(const std::string &filename) {
    return SoundFile(new MIDIFileImpl(filename));
}

SoundFile loadMIDIStream(std::istream &inp) {
    return SoundFile(new MIDIFileImpl(inp));
}

}
}
