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
#include <map>
#include <mutex>
#include <vector>


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
 virtual int getBlockSize() const;
 virtual short getNumChannels() const;
 virtual int getSampleRate() const;
 virtual short getBitsPerSample() const;
 virtual int getDataSize() const;
 virtual int silenceLevel() const;
 SoundFileImpl &getFileImpl() const;
 virtual DecoderImpl *peel();
protected:
 SoundFileImpl &fileImpl;
 int blockSize;
};

}
namespace jukebox {

class SoundFileImpl;
class SoundFile;

class Decoder {
public:
 Decoder(SoundFile soundFile);
 Decoder(Decoder &&) = default;
 Decoder &operator=(Decoder &&) = default;
 int getSamples(char *buf, int pos, int len);
 short getNumChannels() const;
 int getSampleRate() const;
 short getBitsPerSample() const;
 int getDataSize() const;
 int getBlockSize() const;
 const std::string &getFilename() const;
 double getDuration() const;
 int silenceLevel() const;
 Decoder *prototype();

 template<typename T, typename ...Params>
 Decoder &wrap(Params&&... params) {
  impl.reset(new T(impl.release(), std::forward<Params>(params)...));
        return *this;
 }

 Decoder &peel();
private:
 Decoder(std::shared_ptr<SoundFileImpl> soundFileImpl);
 Decoder(const Decoder &) = delete;
 Decoder &operator=(const Decoder &) = delete;

 std::shared_ptr<SoundFileImpl> soundFileImpl;
 std::unique_ptr<DecoderImpl> impl;
};

}
namespace jukebox {

class MIDIConfigurator {
public:
 MIDIConfigurator(MIDIConfigurator &) = delete;
 void operator =(MIDIConfigurator &) = delete;

 const std::string &getSoundFont() const;
 void setSoundFont(const std::string &sfPath);
 static MIDIConfigurator &getInstance();
private:
 std::string soundFontPath;
 static std::unique_ptr<MIDIConfigurator> instance;
 MIDIConfigurator();
};

}
namespace jukebox {

extern size_t dr_libs_read_callback(void *stream, void *outBuf, size_t len);
extern uint32_t dr_libs_seek_callback(void *stream, int offset, int origin);

class SoundFileImpl {
public:
 SoundFileImpl() = default;
 virtual ~SoundFileImpl() = default;
 virtual short getNumChannels() const = 0;
 virtual int getSampleRate() const = 0;
 virtual short getBitsPerSample() const = 0;
 virtual const std::string &getFilename() const = 0;
 virtual DecoderImpl *makeDecoder() = 0;
 virtual int silenceLevel() const;
 virtual void truncAt(int pos);
 virtual int getDataSize() const;
protected:
 int dataSize = 0;
};

}
namespace jukebox {

class Decoder;

class SoundFile {
friend class Decoder;
public:
 SoundFile(SoundFileImpl *impl);
 short getNumChannels() const;
 int getSampleRate() const;
 short getBitsPerSample() const;
 int getDataSize() const;
 const std::string &getFilename() const;
 double getDuration() const;
 void truncAt(int pos);
 DecoderImpl *makeDecoder();
 int silenceLevel() const;
private:
 std::shared_ptr<SoundFileImpl> impl;
};

}
namespace jukebox {

class SoundImpl {
public:
 SoundImpl(Decoder *);
 virtual ~SoundImpl() = default;
 virtual void play() = 0;
 virtual void pause() = 0;
 virtual int getVolume() const = 0;
 virtual void setVolume(int) = 0;
 virtual void loop(bool) = 0;
 virtual bool playing() const = 0;
 virtual int getPosition() const;
 virtual void setPosition(int pos);
 virtual void pushOnStopCallback(std::function<void(void)>);
 virtual std::function<void(void)> popOnStopCallback();
 virtual void clearOnStopStack();
 virtual void addTimedEventCallback(size_t seconds, std::function<void(void)>);
 virtual Decoder &getDecoder();
protected:
 int position = 0;
 std::unique_ptr<Decoder> decoder;
 std::vector<std::function<void (void)>> onStopStack;
 std::recursive_mutex timedEventsMutex;
 std::map<size_t, std::function<void (void)> > timedEvents;

 void processTimedEvents();
};

}
namespace jukebox {

namespace factory {
 class SoundBuilder;
}

class Sound {
public:
 Sound(SoundImpl *impl);
 Sound(Sound &&) = default;
 Sound &operator=(Sound &&) = default;

 Sound &play();
 Sound &restart();
 Sound &pause();
 Sound &stop();
 int getVolume() const;
 int getPosition() const;
 bool playing() const;
 Sound &setPosition(int pos);




 Sound &pushOnStopCallback(std::function<void(void)>);
 std::function<void(void)> popOnStopCallback();
 Sound &clearOnStopStack();




 Sound &addTimedEventCallback(size_t seconds, std::function<void(void)>);

 Sound &reverb(float delay, float decay, size_t numDelays);
 Sound &distortion(float gain);
 Sound &fade(int fadeInSecs, int fadeOutSecs);
 Sound &resolution(int bitsPerSample);
 Sound &fadeOnStop(int fadeOutSecs);
 Sound &setVolume(int);
 Sound &loop(bool);
 Sound &jointStereo();
 Sound &peelDecoder();

 Sound prototype();

 short getNumChannels() const;
 int getSampleRate() const;
 short getBitsPerSample() const;
 int getDataSize() const;
 const std::string &getFilename() const;
 double getDuration() const;

private:
 Sound(const Sound &) = delete;
 Sound &operator=(const Sound &) = delete;

 std::unique_ptr<SoundImpl> impl;
 bool looping = false;
};

}
namespace jukebox {
namespace factory {

Sound makeSound(SoundFile &file);
Sound makeSound(const std::string &filename, bool onMemory = false);
Sound makeSoundOutputToFile(SoundFile &file, const std::string &filename);
Sound makeSoundOutputToFile(const std::string &inputFile, const std::string &filename, bool onMemory = false);
SoundImpl *makeSoundImpl(Decoder *decoder);

SoundFile loadFile(const std::string &filename, bool onMemory = false);

SoundFile loadWaveFile(const std::string &filename, bool onMemory = false);
SoundFile loadWaveStream(std::istream &inp, bool onMemory = false);
SoundFile loadVorbisFile(const std::string &filename, bool onMemory = false);
SoundFile loadVorbisStream(std::istream &inp, bool onMemory = false);
SoundFile loadMP3File(const std::string &filename, bool onMemory = false);
SoundFile loadMP3Stream(std::istream &inp, bool onMemory = false);
SoundFile loadFLACFile(const std::string &filename, bool onMemory = false);
SoundFile loadFLACStream(std::istream &inp, bool onMemory = false);
SoundFile loadMIDIFile(const std::string &filename);
SoundFile loadMIDIStream(std::istream &inp);

}
}
namespace jukebox {

namespace factory {
 extern MixerImpl &makeMixerImpl();
}

}

#endif // #define _LIBJUKEBOX_H
