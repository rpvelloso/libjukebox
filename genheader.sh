#!/bin/bash
headers=(
	'jukebox/Mixer/MixerImpl.h'
	'jukebox/Mixer/Mixer.h'
	'jukebox/Decoders/DecoderImpl.h'
	'jukebox/Decoders/Decoder.h'
	'jukebox/FileFormats/SoundFileImpl.h'
	'jukebox/FileFormats/SoundFile.h'
	'jukebox/FileFormats/MIDIFileImpl.h'
	'jukebox/Sound/SoundImpl.h'
	'jukebox/Sound/Sound.h'
	'jukebox/Sound/SoundTransformation.h'
	'jukebox/Sound/Factory.h'
	'jukebox/FileFormats/MIDIFactory.h'
	'jukebox/Mixer/Factory.h'
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

echo '/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libjukebox.
    libjukebox is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libjukebox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libjukebox.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LIBJUKEBOX_H
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
