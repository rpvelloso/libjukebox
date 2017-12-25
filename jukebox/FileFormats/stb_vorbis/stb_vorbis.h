#ifndef LIBJUKEBOX_STB_VORBIS_2017_12_23_H_
#define LIBJUKEBOX_STB_VORBIS_2017_12_23_H_

extern "C" {
extern int stb_vorbis_decode_memory(
	const unsigned char *mem,
	int len,
	int *channels,
	int *sample_rate,
	short **output);
}

#endif
