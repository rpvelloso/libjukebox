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

#ifndef JUKEBOX_FILEFORMATS_BUFFEREDSOUNDFILEIMPL_H_
#define JUKEBOX_FILEFORMATS_BUFFEREDSOUNDFILEIMPL_H_

#include <memory>
#include <vector>
#include "../SoundFileImpl.h"

namespace jukebox {

/*
 * this is a decorator class that loads the
 * entire file from disk to RAM memory and normalizes it
 *
 * */

class BufferedSoundFileImpl: public SoundFileImpl {
public:
	BufferedSoundFileImpl(SoundFileImpl *impl);
	virtual ~BufferedSoundFileImpl() = default;
	short getNumChannels() const;
	int getSampleRate() const;
	short getBitsPerSample() const;
	const char *getData() const;
	int getDataSize() const;
	const std::string &getFilename() const;
	int read(char *buf, int pos, int len);
private:
	std::unique_ptr<SoundFileImpl> impl;
	std::unique_ptr<char []> data;
	size_t dataSize;

	template<typename T>
	std::vector<float> toFloat();
	void toFixed(std::vector<float> &inp);
};

} /* namespace jukebox */

#endif /* JUKEBOX_FILEFORMATS_BUFFEREDSOUNDFILEIMPL_H_ */
