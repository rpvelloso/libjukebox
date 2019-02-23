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

#ifndef JUKEBOX_DECODERS_MIDICONFIGURATOR_H_
#define JUKEBOX_DECODERS_MIDICONFIGURATOR_H_

#include <memory>

namespace jukebox {

class MIDIConfigurator {
public:
	MIDIConfigurator(MIDIConfigurator &) = delete;
	void operator =(MIDIConfigurator &) = delete;

	const std::string &getSoundFont() const;
	void setSoundFont(const std::string &sfPath);
	static MIDIConfigurator &getInstance();
private:
	std::string soundFontPath;
	static std::unique_ptr<MIDIConfigurator> instance;
	MIDIConfigurator();
};

}

#endif /* JUKEBOX_DECODERS_MIDICONFIGURATOR_H_ */
