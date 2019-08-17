# TODO break down this target in multiple sub libraries

add_subdirectory(Decoders/fluidsynth)

set_source_files_properties(
        Decoders/stb_vorbis/stb_vorbis.c
        PROPERTIES
        HEADER_FILE_ONLY true)

add_library(libjukebox
        Decoders/Decoder.cpp
        Decoders/Decoder.h
        Decoders/DecoderImpl.cpp
        Decoders/DecoderImpl.h
        Decoders/FLACDecoderImpl.cpp
        Decoders/FLACDecoderImpl.h
        Decoders/MIDIConfigurator.h
        Decoders/MIDIDecoderImpl.cpp
        Decoders/MIDIDecoderImpl.h
        Decoders/MP3DecoderImpl.cpp
        Decoders/MP3DecoderImpl.h
        Decoders/VorbisDecoderImpl.cpp
        Decoders/VorbisDecoderImpl.h
        Decoders/WaveDecoderImpl.cpp
        Decoders/WaveDecoderImpl.h
        Decoders/Decorators/DecoderImplDecorator.cpp
        Decoders/Decorators/DecoderImplDecorator.h
        Decoders/Decorators/DistortionImpl.cpp
        Decoders/Decorators/DistortionImpl.h
        Decoders/Decorators/FadeImpl.cpp
        Decoders/Decorators/FadeImpl.h
        Decoders/Decorators/FadeOnStopImpl.cpp
        Decoders/Decorators/FadeOnStopImpl.h
        Decoders/Decorators/ReverbImpl.cpp
        Decoders/Decorators/ReverbImpl.h
        Decoders/Decorators/SampleResolutionImpl.cpp
        Decoders/Decorators/SampleResolutionImpl.h
        Decoders/dr_flac/dr_flac.h
        Decoders/dr_mp3/dr_mp3.h
        Decoders/dr_wav/dr_wav.h
        Decoders/stb_vorbis/stb_vorbis.c
        FileFormats/FileLoader.h
        FileFormats/FLACFileImpl.cpp
        FileFormats/FLACFileImpl.h
        FileFormats/MIDIFileImpl.cpp
        FileFormats/MP3FileImpl.h
        FileFormats/MP3FileImpl.cpp
        FileFormats/MP3FileImpl.h
        FileFormats/SoundFile.cpp
        FileFormats/SoundFile.h
        FileFormats/SoundFileImpl.cpp
        FileFormats/SoundFileImpl.h
        FileFormats/VorbisFileImpl.cpp
        FileFormats/VorbisFileImpl.h
        FileFormats/WaveFileImpl.cpp
        FileFormats/WaveFileImpl.h
        FileFormats/midi/MidiFile.h
        Mixer/Factory.h
        Mixer/Mixer.cpp
        Mixer/Mixer.h
        Mixer/MixerImpl.h
        Sound/Factory.cpp
        Sound/Factory.h
        Sound/FileWriterSoundImpl.cpp
        Sound/FileWriterSoundImpl.h
        Sound/Sound.cpp
        Sound/Sound.h
        Sound/SoundImpl.cpp
        Sound/SoundImpl.h
        Sound/Decorators/FadeOnStopSoundImpl.cpp
        Sound/Decorators/FadeOnStopSoundImpl.h)

target_link_libraries(libjukebox fluidsynth)