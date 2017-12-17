#ifndef _LIBJUKEBOX_H
#define _LIBJUKEBOX_H

#include <string>
#include <memory>
#include <istream>


namespace jukebox {

class SoundFileImpl {
public:
 SoundFileImpl();
 virtual ~SoundFileImpl();
 virtual short getNumChannels() const = 0;
 virtual int getSampleRate() const = 0;
 virtual short getBitsPerSample() const = 0;
 virtual const char *getData() const = 0;
 virtual int getDataSize() const = 0;
 virtual const std::string &getFilename() const = 0;
};

}
namespace jukebox {

class SoundFile {
public:
 SoundFile(SoundFileImpl *impl);
 short getNumChannels() const;
 int getSampleRate() const;
 short getBitsPerSample() const;
 const char *getData() const;
 int getDataSize() const;
 const std::string &getFilename() const;
 double getDuration() const;
private:
 std::unique_ptr<SoundFileImpl> impl;

 template<typename T>
 void normalize();
};

namespace factory {
 extern SoundFile loadWaveFile(const std::string &filename);
 extern SoundFile loadWaveStream(std::istream &inp);
}

}
namespace jukebox {

class WaveFile : public SoundFileImpl {
 struct WaveHeader {
  char ChunkID[4];
  int ChunkSize;
  char Format[4];
  char Subchunk1ID[4];
  int Subchunk1Size;
  short AudioFormat;
  short NumChannels;
  int SampleRate;
  int ByteRate;
  short BlockAlign;
  short BitsPerSample;
  char Subchunk2ID[4];
  int Subchunk2Size;
 };
public:
 WaveFile(const std::string &filename);
 WaveFile(std::istream &inp);
 short getNumChannels() const override;
 int getSampleRate() const override;
 short getBitsPerSample() const override;
 const char *getData() const override;
 int getDataSize() const override;
 const std::string &getFilename() const;
private:
 std::string filename;
 WaveHeader header;
 std::unique_ptr<char []> data;

 void load(std::istream &inp);
};

}
namespace jukebox {

class SoundImpl {
public:
 SoundImpl(SoundFile &file);
 virtual ~SoundImpl();
 virtual void play() = 0;
 virtual void stop() = 0;
 virtual int getVolume() = 0;
 virtual void setVolume(int) = 0;
 SoundFile &getSoundFile();
protected:
 SoundFile &soundFile;
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
private:
 std::unique_ptr<SoundImpl> impl;
};

namespace factory {
 extern Sound makeSound(SoundFile &file);
}

}
namespace jukebox {

class MixerImpl {
public:
 MixerImpl();
 virtual ~MixerImpl();
 virtual int getVolume() = 0;
 virtual void setVolume(int vol) = 0;
};

}
namespace jukebox {

class Mixer {
public:
 Mixer(MixerImpl *impl);
 int getVolume();
 void setVolume(int vol);
private:
 std::unique_ptr<MixerImpl> impl;
};

extern Mixer mixer;

}

#endif // #define _LIBJUKEBOX_H
