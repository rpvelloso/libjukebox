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

#ifndef _LIBJUKEBOX_H
#define _LIBJUKEBOX_H

#include <string>
#include <memory>
#include <istream>
#include <functional>


namespace jukebox {

class MixerImpl {
public:
 virtual ~MixerImpl() = default;
 virtual int getVolume() = 0;
 virtual void setVolume(int vol) = 0;
};

}
namespace jukebox {

class Mixer {
public:
 Mixer();
 int getVolume();
 void setVolume(int vol);
private:
 MixerImpl &impl;
};

}
namespace jukebox {

class SoundFileImpl;

class DecoderImpl {
public:
 DecoderImpl(SoundFileImpl &fileImpl);
 virtual ~DecoderImpl() = default;
 virtual int getSamples(char *buf, int pos, int len) = 0;
 int getBlockSize() const;

private:
 int blockSize;
};

}
namespace jukebox {

class Decoder {
public:
 Decoder(DecoderImpl * impl);
 int getSamples(char *buf, int pos, int len);
private:
 std::unique_ptr<DecoderImpl> impl;
};

}
namespace jukebox {

class SoundFileImpl {
public:
 SoundFileImpl() = default;
 virtual ~SoundFileImpl() = default;
 virtual short getNumChannels() const = 0;
 virtual int getSampleRate() const = 0;
 virtual short getBitsPerSample() const = 0;
 virtual const std::string &getFilename() const = 0;
 virtual std::unique_ptr<Decoder> makeDecoder() = 0;
 virtual void truncAt(int pos);
 virtual int getDataSize() const;
protected:
 int dataSize = 0;
};

}
namespace jukebox {

class SoundFile {
public:
 SoundFile(SoundFileImpl *impl);
 short getNumChannels() const;
 int getSampleRate() const;
 short getBitsPerSample() const;
 int getDataSize() const;
 const std::string &getFilename() const;
 double getDuration() const;
 void truncAt(int pos);
 std::unique_ptr<Decoder> makeDecoder();
private:
 std::unique_ptr<SoundFileImpl> impl;
 int blockSize;
};

}
namespace jukebox {

class MIDIFileImpl: public SoundFileImpl {
public:
 MIDIFileImpl(const std::string &filename);
 MIDIFileImpl(std::istream& inp);
 virtual ~MIDIFileImpl() = default;
 short getNumChannels() const override;
 int getSampleRate() const override;
 short getBitsPerSample() const override;
 const std::string &getFilename() const override;
 std::unique_ptr<Decoder> makeDecoder() override;
 uint8_t *getFileBuffer();
 int getFileSize();
private:
 std::string filename;
 int fileSize = 0;
 std::unique_ptr<uint8_t> fileBuffer;

 void load(std::istream& inp);
};

}
namespace jukebox {

class SoundImpl {
public:
 SoundImpl(SoundFile &file);
 virtual ~SoundImpl() = default;
 virtual void play() = 0;
 virtual void stop() = 0;
 virtual int getVolume() const = 0;
 virtual void setVolume(int) = 0;
 virtual void loop(bool) = 0;
 SoundFile &getSoundFile();
 int getPosition() const;
 void setTransformationCallback(std::function<void(void *, int, int)>);
 virtual void setOnStopCallback(std::function<void(void)>);
protected:
 int position = 0;
 SoundFile &soundFile;
 std::function<void (void *, int, int)> transformation;
 std::function<void (void)> onStop;
};

}
namespace jukebox {

class Sound {
public:
 Sound(SoundImpl *impl);
 void play();
 void stop();
 int getVolume() const;
 void setVolume(int);
 void loop(bool);
 int getPosition() const;




 void setOnStopCallback(std::function<void(void)>);
 void clearOnStopCallback();
private:
 std::unique_ptr<SoundImpl> impl;
};

}
namespace jukebox {

class SoundTransformation {
public:
 SoundTransformation(SoundFile &soundFile) : soundFile(soundFile) {};
 virtual ~SoundTransformation() = default;
 virtual void operator()(void *, int, int) = 0;
protected:
 SoundFile &soundFile;
};

}
namespace jukebox {
namespace factory {

Sound makeSound(SoundFile &file);
SoundImpl *makeSoundImpl(SoundFile& file);

Sound makeFadeOnStopSound(SoundFile &file, int fadeOutSecs);
Sound makeFadedSound(SoundFile &file, int fadeInSecs, int fadeOutSecs);

SoundFile loadFile(const std::string &filename);

SoundFile loadWaveFile(const std::string &filename);
SoundFile loadWaveStream(std::istream &inp);
SoundFile loadVorbisFile(const std::string &filename);
SoundFile loadVorbisStream(std::istream &inp);
SoundFile loadMP3File(const std::string &filename);
SoundFile loadMP3Stream(std::istream &inp);

}
}



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
namespace jukebox {

namespace factory {
 extern MixerImpl &makeMixerImpl();
}

}

#endif // #define _LIBJUKEBOX_H
