#!/bin/bash
headers=(
	'src/FileFormats/SoundFileImpl.h'
	'src/FileFormats/SoundFile.h'
	'src/Sound/SoundImpl.h'
	'src/Sound/Sound.h'
	'src/Mixer/MixerImpl.h'
	'src/Mixer/Mixer.h'
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

' > libjukebox.h

cat tmp2.h|grep -v '# ' >> libjukebox.h

echo '
#endif // #define _LIBJUKEBOX_H' >> libjukebox.h
rm -rf tmp.h tmp2.h
