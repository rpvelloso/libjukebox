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

#ifndef JUKEBOX_FILEFORMATS_FILELOADER_H_
#define JUKEBOX_FILEFORMATS_FILELOADER_H_

#include <iostream>
#include "SoundFileImpl.h"

namespace jukebox {

class FileLoader {
public:
	FileLoader(SoundFileImpl &fileImpl, std::istream& inp) :
		fileImpl(fileImpl), inp(inp) {};

	virtual ~FileLoader() = default;
	virtual void *createHandler() = 0;
protected:
	SoundFileImpl &fileImpl;
	std::istream& inp;
};

class MemoryFileLoader : public FileLoader {
public:
	MemoryFileLoader(SoundFileImpl &fileImpl, std::istream& inp) :
		FileLoader(fileImpl, inp)	{
		auto fileStart = inp.tellg();
		inp.seekg(0, std::ios::end);
		fileSize = inp.tellg() - fileStart;
		inp.seekg(fileStart, std::ios::beg);

		memoryBuffer.reset(new uint8_t[fileSize]);
		inp.read((char *)memoryBuffer.get(), fileSize);
		inp.seekg(fileStart, std::ios::beg);
	};

	virtual ~MemoryFileLoader() = default;

protected:
	std::unique_ptr<uint8_t []> memoryBuffer;
	int fileSize = 0;
};

}

#endif /* JUKEBOX_FILEFORMATS_FILELOADER_H_ */
