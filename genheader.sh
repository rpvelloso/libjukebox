#!/bin/bash
headers=(
	'jukebox/Decoders/DecoderImpl.h'
	'jukebox/Decoders/Decoder.h'
	'jukebox/FileFormats/SoundFileImpl.h'
	'jukebox/FileFormats/SoundFile.h'
	'jukebox/Sound/SoundTransformation.h'
	'jukebox/Sound/SoundImpl.h'
	'jukebox/Sound/Sound.h'
	'jukebox/Mixer/MixerImpl.h'
	'jukebox/Mixer/Mixer.h'
)

rm -rf tmp.h tmp2.h libjukebox.h
touch tmp.h
i=0
while [ "${headers[i]}" != "" ]
do
   cat ${headers[i]}|egrep -v '(#include|#endif|#ifdef|#ifndef|#elif|#define|#undef)' >> tmp.h
   i=$(( $i + 1 ))
done

g++ -E tmp.h -o tmp2.h

echo '#ifndef _LIBJUKEBOX_H
#define _LIBJUKEBOX_H

#include <string>
#include <memory>
#include <istream>
#include <functional>

' > libjukebox.h

cat tmp2.h|grep -v '# ' >> libjukebox.h

echo '
#endif // #define _LIBJUKEBOX_H' >> libjukebox.h
rm -rf tmp.h tmp2.h
