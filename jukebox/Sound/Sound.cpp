/*
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

#include <algorithm>
#include "Sound.h"

namespace {

int normalize(int vol) { return std::max(0,std::min(vol,100)); }

}

namespace jukebox {

Sound::Sound(SoundImpl *impl) : impl(impl) {
}

void Sound::play() {
	//impl->stop();
	impl->play();
}

void Sound::stop() {
	impl->stop();
}

int Sound::getVolume() const {
	auto vol = impl->getVolume();
	return normalize(vol);
}

void Sound::setVolume(int vol) {
	impl->setVolume(normalize(vol));
}

void Sound::loop(bool l) {
	impl->loop(l);
}

int Sound::getPosition() const {
	return impl->getPosition();
}

void Sound::setOnStopCallback(std::function<void(void)> os) {
	impl->setOnStopCallback(os);
}

void Sound::clearOnStopCallback() {
	impl->setOnStopCallback([](){});
}

} /* namespace jukebox */
