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

#include "SoundImpl.h"

namespace jukebox {

SoundImpl::SoundImpl(Decoder *decoder) :
		decoder(decoder),
		onStop([](void){}) {
}

int SoundImpl::getPosition() const {
	return position;
}

void SoundImpl::setPosition(int pos) {
	position = pos;
}

void SoundImpl::setOnStopCallback(std::function<void(void)> os) {
	onStop = os;
}

void SoundImpl::addTimedEventCallback(size_t seconds, std::function<void(void)> te) {
	timedEvents[seconds] = te;
}

Decoder& SoundImpl::getDecoder() {
	return *decoder;
}

} /* namespace jukebox */
