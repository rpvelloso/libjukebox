local filename = './jukebox_test/data/g_t_120_50.wav' --three-two-one-go-deep-voice.wav'

-- midiConfig:setSoundFont('./jukebox_test/data/GXSCC_gm_033.sf2')

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

sound
  :loop(true)
  :distortion(500)

io.write('setting global volume...\n')
local mixer = Mixer.new()
mixer:setVolume(50)

io.write('playing sound, press enter for echo/delay effect...\n')
sound:play()
io.read()

sound -- applying effects during playback works too
  :reverb(0.1, 0.5, 2)

io.write('press enter to restore original sound...\n')
io.read()
sound:peelDecoder()
sound:peelDecoder()
sound:peelDecoder()
sound:peelDecoder()
sound:peelDecoder()
sound:peelDecoder()


io.write('press enter to exit...\n')
io.read()
