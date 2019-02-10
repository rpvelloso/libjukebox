/*
    Copyright 2019 Roberto Panerai Velloso.
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

#include "SoundBuilder.h"
#include "jukebox/Decoders/Decoder.h"
#include "Factory.h"
#include "../Decoders/Decorators/FadeImpl.h"
#include "../Decoders/Decorators/ReverbImpl.h"
#include "../Decoders/Decorators/SampleResolutionImpl.h"
#include "../Decoders/Decorators/DistortionImpl.h"
#include "jukebox/Sound/Decorators/FadeOnStopSoundImpl.h"

namespace jukebox {
namespace factory {
SoundBuilder::SoundBuilder(Sound &sound) :
		sound(sound) {
}

SoundBuilder& SoundBuilder::reverb(float delay, float decay, size_t numDelays) {
	sound.impl->getDecoder().wrapDecoder<ReverbImpl>(delay, decay, numDelays);
	return *this;
}

SoundBuilder& SoundBuilder::distortion(float gain) {
	sound.impl->getDecoder().wrapDecoder<DistortionImpl>(gain);
	return *this;
}

SoundBuilder& SoundBuilder::fade(int fadeInSecs, int fadeOutSecs) {
	sound.impl->getDecoder().wrapDecoder<FadeImpl>(fadeInSecs, fadeOutSecs);
	return *this;
}

SoundBuilder& SoundBuilder::resolution(int bitsPerSample) {
	sound.impl->getDecoder().wrapDecoder<SampleResolutionImpl>(bitsPerSample);
	return *this;
}

SoundBuilder& SoundBuilder::fadeOnStop(int fadeOutSecs) {
	sound.impl.reset(new FadeOnStopSoundImpl(sound.impl.release(), fadeOutSecs));
	return *this;
}

SoundBuilder& SoundBuilder::loop(bool l) {
	sound.loop(l);
	return *this;
}

SoundBuilder& SoundBuilder::setVolume(int vol) {
	sound.setVolume(vol);
	return *this;
}
} /* namespace factory */
} /* namespace jukebox */
