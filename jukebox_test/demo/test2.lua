local filename = './jukebox_test/data/ACDC_-_Back_In_Black-sample.ogg'

io.write('loading sound file...\n\n')
local soundFile = loadSoundFile(filename)

io.write('creating sound object...\n')
local sound = makeSound(soundFile)
sound:loop(false):jointStereo()

io.write('Filename: ', sound:getFilename(), '\n')
io.write('NumChannels: ', sound:getNumChannels(), '\n')
io.write('SampleRate: ', sound:getSampleRate(), '\n')
io.write('BitsPerSample: ', sound:getBitsPerSample(), '\n')
io.write('DataSize: ', sound:getDataSize(), '\n')
io.write('Duration: ', sound:getDuration(), '\n\n')

sound:play()
io.write('enter to stop...\n')
io.read()

sound:stop()
io.write('enter to play again...\n')
io.read()

sound:play(); -- keep on looping after playing the sound again
io.write('enter to stop and exit...\n')
io.read()
sound:stop()
