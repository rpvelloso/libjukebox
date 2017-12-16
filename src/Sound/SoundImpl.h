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

#ifndef SOUNDIMPL_H_
#define SOUNDIMPL_H_

#include "FileFormats/SoundFile.h"

namespace jukebox {

class SoundImpl {
public:
	SoundImpl(SoundFile &file);
	virtual ~SoundImpl();
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual int getVolume() = 0;
	virtual void setVolume(int) = 0;
	SoundFile &getSoundFile();
protected:
	SoundFile &soundFile;
};

} /* namespace jukebox */

#endif /* SOUNDIMPL_H_ */
