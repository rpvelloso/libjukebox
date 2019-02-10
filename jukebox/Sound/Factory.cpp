#include "Factory.h"
#include <exception>
#include <algorithm>

#include "jukebox/Sound/SoundBuilder.h"
#include "jukebox/Sound/FileWriterSoundImpl.h"
#include "jukebox/FileFormats/MP3FileImpl.h"
#include "jukebox/FileFormats/VorbisFileImpl.h"
#include "jukebox/FileFormats/WaveFileImpl.h"
#include "jukebox/FileFormats/FLACFileImpl.h"

namespace jukebox {
namespace factory {

// makeSoundImpl() is defined by the respective win/linux implementations

Sound makeSound(SoundFile &file) {
	return Sound(makeSoundImpl(new Decoder(file.makeDecoder())));
}

Sound makeSoundOutputToFile(SoundFile &file, std::string filename) {
	return Sound(new FileWriterSoundImpl(new Decoder(file.makeDecoder()), filename));
}

Sound makeFadeOnStopSound(SoundFile &file, int fadeOutSecs)
{
	auto sound = makeSound(file);
	SoundBuilder builder(sound);

	builder.fadeOnStop(fadeOutSecs);
	return sound;
}

Sound makeFadedSound(SoundFile &file, int fadeInSecs, int fadeOutSecs)
{
	auto sound = makeSound(file);
	SoundBuilder builder(sound);

	builder.fade(fadeInSecs, fadeOutSecs);
	return sound;
}

Sound makeReverbSound(SoundFile &file, float delay, float decay, int numDelays)
{
	auto sound = makeSound(file);
	SoundBuilder builder(sound);

	builder.reverb(delay, decay, numDelays);
    return sound;
}

// TODO: add more extensions and/or a way to autodetect the file format
SoundFile loadFile(const std::string &filename)
{
    auto fileExtension = [](std::string const & filename) {
        auto p = filename.rfind('.');
        std::string extension = p != std::string::npos ? filename.substr(p+1) : std::string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        return extension;
    };

    auto ext = fileExtension(filename);
    if (ext == "ogg") return loadVorbisFile(filename);
    if (ext == "mp3") return loadMP3File(filename);
    if (ext == "flac") return loadFLACFile(filename);
    // if (ext == "mid") return loadMIDIFile(filename);
    if (ext == "wav") return loadWaveFile(filename);

    throw std::runtime_error("error loading " + filename + ". invalid extension " + ext);
}

SoundFile loadWaveFile(const std::string &filename)
{
    return SoundFile(new WaveFileImpl(filename));
}

SoundFile loadWaveStream(std::istream &inp)
{
    return SoundFile(new WaveFileImpl(inp));
}

SoundFile loadVorbisFile(const std::string &filename)
{
    return SoundFile(new VorbisFileImpl(filename));
}

SoundFile loadVorbisStream(std::istream &inp)
{
    return SoundFile(new VorbisFileImpl(inp));
}

SoundFile loadMP3File(const std::string &filename)
{
    return SoundFile(new MP3FileImpl(filename));
}

SoundFile loadMP3Stream(std::istream &inp)
{
    return SoundFile(new MP3FileImpl(inp));
}

SoundFile loadFLACFile(const std::string &filename)
{
    return SoundFile(new FLACFileImpl(filename));
}

SoundFile loadFLACStream(std::istream &inp)
{
    return SoundFile(new FLACFileImpl(inp));
}

}
}
