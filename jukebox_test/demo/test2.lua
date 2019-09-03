local filename = './jukebox_test/data/g_t_120_50.wav'

io.write('loading sound file...\n\n')
local soundFile = loadSoundFile(filename)

io.write('creating sound object...\n')
local sound = makeSound(soundFile)
sound
  :distortion(500)
  --:reverb(0.5, 0.5, 7)
  :movingAverage(0.0005)

io.write('Filename: ', sound:getFilename(), '\n')
io.write('NumChannels: ', sound:getNumChannels(), '\n')
io.write('SampleRate: ', sound:getSampleRate(), '\n')
io.write('BitsPerSample: ', sound:getBitsPerSample(), '\n')
io.write('DataSize: ', sound:getDataSize(), '\n')
io.write('Duration: ', sound:getDuration(), '\n\n')

sound:play()
io.write('enter to stop...\n')
io.read()

sound:pause()
io.write('enter to play again...\n')
io.read()

sound:play() -- keep on looping after playing the sound again
io.write('enter to stop and exit...\n')
io.read()
sound:stop()
