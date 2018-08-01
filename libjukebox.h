#ifndef _LIBJUKEBOX_H
#define _LIBJUKEBOX_H

#include <string>
#include <memory>
#include <istream>


namespace jukebox {

class DecoderImpl {
public:
 DecoderImpl() = default;
 virtual ~DecoderImpl() = default;
 virtual int getSamples(char *buf, int pos, int len) = 0;
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
 virtual ~SoundFileImpl() = default;
 virtual short getNumChannels() const = 0;
 virtual int getSampleRate() const = 0;
 virtual short getBitsPerSample() const = 0;
 virtual int getDataSize() const = 0;
 virtual const std::string &getFilename() const = 0;




 virtual int read(char *buf, int pos, int len) = 0;
 virtual std::unique_ptr<Decoder> makeDecoder() = 0;
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
 int read(char* buf, int pos, int len);
 void truncAt(int pos);
 std::unique_ptr<Decoder> makeDecoder();

private:
 std::unique_ptr<SoundFileImpl> impl;
 int blockSize, dataSize;
};


namespace factory {
 extern SoundFile loadWaveFile(const std::string &filename);
 extern SoundFile loadWaveStream(std::istream &inp);
 extern SoundFile loadVorbisFile(const std::string &filename);
 extern SoundFile loadVorbisStream(std::istream &inp);
 extern SoundFile loadBufferedWaveFile(const std::string &filename);
 extern SoundFile loadBufferedWaveStream(std::istream &inp);
 extern SoundFile loadBufferedVorbisFile(const std::string &filename);
 extern SoundFile loadBufferedVorbisStream(std::istream &inp);
 extern SoundFile loadFadedWaveFile(const std::string &filename, int fadeInSecs, int fadeOutSecs);
 extern SoundFile loadFadedWaveStream(std::istream &inp, int fadeInSecs, int fadeOutSecs);
 extern SoundFile loadFadedVorbisFile(const std::string &filename, int fadeInSecs, int fadeOutSecs);
 extern SoundFile loadFadedVorbisStream(std::istream &inp, int fadeInSecs, int fadeOutSecs);
 extern SoundFile loadMP3File(const std::string &filename);
 extern SoundFile loadMP3Stream(std::istream &inp);
 extern SoundFile loadBufferedMP3File(const std::string &filename);
 extern SoundFile loadBufferedMP3Stream(std::istream &inp);
 extern SoundFile loadFadedMP3File(const std::string &filename, int fadeInSecs, int fadeOutSecs);
 extern SoundFile loadFadedMP3Stream(std::istream &inp, int fadeInSecs, int fadeOutSecs);
}

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

class SoundImpl {
public:
 SoundImpl(SoundFile &file);
 virtual ~SoundImpl() = default;
 virtual void play() = 0;
 virtual void stop() = 0;
 virtual int getVolume() = 0;
 virtual void setVolume(int) = 0;
 virtual void loop(bool) = 0;
 SoundFile &getSoundFile();
 int getPosition() const;
 void setTransformation(SoundTransformation *);
protected:
 int position = 0;
 SoundFile &soundFile;
 std::unique_ptr<SoundTransformation> transformation;
};

}
namespace jukebox {

class Sound {
public:
 Sound(SoundImpl *impl);
 void play();
 void stop();
 int getVolume();
 void setVolume(int);
 void loop(bool);
 int getPosition() const;
private:
 std::unique_ptr<SoundImpl> impl;
};

namespace factory {
 extern Sound makeSound(SoundFile &file);
 extern Sound makeFadeOnStopSound(SoundFile &file, int);
}

}
namespace jukebox {

class MixerImpl {
public:
 virtual ~MixerImpl() = default;
 virtual int getVolume() = 0;
 virtual void setVolume(int vol) = 0;
};

namespace factory {
 extern MixerImpl &makeMixerImpl();
}

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

#endif // #define _LIBJUKEBOX_H
