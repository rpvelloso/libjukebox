#ifndef LIBJUKEBOX_MIDI_FACTORY_2018_10_06_H_
#define LIBJUKEBOX_MIDI_FACTORY_2018_10_06_H_

#include <istream>
#include <string>
#include "MIDIFileImpl.h"
#include "SoundFile.h"

namespace jukebox {
namespace factory {

    inline SoundFile loadMIDIFile(const std::string &filename) {
        return SoundFile(new MIDIFileImpl(filename));
    }

    inline SoundFile loadMIDIStream(std::istream &inp) {
        return SoundFile(new MIDIFileImpl(inp));
    }
}
}

#endif
