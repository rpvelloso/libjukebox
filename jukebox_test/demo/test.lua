io.write('loading sound file...\n')
local soundFile = loadSoundFile('./jukebox_test/data/congratulations.wav')

io.write('creating sound object...\n')
local sound = makeSound(soundFile)


io.write('configuring sound object...\n')
local soundBuilder = SoundBuilder.new(sound)

soundBuilder
  :reverb(0.01, 0.8, 3)
  :distortion(5)

io.write('playing sound, press enter to exit...\n')
sound:play()
io.read()