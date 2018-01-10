#ifndef _LIBJUKEBOX_H
#define _LIBJUKEBOX_H

#include <string>
#include <memory>
#include <istream>


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
private:
 std::unique_ptr<SoundFileImpl> impl;
 int blockSize;
};


namespace factory {
 extern SoundFile loadWaveFile(const std::string &filename);
 extern SoundFile loadWaveStream(std::istream &inp);
 extern SoundFile loadVorbisFile(const std::string &filename);
 extern SoundFile loadVorbisStream(std::istream &inp);
}

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
