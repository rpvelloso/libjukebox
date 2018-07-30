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
 void truncAt(int pos);
private:
 std::unique_ptr<SoundFileImpl> impl;
 int blockSize, dataSize;
};


namespace factory {
 extern SoundFile loadMP3File(const std::string &filename);
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
}

}
namespace jukebox {

class SoundTransformation {
public:
 SoundTransformation(SoundFile &soundFile) : soundFile(soundFile) {};
 virtual ~SoundTransformation() = default;
<<<<<<< HEAD
 virtual void operator()(void *, int, int) = 0;
=======
 virtual void operator()(uint8_t *, int, int) = 0;
 virtual void operator()(int16_t *, int, int) = 0;
>>>>>>> branch 'master' of https://github.com/rpvelloso/libjukebox.git
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
<<<<<<< HEAD
};

}
namespace jukebox {

class FadeOnStopSoundImpl: public SoundImpl {
public:
 FadeOnStopSoundImpl(SoundImpl *, int);
 virtual ~FadeOnStopSoundImpl() = default;
 void play() override;
 void stop() override;
 int getVolume() override;
 void setVolume(int) override;
 void loop(bool) override;
private:
 std::unique_ptr<SoundImpl> impl;
 int fadeOutSecs;
=======
>>>>>>> branch 'master' of https://github.com/rpvelloso/libjukebox.git
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
