#include "Factory.h"
#include <exception>
#include <algorithm>
#include "jukebox/Sound/Decorators/FadedSoundImpl.h"
#include "jukebox/Sound/Decorators/FadeOnStopSoundImpl.h"
#include "jukebox/FileFormats/MP3FileImpl.h"
#include "jukebox/FileFormats/VorbisFileImpl.h"
#include "jukebox/FileFormats/WaveFileImpl.h"

namespace jukebox {
namespace factory {

// makeSound() and makeSoundImpl() are defined by the respective win/linux implementations

Sound makeFadeOnStopSound(SoundFile &file, int fadeOutSecs)
{
    return Sound(new FadeOnStopSoundImpl(makeSoundImpl(file), fadeOutSecs));
}

Sound makeFadedSound(SoundFile &file, int fadeInSecs, int fadeOutSecs)
{
    return Sound(new FadedSoundImpl(makeSoundImpl(file), fadeInSecs, fadeOutSecs));
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

}
}
