local filename = './jukebox_test/data/congratulations.wav'
io.write('loading sound file...\n\n')
local soundFile = loadSoundFile(filename)
io.write('Filename: ', soundFile:getFilename(), '\n')
io.write('NumChannels: ', soundFile:getNumChannels(), '\n')
io.write('SampleRate: ', soundFile:getSampleRate(), '\n')
io.write('BitsPerSample: ', soundFile:getBitsPerSample(), '\n')
io.write('DataSize: ', soundFile:getDataSize(), '\n')
io.write('Duration: ', soundFile:getDuration(), '\n\n')

io.write('creating sound object...\n')
local sound = makeSound(soundFile)


io.write('configuring sound object...\n')
local soundBuilder = SoundBuilder.new(sound)

soundBuilder
  :reverb(0.01, 0.5, 2)
  :distortion(5)

io.write('setting global volume...\n')
local mixer = Mixer.new()
mixer:setVolume(100)

io.write('playing sound, press enter to exit...\n')
sound:play()
io.read()