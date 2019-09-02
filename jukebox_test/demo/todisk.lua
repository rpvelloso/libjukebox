local input = '../games/mimomania/res/sound/mimi_meow.mp3'
local output = '../games/mimomania/res/sound/mimi_meow.wav'

io.write('loading sound file...\n\n')
local soundFile = loadSoundFile(input)
io.write('Filename: ', soundFile:getFilename(), '\n')
io.write('NumChannels: ', soundFile:getNumChannels(), '\n')
io.write('SampleRate: ', soundFile:getSampleRate(), '\n')
io.write('BitsPerSample: ', soundFile:getBitsPerSample(), '\n')
io.write('DataSize: ', soundFile:getDataSize(), '\n')
io.write('Duration: ', soundFile:getDuration(), '\n\n')

io.write('creating sound object...\n')
local sound = makeSoundOutputToFile(soundFile, output)
sound:play()
io.write('done...\n')
