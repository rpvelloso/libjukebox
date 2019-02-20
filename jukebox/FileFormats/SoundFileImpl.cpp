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

#include <istream>
#include "SoundFileImpl.h"

namespace jukebox {

void SoundFileImpl::truncAt(int pos) {
	dataSize = std::min(pos, dataSize);
};

int SoundFileImpl::getDataSize() const {
	return dataSize;
};

int jukebox::SoundFileImpl::silenceLevel() const {
	return getBitsPerSample() == 8?128:0;
}

size_t dr_libs_read_callback(void *stream, void *outBuf, size_t len) {
	auto inp = (std::istream *)stream;
	inp->clear();
	return inp->read((char *)outBuf, len).gcount();
}

uint32_t dr_libs_seek_callback(void *stream, int offset, int origin) { // seek origin: 0=start; 1=current
	auto inp = (std::istream *)stream;
	inp->clear();
	inp->seekg(offset, origin==0?std::ios::beg:std::ios::cur);
	return true;
}

}
