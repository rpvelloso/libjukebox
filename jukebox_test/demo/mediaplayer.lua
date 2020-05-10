local filename = args[1]

io.write('loading sound file...\n\n')
local soundFile = loadSoundFile(filename)

io.write('creating sound object...\n')
sound = makeSound(soundFile)
	:movingAverage(0.0001)

io.write('Filename: ', sound:getFilename(), '\n')
io.write('NumChannels: ', sound:getNumChannels(), '\n')
io.write('SampleRate: ', sound:getSampleRate(), '\n')
io.write('BitsPerSample: ', sound:getBitsPerSample(), '\n')
io.write('DataSize: ', sound:getDataSize(), '\n')
io.write('Duration: ', sound:getDuration(), '\n\n')

local cmd = ''
while cmd ~= 'exit' do
	cmd = io.read()
	if cmd ~= 'exit' and cmd ~= '' then
		cmd = 'sound:'..cmd
		if (cmd:sub(#cmd) ~= ')') then
		   cmd = cmd..'()'
		end
		io.write(cmd, '\n')
		assert(load(cmd))()
	end
end
