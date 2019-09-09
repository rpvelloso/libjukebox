local filename = './jukebox_test/data/g_t_120_50.wav'

io.write('loading sound file...\n\n')
local soundFile = loadSoundFile(filename)

io.write('creating sound object...\n')
local sound = makeSound(soundFile)
  :loop(true)
--  :jointStereo()
--  :resolution(32)
--  :distortion(50)
--  :reverb(0.1, 0.5, 7)
--  :movingAverage(0.0001)

io.write('Filename: ', sound:getFilename(), '\n')
io.write('NumChannels: ', sound:getNumChannels(), '\n')
io.write('SampleRate: ', sound:getSampleRate(), '\n')
io.write('BitsPerSample: ', sound:getBitsPerSample(), '\n')
io.write('DataSize: ', sound:getDataSize(), '\n')
io.write('Duration: ', sound:getDuration(), '\n\n')

sound:play()
io.write('enter to pause...\n')
io.read()

sound:pause()
io.write('enter to play again...\n')
io.read()

sound:play() -- keep on looping after playing the sound again
io.write('enter to stop and exit...\n')
io.read()
sound:stop()
