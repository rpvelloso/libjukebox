local filename = '../games/mimomania/res/sound/card_deal.wav' -- './jukebox_test/data/g_t_120_50.wav' --three-two-one-go-deep-voice.wav'

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
sound:loop(true):play()
io.write('enter to stop...\n')
io.read()

sound:stop()
io.write('enter to play again...\n')
io.read()

sound:play(); -- keep on looping after playing the sound again
io.write('enter to stop and exit...\n')
io.read()
sound:stop()
