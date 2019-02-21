// MP3 audio decoder. Public domain. See "unlicense" statement at the end of this file.
// dr_mp3 - v0.4.2 - 2018-xx-xx
//
// David Reid - mackron@gmail.com
//
// Based off minimp3 (https://github.com/lieff/minimp3) which is where the real work was done. See the bottom of this file for
// differences between minimp3 and dr_mp3.

// USAGE
// =====
// dr_mp3 is a single-file library. To use it, do something like the following in one .c file.
//     #define DR_MP3_IMPLEMENTATION
//     #include "dr_mp3.h"
//
// You can then #include this file in other parts of the program as you would with any other header file. To decode audio data,
// do something like the following:
//
//     drmp3 mp3;
//     if (!drmp3_init_file(&mp3, "MySong.mp3", NULL)) {
//         // Failed to open file
//     }
//
//     ...
//
//     drmp3_uint64 framesRead = drmp3_read_pcm_frames_f32(pMP3, framesToRead, pFrames);
//
// The drmp3 object is transparent so you can get access to the channel count and sample rate like so:
//
//     drmp3_uint32 channels = mp3.channels;
//     drmp3_uint32 sampleRate = mp3.sampleRate;
//
// The third parameter of drmp3_init_file() in the example above allows you to control the output channel count and sample rate. It
// is a pointer to a drmp3_config object. Setting any of the variables of this object to 0 will cause dr_mp3 to use defaults.
//
// The example above initializes a decoder from a file, but you can also initialize it from a block of memory and read and seek
// callbacks with drmp3_init_memory() and drmp3_init() respectively.
//
// You do not need to do any annoying memory management when reading PCM frames - this is all managed internally. You can request
// any number of PCM frames in each call to drmp3_read_pcm_frames_f32() and it will return as many PCM frames as it can, up to the
// requested amount.
//
// You can also decode an entire file in one go with drmp3_open_and_read_f32(), drmp3_open_memory_and_read_f32() and
// drmp3_open_file_and_read_f32().
//
//
// OPTIONS
// =======
// #define these options before including this file.
//
// #define DR_MP3_NO_STDIO
//   Disable drmp3_init_file(), etc.
//
// #define DR_MP3_NO_SIMD
//   Disable SIMD optimizations.

#ifndef dr_mp3_h
#define dr_mp3_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#if defined(_MSC_VER) && _MSC_VER < 1600
typedef   signed char    drmp3_int8;
typedef unsigned char    drmp3_uint8;
typedef   signed short   drmp3_int16;
typedef unsigned short   drmp3_uint16;
typedef   signed int     drmp3_int32;
typedef unsigned int     drmp3_uint32;
typedef   signed __int64 drmp3_int64;
typedef unsigned __int64 drmp3_uint64;
#else
#include <stdint.h>
typedef int8_t           drmp3_int8;
typedef uint8_t          drmp3_uint8;
typedef int16_t          drmp3_int16;
typedef uint16_t         drmp3_uint16;
typedef int32_t          drmp3_int32;
typedef uint32_t         drmp3_uint32;
typedef int64_t          drmp3_int64;
typedef uint64_t         drmp3_uint64;
#endif
typedef drmp3_uint8      drmp3_bool8;
typedef drmp3_uint32     drmp3_bool32;
#define DRMP3_TRUE       1
#define DRMP3_FALSE      0

#define DRMP3_MAX_PCM_FRAMES_PER_MP3_FRAME  1152
#define DRMP3_MAX_SAMPLES_PER_FRAME         (DRMP3_MAX_PCM_FRAMES_PER_MP3_FRAME*2)


// Low Level Push API
// ==================
typedef struct
{
    int frame_bytes, channels, hz, layer, bitrate_kbps;
} drmp3dec_frame_info;

typedef struct
{
    float mdct_overlap[2][9*32], qmf_state[15*2*32];
    int reserv, free_format_bytes;
    unsigned char header[4], reserv_buf[511];
} drmp3dec;

// Initializes a low level decoder.
void drmp3dec_init(drmp3dec *dec);

// Reads a frame from a low level decoder.
int drmp3dec_decode_frame(drmp3dec *dec, const unsigned char *mp3, int mp3_bytes, void *pcm, drmp3dec_frame_info *info);

// Helper for converting between f32 and s16.
void drmp3dec_f32_to_s16(const float *in, drmp3_int16 *out, int num_samples);




// Main API (Pull API)
// ===================

typedef struct drmp3_src drmp3_src;
typedef drmp3_uint64 (* drmp3_src_read_proc)(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut, void* pUserData); // Returns the number of frames that were read.

typedef enum
{
    drmp3_src_algorithm_none,
    drmp3_src_algorithm_linear
} drmp3_src_algorithm;

#define DRMP3_SRC_CACHE_SIZE_IN_FRAMES    512
typedef struct
{
    drmp3_src* pSRC;
    float pCachedFrames[2 * DRMP3_SRC_CACHE_SIZE_IN_FRAMES];
    drmp3_uint32 cachedFrameCount;
    drmp3_uint32 iNextFrame;
} drmp3_src_cache;

typedef struct
{
    drmp3_uint32 sampleRateIn;
    drmp3_uint32 sampleRateOut;
    drmp3_uint32 channels;
    drmp3_src_algorithm algorithm;
    drmp3_uint32 cacheSizeInFrames;  // The number of frames to read from the client at a time.
} drmp3_src_config;

struct drmp3_src
{
    drmp3_src_config config;
    drmp3_src_read_proc onRead;
    void* pUserData;
    float bin[256];
    drmp3_src_cache cache;    // <-- For simplifying and optimizing client -> memory reading.
    union
    {
        struct
        {
            double alpha;
            drmp3_bool32 isPrevFramesLoaded : 1;
            drmp3_bool32 isNextFramesLoaded : 1;
        } linear;
    } algo;
};

typedef enum
{
    drmp3_seek_origin_start,
    drmp3_seek_origin_current
} drmp3_seek_origin;

typedef struct
{
    drmp3_uint64 seekPosInBytes;        // Points to the first byte of an MP3 frame.
    drmp3_uint64 pcmFrameIndex;         // The index of the PCM frame this seek point targets.
    drmp3_uint16 mp3FramesToDiscard;    // The number of whole MP3 frames to be discarded before pcmFramesToDiscard.
    drmp3_uint16 pcmFramesToDiscard;    // The number of leading samples to read and discard. These are discarded after mp3FramesToDiscard.
} drmp3_seek_point;

// Callback for when data is read. Return value is the number of bytes actually read.
//
// pUserData   [in]  The user data that was passed to drmp3_init(), drmp3_open() and family.
// pBufferOut  [out] The output buffer.
// bytesToRead [in]  The number of bytes to read.
//
// Returns the number of bytes actually read.
//
// A return value of less than bytesToRead indicates the end of the stream. Do _not_ return from this callback until
// either the entire bytesToRead is filled or you have reached the end of the stream.
typedef size_t (* drmp3_read_proc)(void* pUserData, void* pBufferOut, size_t bytesToRead);

// Callback for when data needs to be seeked.
//
// pUserData [in] The user data that was passed to drmp3_init(), drmp3_open() and family.
// offset    [in] The number of bytes to move, relative to the origin. Will never be negative.
// origin    [in] The origin of the seek - the current position or the start of the stream.
//
// Returns whether or not the seek was successful.
//
// Whether or not it is relative to the beginning or current position is determined by the "origin" parameter which
// will be either drmp3_seek_origin_start or drmp3_seek_origin_current.
typedef drmp3_bool32 (* drmp3_seek_proc)(void* pUserData, int offset, drmp3_seek_origin origin);

typedef struct
{
    drmp3_uint32 outputChannels;
    drmp3_uint32 outputSampleRate;
} drmp3_config;

typedef struct
{
    drmp3dec decoder;
    drmp3dec_frame_info frameInfo;
    drmp3_uint32 channels;
    drmp3_uint32 sampleRate;
    drmp3_read_proc onRead;
    drmp3_seek_proc onSeek;
    void* pUserData;
    drmp3_uint32 mp3FrameChannels;      // The number of channels in the currently loaded MP3 frame. Internal use only.
    drmp3_uint32 mp3FrameSampleRate;    // The sample rate of the currently loaded MP3 frame. Internal use only.
    drmp3_uint32 pcmFramesConsumedInMP3Frame;
    drmp3_uint32 pcmFramesRemainingInMP3Frame;
    drmp3_uint8 pcmFrames[sizeof(float)*DRMP3_MAX_SAMPLES_PER_FRAME];  // <-- Multipled by sizeof(float) to ensure there's enough room for DR_MP3_FLOAT_OUTPUT.
    drmp3_uint64 currentPCMFrame;       // The current PCM frame, globally, based on the output sample rate. Mainly used for seeking.
    drmp3_uint64 streamCursor;          // The current byte the decoder is sitting on in the raw stream.
    drmp3_src src;
    drmp3_seek_point* pSeekPoints;      // NULL by default. Set with drmp3_bind_seek_table(). Memory is owned by the client. dr_mp3 will never attempt to free this pointer.
    drmp3_uint32 seekPointCount;        // The number of items in pSeekPoints. When set to 0 assumes to no seek table. Defaults to zero.
    size_t dataSize;
    size_t dataCapacity;
    drmp3_uint8* pData;
    drmp3_bool32 atEnd : 1;
    struct
    {
        const drmp3_uint8* pData;
        size_t dataSize;
        size_t currentReadPos;
    } memory;   // Only used for decoders that were opened against a block of memory.
} drmp3;

// Initializes an MP3 decoder.
//
// onRead    [in]           The function to call when data needs to be read from the client.
// onSeek    [in]           The function to call when the read position of the client data needs to move.
// pUserData [in, optional] A pointer to application defined data that will be passed to onRead and onSeek.
//
// Returns true if successful; false otherwise.
//
// Close the loader with drmp3_uninit().
//
// See also: drmp3_init_file(), drmp3_init_memory(), drmp3_uninit()
drmp3_bool32 drmp3_init(drmp3* pMP3, drmp3_read_proc onRead, drmp3_seek_proc onSeek, void* pUserData, const drmp3_config* pConfig);

// Initializes an MP3 decoder from a block of memory.
//
// This does not create a copy of the data. It is up to the application to ensure the buffer remains valid for
// the lifetime of the drmp3 object.
//
// The buffer should contain the contents of the entire MP3 file.
drmp3_bool32 drmp3_init_memory(drmp3* pMP3, const void* pData, size_t dataSize, const drmp3_config* pConfig);

#ifndef DR_MP3_NO_STDIO
// Initializes an MP3 decoder from a file.
//
// This holds the internal FILE object until drmp3_uninit() is called. Keep this in mind if you're caching drmp3
// objects because the operating system may restrict the number of file handles an application can have open at
// any given time.
drmp3_bool32 drmp3_init_file(drmp3* pMP3, const char* filePath, const drmp3_config* pConfig);
#endif

// Uninitializes an MP3 decoder.
void drmp3_uninit(drmp3* pMP3);

// Reads PCM frames as interleaved 32-bit IEEE floating point PCM.
//
// Note that framesToRead specifies the number of PCM frames to read, _not_ the number of MP3 frames.
drmp3_uint64 drmp3_read_pcm_frames_f32(drmp3* pMP3, drmp3_uint64 framesToRead, float* pBufferOut);

// Seeks to a specific frame.
//
// Note that this is _not_ an MP3 frame, but rather a PCM frame.
drmp3_bool32 drmp3_seek_to_pcm_frame(drmp3* pMP3, drmp3_uint64 frameIndex);

// Calculates the total number of PCM frames in the MP3 stream. Cannot be used for infinite streams such as internet
// radio. Runs in linear time. Returns 0 on error.
drmp3_uint64 drmp3_get_pcm_frame_count(drmp3* pMP3);

// Calculates the total number of MP3 frames in the MP3 stream. Cannot be used for infinite streams such as internet
// radio. Runs in linear time. Returns 0 on error.
drmp3_uint64 drmp3_get_mp3_frame_count(drmp3* pMP3);

// Calculates the seekpoints based on PCM frames. This is slow.
//
// pSeekpoint count is a pointer to a uint32 containing the seekpoint count. On input it contains the desired count.
// On output it contains the actual count. The reason for this design is that the client may request too many
// seekpoints, in which case dr_mp3 will return a corrected count.
//
// Note that seektable seeking is not quite sample exact when the MP3 stream contains inconsistent sample rates.
drmp3_bool32 drmp3_calculate_seek_points(drmp3* pMP3, drmp3_uint32* pSeekPointCount, drmp3_seek_point* pSeekPoints);

// Binds a seek table to the decoder.
//
// This does _not_ make a copy of pSeekPoints - it only references it. It is up to the application to ensure this
// remains valid while it is bound to the decoder.
//
// Use drmp3_calculate_seek_points() to calculate the seek points.
drmp3_bool32 drmp3_bind_seek_table(drmp3* pMP3, drmp3_uint32 seekPointCount, drmp3_seek_point* pSeekPoints);



// Opens an decodes an entire MP3 stream as a single operation.
//
// pConfig is both an input and output. On input it contains what you want. On output it contains what you got.
//
// Free the returned pointer with drmp3_free().
float* drmp3_open_and_read_f32(drmp3_read_proc onRead, drmp3_seek_proc onSeek, void* pUserData, drmp3_config* pConfig, drmp3_uint64* pTotalFrameCount);
float* drmp3_open_memory_and_read_f32(const void* pData, size_t dataSize, drmp3_config* pConfig, drmp3_uint64* pTotalFrameCount);
#ifndef DR_MP3_NO_STDIO
float* drmp3_open_file_and_read_f32(const char* filePath, drmp3_config* pConfig, drmp3_uint64* pTotalFrameCount);
#endif

// Frees any memory that was allocated by a public drmp3 API.
void drmp3_free(void* p);

#ifdef __cplusplus
}
#endif
#endif  // dr_mp3_h


/////////////////////////////////////////////////////
//
// IMPLEMENTATION
//
/////////////////////////////////////////////////////
#ifdef DR_MP3_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h> // For INT_MAX

// Disable SIMD when compiling with TCC for now.
#if defined(__TINYC__)
#define DR_MP3_NO_SIMD
#endif

#define DRMP3_OFFSET_PTR(p, offset) ((void*)((drmp3_uint8*)(p) + (offset)))

#define DRMP3_MAX_FREE_FORMAT_FRAME_SIZE  2304    /* more than ISO spec's */
#ifndef DRMP3_MAX_FRAME_SYNC_MATCHES
#define DRMP3_MAX_FRAME_SYNC_MATCHES      10
#endif

#define DRMP3_MAX_L3_FRAME_PAYLOAD_BYTES  DRMP3_MAX_FREE_FORMAT_FRAME_SIZE /* MUST be >= 320000/8/32000*1152 = 1440 */

#define DRMP3_MAX_BITRESERVOIR_BYTES      511
#define DRMP3_SHORT_BLOCK_TYPE            2
#define DRMP3_STOP_BLOCK_TYPE             3
#define DRMP3_MODE_MONO                   3
#define DRMP3_MODE_JOINT_STEREO           1
#define DRMP3_HDR_SIZE                    4
#define DRMP3_HDR_IS_MONO(h)              (((h[3]) & 0xC0) == 0xC0)
#define DRMP3_HDR_IS_MS_STEREO(h)         (((h[3]) & 0xE0) == 0x60)
#define DRMP3_HDR_IS_FREE_FORMAT(h)       (((h[2]) & 0xF0) == 0)
#define DRMP3_HDR_IS_CRC(h)               (!((h[1]) & 1))
#define DRMP3_HDR_TEST_PADDING(h)         ((h[2]) & 0x2)
#define DRMP3_HDR_TEST_MPEG1(h)           ((h[1]) & 0x8)
#define DRMP3_HDR_TEST_NOT_MPEG25(h)      ((h[1]) & 0x10)
#define DRMP3_HDR_TEST_I_STEREO(h)        ((h[3]) & 0x10)
#define DRMP3_HDR_TEST_MS_STEREO(h)       ((h[3]) & 0x20)
#define DRMP3_HDR_GET_STEREO_MODE(h)      (((h[3]) >> 6) & 3)
#define DRMP3_HDR_GET_STEREO_MODE_EXT(h)  (((h[3]) >> 4) & 3)
#define DRMP3_HDR_GET_LAYER(h)            (((h[1]) >> 1) & 3)
#define DRMP3_HDR_GET_BITRATE(h)          ((h[2]) >> 4)
#define DRMP3_HDR_GET_SAMPLE_RATE(h)      (((h[2]) >> 2) & 3)
#define DRMP3_HDR_GET_MY_SAMPLE_RATE(h)   (DRMP3_HDR_GET_SAMPLE_RATE(h) + (((h[1] >> 3) & 1) + ((h[1] >> 4) & 1))*3)
#define DRMP3_HDR_IS_FRAME_576(h)         ((h[1] & 14) == 2)
#define DRMP3_HDR_IS_LAYER_1(h)           ((h[1] & 6) == 6)

#define DRMP3_BITS_DEQUANTIZER_OUT        -1
#define DRMP3_MAX_SCF                     (255 + DRMP3_BITS_DEQUANTIZER_OUT*4 - 210)
#define DRMP3_MAX_SCFI                    ((DRMP3_MAX_SCF + 3) & ~3)

#define DRMP3_MIN(a, b)           ((a) > (b) ? (b) : (a))
#define DRMP3_MAX(a, b)           ((a) < (b) ? (b) : (a))

#if !defined(DR_MP3_NO_SIMD)

#if !defined(DR_MP3_ONLY_SIMD) && (defined(_M_X64) || defined(_M_ARM64) || defined(__x86_64__) || defined(__aarch64__))
/* x64 always have SSE2, arm64 always have neon, no need for generic code */
#define DR_MP3_ONLY_SIMD
#endif

#if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || ((defined(__i386__) || defined(__x86_64__)) && defined(__SSE2__))
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#include <emmintrin.h>
#define DRMP3_HAVE_SSE 1
#define DRMP3_HAVE_SIMD 1
#define DRMP3_VSTORE _mm_storeu_ps
#define DRMP3_VLD _mm_loadu_ps
#define DRMP3_VSET _mm_set1_ps
#define DRMP3_VADD _mm_add_ps
#define DRMP3_VSUB _mm_sub_ps
#define DRMP3_VMUL _mm_mul_ps
#define DRMP3_VMAC(a, x, y) _mm_add_ps(a, _mm_mul_ps(x, y))
#define DRMP3_VMSB(a, x, y) _mm_sub_ps(a, _mm_mul_ps(x, y))
#define DRMP3_VMUL_S(x, s)  _mm_mul_ps(x, _mm_set1_ps(s))
#define DRMP3_VREV(x) _mm_shuffle_ps(x, x, _MM_SHUFFLE(0, 1, 2, 3))
typedef __m128 drmp3_f4;
#if defined(_MSC_VER) || defined(DR_MP3_ONLY_SIMD)
#define drmp3_cpuid __cpuid
#else
static __inline__ __attribute__((always_inline)) void drmp3_cpuid(int CPUInfo[], const int InfoType)
{
#if defined(__PIC__)
    __asm__ __volatile__(
#if defined(__x86_64__)
        "push %%rbx\n"
        "cpuid\n"
        "xchgl %%ebx, %1\n"
        "pop  %%rbx\n"
#else
        "xchgl %%ebx, %1\n"
        "cpuid\n"
        "xchgl %%ebx, %1\n"
#endif
        : "=a" (CPUInfo[0]), "=r" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3])
        : "a" (InfoType));
#else
    __asm__ __volatile__(
        "cpuid"
        : "=a" (CPUInfo[0]), "=b" (CPUInfo[1]), "=c" (CPUInfo[2]), "=d" (CPUInfo[3])
        : "a" (InfoType));
#endif
}
#endif
static int drmp3_have_simd()
{
#ifdef DR_MP3_ONLY_SIMD
    return 1;
#else
    static int g_have_simd;
    int CPUInfo[4];
#ifdef MINIMP3_TEST
    static int g_counter;
    if (g_counter++ > 100)
        return 0;
#endif
    if (g_have_simd)
        goto end;
    drmp3_cpuid(CPUInfo, 0);
    if (CPUInfo[0] > 0)
    {
        drmp3_cpuid(CPUInfo, 1);
        g_have_simd = (CPUInfo[3] & (1 << 26)) + 1; /* SSE2 */
        return g_have_simd - 1;
    }

end:
    return g_have_simd - 1;
#endif
}
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#define DRMP3_HAVE_SIMD 1
#define DRMP3_VSTORE vst1q_f32
#define DRMP3_VLD vld1q_f32
#define DRMP3_VSET vmovq_n_f32
#define DRMP3_VADD vaddq_f32
#define DRMP3_VSUB vsubq_f32
#define DRMP3_VMUL vmulq_f32
#define DRMP3_VMAC(a, x, y) vmlaq_f32(a, x, y)
#define DRMP3_VMSB(a, x, y) vmlsq_f32(a, x, y)
#define DRMP3_VMUL_S(x, s)  vmulq_f32(x, vmovq_n_f32(s))
#define DRMP3_VREV(x) vcombine_f32(vget_high_f32(vrev64q_f32(x)), vget_low_f32(vrev64q_f32(x)))
typedef float32x4_t drmp3_f4;
static int drmp3_have_simd()
{   /* TODO: detect neon for !DR_MP3_ONLY_SIMD */
    return 1;
}
#else
#define DRMP3_HAVE_SIMD 0
#ifdef DR_MP3_ONLY_SIMD
#error DR_MP3_ONLY_SIMD used, but SSE/NEON not enabled
#endif
#endif

#else

#define DRMP3_HAVE_SIMD 0

#endif

typedef struct
{
    const drmp3_uint8 *buf;
    int pos, limit;
} drmp3_bs;

typedef struct
{
    float scf[3*64];
    drmp3_uint8 total_bands, stereo_bands, bitalloc[64], scfcod[64];
} drmp3_L12_scale_info;

typedef struct
{
    drmp3_uint8 tab_offset, code_tab_width, band_count;
} drmp3_L12_subband_alloc;

typedef struct
{
    const drmp3_uint8 *sfbtab;
    drmp3_uint16 part_23_length, big_values, scalefac_compress;
    drmp3_uint8 global_gain, block_type, mixed_block_flag, n_long_sfb, n_short_sfb;
    drmp3_uint8 table_select[3], region_count[3], subblock_gain[3];
    drmp3_uint8 preflag, scalefac_scale, count1_table, scfsi;
} drmp3_L3_gr_info;

typedef struct
{
    drmp3_bs bs;
    drmp3_uint8 maindata[DRMP3_MAX_BITRESERVOIR_BYTES + DRMP3_MAX_L3_FRAME_PAYLOAD_BYTES];
    drmp3_L3_gr_info gr_info[4];
    float grbuf[2][576], scf[40], syn[18 + 15][2*32];
    drmp3_uint8 ist_pos[2][39];
} drmp3dec_scratch;

static void drmp3_bs_init(drmp3_bs *bs, const drmp3_uint8 *data, int bytes)
{
    bs->buf   = data;
    bs->pos   = 0;
    bs->limit = bytes*8;
}

static drmp3_uint32 drmp3_bs_get_bits(drmp3_bs *bs, int n)
{
    drmp3_uint32 next, cache = 0, s = bs->pos & 7;
    int shl = n + s;
    const drmp3_uint8 *p = bs->buf + (bs->pos >> 3);
    if ((bs->pos += n) > bs->limit)
        return 0;
    next = *p++ & (255 >> s);
    while ((shl -= 8) > 0)
    {
        cache |= next << shl;
        next = *p++;
    }
    return cache | (next >> -shl);
}

static int drmp3_hdr_valid(const drmp3_uint8 *h)
{
    return h[0] == 0xff &&
        ((h[1] & 0xF0) == 0xf0 || (h[1] & 0xFE) == 0xe2) &&
        (DRMP3_HDR_GET_LAYER(h) != 0) &&
        (DRMP3_HDR_GET_BITRATE(h) != 15) &&
        (DRMP3_HDR_GET_SAMPLE_RATE(h) != 3);
}

static int drmp3_hdr_compare(const drmp3_uint8 *h1, const drmp3_uint8 *h2)
{
    return drmp3_hdr_valid(h2) &&
        ((h1[1] ^ h2[1]) & 0xFE) == 0 &&
        ((h1[2] ^ h2[2]) & 0x0C) == 0 &&
        !(DRMP3_HDR_IS_FREE_FORMAT(h1) ^ DRMP3_HDR_IS_FREE_FORMAT(h2));
}

static unsigned drmp3_hdr_bitrate_kbps(const drmp3_uint8 *h)
{
    static const drmp3_uint8 halfrate[2][3][15] = {
        { { 0,4,8,12,16,20,24,28,32,40,48,56,64,72,80 }, { 0,4,8,12,16,20,24,28,32,40,48,56,64,72,80 }, { 0,16,24,28,32,40,48,56,64,72,80,88,96,112,128 } },
        { { 0,16,20,24,28,32,40,48,56,64,80,96,112,128,160 }, { 0,16,24,28,32,40,48,56,64,80,96,112,128,160,192 }, { 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224 } },
    };
    return 2*halfrate[!!DRMP3_HDR_TEST_MPEG1(h)][DRMP3_HDR_GET_LAYER(h) - 1][DRMP3_HDR_GET_BITRATE(h)];
}

static unsigned drmp3_hdr_sample_rate_hz(const drmp3_uint8 *h)
{
    static const unsigned g_hz[3] = { 44100, 48000, 32000 };
    return g_hz[DRMP3_HDR_GET_SAMPLE_RATE(h)] >> (int)!DRMP3_HDR_TEST_MPEG1(h) >> (int)!DRMP3_HDR_TEST_NOT_MPEG25(h);
}

static unsigned drmp3_hdr_frame_samples(const drmp3_uint8 *h)
{
    return DRMP3_HDR_IS_LAYER_1(h) ? 384 : (1152 >> (int)DRMP3_HDR_IS_FRAME_576(h));
}

static int drmp3_hdr_frame_bytes(const drmp3_uint8 *h, int free_format_size)
{
    int frame_bytes = drmp3_hdr_frame_samples(h)*drmp3_hdr_bitrate_kbps(h)*125/drmp3_hdr_sample_rate_hz(h);
    if (DRMP3_HDR_IS_LAYER_1(h))
    {
        frame_bytes &= ~3; /* slot align */
    }
    return frame_bytes ? frame_bytes : free_format_size;
}

static int drmp3_hdr_padding(const drmp3_uint8 *h)
{
    return DRMP3_HDR_TEST_PADDING(h) ? (DRMP3_HDR_IS_LAYER_1(h) ? 4 : 1) : 0;
}

#ifndef DR_MP3_ONLY_MP3
static const drmp3_L12_subband_alloc *drmp3_L12_subband_alloc_table(const drmp3_uint8 *hdr, drmp3_L12_scale_info *sci)
{
    const drmp3_L12_subband_alloc *alloc;
    int mode = DRMP3_HDR_GET_STEREO_MODE(hdr);
    int nbands, stereo_bands = (mode == DRMP3_MODE_MONO) ? 0 : (mode == DRMP3_MODE_JOINT_STEREO) ? (DRMP3_HDR_GET_STEREO_MODE_EXT(hdr) << 2) + 4 : 32;

    if (DRMP3_HDR_IS_LAYER_1(hdr))
    {
        static const drmp3_L12_subband_alloc g_alloc_L1[] = { { 76, 4, 32 } };
        alloc = g_alloc_L1;
        nbands = 32;
    } else if (!DRMP3_HDR_TEST_MPEG1(hdr))
    {
        static const drmp3_L12_subband_alloc g_alloc_L2M2[] = { { 60, 4, 4 }, { 44, 3, 7 }, { 44, 2, 19 } };
        alloc = g_alloc_L2M2;
        nbands = 30;
    } else
    {
        static const drmp3_L12_subband_alloc g_alloc_L2M1[] = { { 0, 4, 3 }, { 16, 4, 8 }, { 32, 3, 12 }, { 40, 2, 7 } };
        int sample_rate_idx = DRMP3_HDR_GET_SAMPLE_RATE(hdr);
        unsigned kbps = drmp3_hdr_bitrate_kbps(hdr) >> (int)(mode != DRMP3_MODE_MONO);
        if (!kbps) /* free-format */
        {
            kbps = 192;
        }

        alloc = g_alloc_L2M1;
        nbands = 27;
        if (kbps < 56)
        {
            static const drmp3_L12_subband_alloc g_alloc_L2M1_lowrate[] = { { 44, 4, 2 }, { 44, 3, 10 } };
            alloc = g_alloc_L2M1_lowrate;
            nbands = sample_rate_idx == 2 ? 12 : 8;
        } else if (kbps >= 96 && sample_rate_idx != 1)
        {
            nbands = 30;
        }
    }

    sci->total_bands = (drmp3_uint8)nbands;
    sci->stereo_bands = (drmp3_uint8)DRMP3_MIN(stereo_bands, nbands);

    return alloc;
}

static void drmp3_L12_read_scalefactors(drmp3_bs *bs, drmp3_uint8 *pba, drmp3_uint8 *scfcod, int bands, float *scf)
{
    static const float g_deq_L12[18*3] = {
#define DRMP3_DQ(x) 9.53674316e-07f/x, 7.56931807e-07f/x, 6.00777173e-07f/x
        DRMP3_DQ(3),DRMP3_DQ(7),DRMP3_DQ(15),DRMP3_DQ(31),DRMP3_DQ(63),DRMP3_DQ(127),DRMP3_DQ(255),DRMP3_DQ(511),DRMP3_DQ(1023),DRMP3_DQ(2047),DRMP3_DQ(4095),DRMP3_DQ(8191),DRMP3_DQ(16383),DRMP3_DQ(32767),DRMP3_DQ(65535),DRMP3_DQ(3),DRMP3_DQ(5),DRMP3_DQ(9)
    };
    int i, m;
    for (i = 0; i < bands; i++)
    {
        float s = 0;
        int ba = *pba++;
        int mask = ba ? 4 + ((19 >> scfcod[i]) & 3) : 0;
        for (m = 4; m; m >>= 1)
        {
            if (mask & m)
            {
                int b = drmp3_bs_get_bits(bs, 6);
                s = g_deq_L12[ba*3 - 6 + b % 3]*(1 << 21 >> b/3);
            }
            *scf++ = s;
        }
    }
}

static void drmp3_L12_read_scale_info(const drmp3_uint8 *hdr, drmp3_bs *bs, drmp3_L12_scale_info *sci)
{
    static const drmp3_uint8 g_bitalloc_code_tab[] = {
        0,17, 3, 4, 5,6,7, 8,9,10,11,12,13,14,15,16,
        0,17,18, 3,19,4,5, 6,7, 8, 9,10,11,12,13,16,
        0,17,18, 3,19,4,5,16,
        0,17,18,16,
        0,17,18,19, 4,5,6, 7,8, 9,10,11,12,13,14,15,
        0,17,18, 3,19,4,5, 6,7, 8, 9,10,11,12,13,14,
        0, 2, 3, 4, 5,6,7, 8,9,10,11,12,13,14,15,16
    };
    const drmp3_L12_subband_alloc *subband_alloc = drmp3_L12_subband_alloc_table(hdr, sci);

    int i, k = 0, ba_bits = 0;
    const drmp3_uint8 *ba_code_tab = g_bitalloc_code_tab;

    for (i = 0; i < sci->total_bands; i++)
    {
        drmp3_uint8 ba;
        if (i == k)
        {
            k += subband_alloc->band_count;
            ba_bits = subband_alloc->code_tab_width;
            ba_code_tab = g_bitalloc_code_tab + subband_alloc->tab_offset;
            subband_alloc++;
        }
        ba = ba_code_tab[drmp3_bs_get_bits(bs, ba_bits)];
        sci->bitalloc[2*i] = ba;
        if (i < sci->stereo_bands)
        {
            ba = ba_code_tab[drmp3_bs_get_bits(bs, ba_bits)];
        }
        sci->bitalloc[2*i + 1] = sci->stereo_bands ? ba : 0;
    }

    for (i = 0; i < 2*sci->total_bands; i++)
    {
        sci->scfcod[i] = (drmp3_uint8)(sci->bitalloc[i] ? DRMP3_HDR_IS_LAYER_1(hdr) ? 2 : drmp3_bs_get_bits(bs, 2) : 6);
    }

    drmp3_L12_read_scalefactors(bs, sci->bitalloc, sci->scfcod, sci->total_bands*2, sci->scf);

    for (i = sci->stereo_bands; i < sci->total_bands; i++)
    {
        sci->bitalloc[2*i + 1] = 0;
    }
}

static int drmp3_L12_dequantize_granule(float *grbuf, drmp3_bs *bs, drmp3_L12_scale_info *sci, int group_size)
{
    int i, j, k, choff = 576;
    for (j = 0; j < 4; j++)
    {
        float *dst = grbuf + group_size*j;
        for (i = 0; i < 2*sci->total_bands; i++)
        {
            int ba = sci->bitalloc[i];
            if (ba != 0)
            {
                if (ba < 17)
                {
                    int half = (1 << (ba - 1)) - 1;
                    for (k = 0; k < group_size; k++)
                    {
                        dst[k] = (float)((int)drmp3_bs_get_bits(bs, ba) - half);
                    }
                } else
                {
                    unsigned mod = (2 << (ba - 17)) + 1;    /* 3, 5, 9 */
                    unsigned code = drmp3_bs_get_bits(bs, mod + 2 - (mod >> 3));  /* 5, 7, 10 */
                    for (k = 0; k < group_size; k++, code /= mod)
                    {
                        dst[k] = (float)((int)(code % mod - mod/2));
                    }
                }
            }
            dst += choff;
            choff = 18 - choff;
        }
    }
    return group_size*4;
}

static void drmp3_L12_apply_scf_384(drmp3_L12_scale_info *sci, const float *scf, float *dst)
{
    int i, k;
    memcpy(dst + 576 + sci->stereo_bands*18, dst + sci->stereo_bands*18, (sci->total_bands - sci->stereo_bands)*18*sizeof(float));
    for (i = 0; i < sci->total_bands; i++, dst += 18, scf += 6)
    {
        for (k = 0; k < 12; k++)
        {
            dst[k + 0]   *= scf[0];
            dst[k + 576] *= scf[3];
        }
    }
}
#endif

static int drmp3_L3_read_side_info(drmp3_bs *bs, drmp3_L3_gr_info *gr, const drmp3_uint8 *hdr)
{
    static const drmp3_uint8 g_scf_long[8][23] = {
        { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
        { 12,12,12,12,12,12,16,20,24,28,32,40,48,56,64,76,90,2,2,2,2,2,0 },
        { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
        { 6,6,6,6,6,6,8,10,12,14,16,18,22,26,32,38,46,54,62,70,76,36,0 },
        { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
        { 4,4,4,4,4,4,6,6,8,8,10,12,16,20,24,28,34,42,50,54,76,158,0 },
        { 4,4,4,4,4,4,6,6,6,8,10,12,16,18,22,28,34,40,46,54,54,192,0 },
        { 4,4,4,4,4,4,6,6,8,10,12,16,20,24,30,38,46,56,68,84,102,26,0 }
    };
    static const drmp3_uint8 g_scf_short[8][40] = {
        { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 8,8,8,8,8,8,8,8,8,12,12,12,16,16,16,20,20,20,24,24,24,28,28,28,36,36,36,2,2,2,2,2,2,2,2,2,26,26,26,0 },
        { 4,4,4,4,4,4,4,4,4,6,6,6,6,6,6,8,8,8,10,10,10,14,14,14,18,18,18,26,26,26,32,32,32,42,42,42,18,18,18,0 },
        { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,32,32,32,44,44,44,12,12,12,0 },
        { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,22,22,22,30,30,30,56,56,56,0 },
        { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,6,6,6,10,10,10,12,12,12,14,14,14,16,16,16,20,20,20,26,26,26,66,66,66,0 },
        { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,12,12,12,16,16,16,20,20,20,26,26,26,34,34,34,42,42,42,12,12,12,0 }
    };
    static const drmp3_uint8 g_scf_mixed[8][40] = {
        { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 12,12,12,4,4,4,8,8,8,12,12,12,16,16,16,20,20,20,24,24,24,28,28,28,36,36,36,2,2,2,2,2,2,2,2,2,26,26,26,0 },
        { 6,6,6,6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,14,14,14,18,18,18,26,26,26,32,32,32,42,42,42,18,18,18,0 },
        { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,32,32,32,44,44,44,12,12,12,0 },
        { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
        { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,22,22,22,30,30,30,56,56,56,0 },
        { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,6,6,6,10,10,10,12,12,12,14,14,14,16,16,16,20,20,20,26,26,26,66,66,66,0 },
        { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,8,8,8,12,12,12,16,16,16,20,20,20,26,26,26,34,34,34,42,42,42,12,12,12,0 }
    };

    unsigned tables, scfsi = 0;
    int main_data_begin, part_23_sum = 0;
    int sr_idx = DRMP3_HDR_GET_MY_SAMPLE_RATE(hdr); sr_idx -= (sr_idx != 0);
    int gr_count = DRMP3_HDR_IS_MONO(hdr) ? 1 : 2;

    if (DRMP3_HDR_TEST_MPEG1(hdr))
    {
        gr_count *= 2;
        main_data_begin = drmp3_bs_get_bits(bs, 9);
        scfsi = drmp3_bs_get_bits(bs, 7 + gr_count);
    } else
    {
        main_data_begin = drmp3_bs_get_bits(bs, 8 + gr_count) >> gr_count;
    }

    do
    {
        if (DRMP3_HDR_IS_MONO(hdr))
        {
            scfsi <<= 4;
        }
        gr->part_23_length = (drmp3_uint16)drmp3_bs_get_bits(bs, 12);
        part_23_sum += gr->part_23_length;
        gr->big_values = (drmp3_uint16)drmp3_bs_get_bits(bs,  9);
        if (gr->big_values > 288)
        {
            return -1;
        }
        gr->global_gain = (drmp3_uint8)drmp3_bs_get_bits(bs, 8);
        gr->scalefac_compress = (drmp3_uint16)drmp3_bs_get_bits(bs, DRMP3_HDR_TEST_MPEG1(hdr) ? 4 : 9);
        gr->sfbtab = g_scf_long[sr_idx];
        gr->n_long_sfb  = 22;
        gr->n_short_sfb = 0;
        if (drmp3_bs_get_bits(bs, 1))
        {
            gr->block_type = (drmp3_uint8)drmp3_bs_get_bits(bs, 2);
            if (!gr->block_type)
            {
                return -1;
            }
            gr->mixed_block_flag = (drmp3_uint8)drmp3_bs_get_bits(bs, 1);
            gr->region_count[0] = 7;
            gr->region_count[1] = 255;
            if (gr->block_type == DRMP3_SHORT_BLOCK_TYPE)
            {
                scfsi &= 0x0F0F;
                if (!gr->mixed_block_flag)
                {
                    gr->region_count[0] = 8;
                    gr->sfbtab = g_scf_short[sr_idx];
                    gr->n_long_sfb = 0;
                    gr->n_short_sfb = 39;
                } else
                {
                    gr->sfbtab = g_scf_mixed[sr_idx];
                    gr->n_long_sfb = DRMP3_HDR_TEST_MPEG1(hdr) ? 8 : 6;
                    gr->n_short_sfb = 30;
                }
            }
            tables = drmp3_bs_get_bits(bs, 10);
            tables <<= 5;
            gr->subblock_gain[0] = (drmp3_uint8)drmp3_bs_get_bits(bs, 3);
            gr->subblock_gain[1] = (drmp3_uint8)drmp3_bs_get_bits(bs, 3);
            gr->subblock_gain[2] = (drmp3_uint8)drmp3_bs_get_bits(bs, 3);
        } else
        {
            gr->block_type = 0;
            gr->mixed_block_flag = 0;
            tables = drmp3_bs_get_bits(bs, 15);
            gr->region_count[0] = (drmp3_uint8)drmp3_bs_get_bits(bs, 4);
            gr->region_count[1] = (drmp3_uint8)drmp3_bs_get_bits(bs, 3);
            gr->region_count[2] = 255;
        }
        gr->table_select[0] = (drmp3_uint8)(tables >> 10);
        gr->table_select[1] = (drmp3_uint8)((tables >> 5) & 31);
        gr->table_select[2] = (drmp3_uint8)((tables) & 31);
        gr->preflag = (drmp3_uint8)(DRMP3_HDR_TEST_MPEG1(hdr) ? drmp3_bs_get_bits(bs, 1) : (gr->scalefac_compress >= 500));
        gr->scalefac_scale = (drmp3_uint8)drmp3_bs_get_bits(bs, 1);
        gr->count1_table = (drmp3_uint8)drmp3_bs_get_bits(bs, 1);
        gr->scfsi = (drmp3_uint8)((scfsi >> 12) & 15);
        scfsi <<= 4;
        gr++;
    } while(--gr_count);

    if (part_23_sum + bs->pos > bs->limit + main_data_begin*8)
    {
        return -1;
    }

    return main_data_begin;
}

static void drmp3_L3_read_scalefactors(drmp3_uint8 *scf, drmp3_uint8 *ist_pos, const drmp3_uint8 *scf_size, const drmp3_uint8 *scf_count, drmp3_bs *bitbuf, int scfsi)
{
    int i, k;
    for (i = 0; i < 4 && scf_count[i]; i++, scfsi *= 2)
    {
        int cnt = scf_count[i];
        if (scfsi & 8)
        {
            memcpy(scf, ist_pos, cnt);
        } else
        {
            int bits = scf_size[i];
            if (!bits)
            {
                memset(scf, 0, cnt);
                memset(ist_pos, 0, cnt);
            } else
            {
                int max_scf = (scfsi < 0) ? (1 << bits) - 1 : -1;
                for (k = 0; k < cnt; k++)
                {
                    int s = drmp3_bs_get_bits(bitbuf, bits);
                    ist_pos[k] = (drmp3_uint8)(s == max_scf ? -1 : s);
                    scf[k] = (drmp3_uint8)s;
                }
            }
        }
        ist_pos += cnt;
        scf += cnt;
    }
    scf[0] = scf[1] = scf[2] = 0;
}

static float drmp3_L3_ldexp_q2(float y, int exp_q2)
{
    static const float g_expfrac[4] = { 9.31322575e-10f,7.83145814e-10f,6.58544508e-10f,5.53767716e-10f };
    int e;
    do
    {
        e = DRMP3_MIN(30*4, exp_q2);
        y *= g_expfrac[e & 3]*(1 << 30 >> (e >> 2));
    } while ((exp_q2 -= e) > 0);
    return y;
}

static void drmp3_L3_decode_scalefactors(const drmp3_uint8 *hdr, drmp3_uint8 *ist_pos, drmp3_bs *bs, const drmp3_L3_gr_info *gr, float *scf, int ch)
{
    static const drmp3_uint8 g_scf_partitions[3][28] = {
        { 6,5,5, 5,6,5,5,5,6,5, 7,3,11,10,0,0, 7, 7, 7,0, 6, 6,6,3, 8, 8,5,0 },
        { 8,9,6,12,6,9,9,9,6,9,12,6,15,18,0,0, 6,15,12,0, 6,12,9,6, 6,18,9,0 },
        { 9,9,6,12,9,9,9,9,9,9,12,6,18,18,0,0,12,12,12,0,12, 9,9,6,15,12,9,0 }
    };
    const drmp3_uint8 *scf_partition = g_scf_partitions[!!gr->n_short_sfb + !gr->n_long_sfb];
    drmp3_uint8 scf_size[4], iscf[40];
    int i, scf_shift = gr->scalefac_scale + 1, gain_exp, scfsi = gr->scfsi;
    float gain;

    if (DRMP3_HDR_TEST_MPEG1(hdr))
    {
        static const drmp3_uint8 g_scfc_decode[16] = { 0,1,2,3, 12,5,6,7, 9,10,11,13, 14,15,18,19 };
        int part = g_scfc_decode[gr->scalefac_compress];
        scf_size[1] = scf_size[0] = (drmp3_uint8)(part >> 2);
        scf_size[3] = scf_size[2] = (drmp3_uint8)(part & 3);
    } else
    {
        static const drmp3_uint8 g_mod[6*4] = { 5,5,4,4,5,5,4,1,4,3,1,1,5,6,6,1,4,4,4,1,4,3,1,1 };
        int k, modprod, sfc, ist = DRMP3_HDR_TEST_I_STEREO(hdr) && ch;
        sfc = gr->scalefac_compress >> ist;
        for (k = ist*3*4; sfc >= 0; sfc -= modprod, k += 4)
        {
            for (modprod = 1, i = 3; i >= 0; i--)
            {
                scf_size[i] = (drmp3_uint8)(sfc / modprod % g_mod[k + i]);
                modprod *= g_mod[k + i];
            }
        }
        scf_partition += k;
        scfsi = -16;
    }
    drmp3_L3_read_scalefactors(iscf, ist_pos, scf_size, scf_partition, bs, scfsi);

    if (gr->n_short_sfb)
    {
        int sh = 3 - scf_shift;
        for (i = 0; i < gr->n_short_sfb; i += 3)
        {
            iscf[gr->n_long_sfb + i + 0] += gr->subblock_gain[0] << sh;
            iscf[gr->n_long_sfb + i + 1] += gr->subblock_gain[1] << sh;
            iscf[gr->n_long_sfb + i + 2] += gr->subblock_gain[2] << sh;
        }
    } else if (gr->preflag)
    {
        static const drmp3_uint8 g_preamp[10] = { 1,1,1,1,2,2,3,3,3,2 };
        for (i = 0; i < 10; i++)
        {
            iscf[11 + i] += g_preamp[i];
        }
    }

    gain_exp = gr->global_gain + DRMP3_BITS_DEQUANTIZER_OUT*4 - 210 - (DRMP3_HDR_IS_MS_STEREO(hdr) ? 2 : 0);
    gain = drmp3_L3_ldexp_q2(1 << (DRMP3_MAX_SCFI/4),  DRMP3_MAX_SCFI - gain_exp);
    for (i = 0; i < (int)(gr->n_long_sfb + gr->n_short_sfb); i++)
    {
        scf[i] = drmp3_L3_ldexp_q2(gain, iscf[i] << scf_shift);
    }
}

static const float g_drmp3_pow43[129 + 16] = {
    0,-1,-2.519842f,-4.326749f,-6.349604f,-8.549880f,-10.902724f,-13.390518f,-16.000000f,-18.720754f,-21.544347f,-24.463781f,-27.473142f,-30.567351f,-33.741992f,-36.993181f,
    0,1,2.519842f,4.326749f,6.349604f,8.549880f,10.902724f,13.390518f,16.000000f,18.720754f,21.544347f,24.463781f,27.473142f,30.567351f,33.741992f,36.993181f,40.317474f,43.711787f,47.173345f,50.699631f,54.288352f,57.937408f,61.644865f,65.408941f,69.227979f,73.100443f,77.024898f,81.000000f,85.024491f,89.097188f,93.216975f,97.382800f,101.593667f,105.848633f,110.146801f,114.487321f,118.869381f,123.292209f,127.755065f,132.257246f,136.798076f,141.376907f,145.993119f,150.646117f,155.335327f,160.060199f,164.820202f,169.614826f,174.443577f,179.305980f,184.201575f,189.129918f,194.090580f,199.083145f,204.107210f,209.162385f,214.248292f,219.364564f,224.510845f,229.686789f,234.892058f,240.126328f,245.389280f,250.680604f,256.000000f,261.347174f,266.721841f,272.123723f,277.552547f,283.008049f,288.489971f,293.998060f,299.532071f,305.091761f,310.676898f,316.287249f,321.922592f,327.582707f,333.267377f,338.976394f,344.709550f,350.466646f,356.247482f,362.051866f,367.879608f,373.730522f,379.604427f,385.501143f,391.420496f,397.362314f,403.326427f,409.312672f,415.320884f,421.350905f,427.402579f,433.475750f,439.570269f,445.685987f,451.822757f,457.980436f,464.158883f,470.357960f,476.577530f,482.817459f,489.077615f,495.357868f,501.658090f,507.978156f,514.317941f,520.677324f,527.056184f,533.454404f,539.871867f,546.308458f,552.764065f,559.238575f,565.731879f,572.243870f,578.774440f,585.323483f,591.890898f,598.476581f,605.080431f,611.702349f,618.342238f,625.000000f,631.675540f,638.368763f,645.079578f
};

static float drmp3_L3_pow_43(int x)
{
    float frac;
    int sign, mult = 256;

    if (x < 129)
    {
        return g_drmp3_pow43[16 + x];
    }

    if (x < 1024)
    {
        mult = 16;
        x <<= 3;
    }

    sign = 2*x & 64;
    frac = (float)((x & 63) - sign) / ((x & ~63) + sign);
    return g_drmp3_pow43[16 + ((x + sign) >> 6)]*(1.f + frac*((4.f/3) + frac*(2.f/9)))*mult;
}

static void drmp3_L3_huffman(float *dst, drmp3_bs *bs, const drmp3_L3_gr_info *gr_info, const float *scf, int layer3gr_limit)
{
    static const drmp3_int16 tabs[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        785,785,785,785,784,784,784,784,513,513,513,513,513,513,513,513,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,
        -255,1313,1298,1282,785,785,785,785,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,290,288,
        -255,1313,1298,1282,769,769,769,769,529,529,529,529,529,529,529,529,528,528,528,528,528,528,528,528,512,512,512,512,512,512,512,512,290,288,
        -253,-318,-351,-367,785,785,785,785,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,819,818,547,547,275,275,275,275,561,560,515,546,289,274,288,258,
        -254,-287,1329,1299,1314,1312,1057,1057,1042,1042,1026,1026,784,784,784,784,529,529,529,529,529,529,529,529,769,769,769,769,768,768,768,768,563,560,306,306,291,259,
        -252,-413,-477,-542,1298,-575,1041,1041,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-383,-399,1107,1092,1106,1061,849,849,789,789,1104,1091,773,773,1076,1075,341,340,325,309,834,804,577,577,532,532,516,516,832,818,803,816,561,561,531,531,515,546,289,289,288,258,
        -252,-429,-493,-559,1057,1057,1042,1042,529,529,529,529,529,529,529,529,784,784,784,784,769,769,769,769,512,512,512,512,512,512,512,512,-382,1077,-415,1106,1061,1104,849,849,789,789,1091,1076,1029,1075,834,834,597,581,340,340,339,324,804,833,532,532,832,772,818,803,817,787,816,771,290,290,290,290,288,258,
        -253,-349,-414,-447,-463,1329,1299,-479,1314,1312,1057,1057,1042,1042,1026,1026,785,785,785,785,784,784,784,784,769,769,769,769,768,768,768,768,-319,851,821,-335,836,850,805,849,341,340,325,336,533,533,579,579,564,564,773,832,578,548,563,516,321,276,306,291,304,259,
        -251,-572,-733,-830,-863,-879,1041,1041,784,784,784,784,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-511,-527,-543,1396,1351,1381,1366,1395,1335,1380,-559,1334,1138,1138,1063,1063,1350,1392,1031,1031,1062,1062,1364,1363,1120,1120,1333,1348,881,881,881,881,375,374,359,373,343,358,341,325,791,791,1123,1122,-703,1105,1045,-719,865,865,790,790,774,774,1104,1029,338,293,323,308,-799,-815,833,788,772,818,803,816,322,292,307,320,561,531,515,546,289,274,288,258,
        -251,-525,-605,-685,-765,-831,-846,1298,1057,1057,1312,1282,785,785,785,785,784,784,784,784,769,769,769,769,512,512,512,512,512,512,512,512,1399,1398,1383,1367,1382,1396,1351,-511,1381,1366,1139,1139,1079,1079,1124,1124,1364,1349,1363,1333,882,882,882,882,807,807,807,807,1094,1094,1136,1136,373,341,535,535,881,775,867,822,774,-591,324,338,-671,849,550,550,866,864,609,609,293,336,534,534,789,835,773,-751,834,804,308,307,833,788,832,772,562,562,547,547,305,275,560,515,290,290,
        -252,-397,-477,-557,-622,-653,-719,-735,-750,1329,1299,1314,1057,1057,1042,1042,1312,1282,1024,1024,785,785,785,785,784,784,784,784,769,769,769,769,-383,1127,1141,1111,1126,1140,1095,1110,869,869,883,883,1079,1109,882,882,375,374,807,868,838,881,791,-463,867,822,368,263,852,837,836,-543,610,610,550,550,352,336,534,534,865,774,851,821,850,805,593,533,579,564,773,832,578,578,548,548,577,577,307,276,306,291,516,560,259,259,
        -250,-2107,-2507,-2764,-2909,-2974,-3007,-3023,1041,1041,1040,1040,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-767,-1052,-1213,-1277,-1358,-1405,-1469,-1535,-1550,-1582,-1614,-1647,-1662,-1694,-1726,-1759,-1774,-1807,-1822,-1854,-1886,1565,-1919,-1935,-1951,-1967,1731,1730,1580,1717,-1983,1729,1564,-1999,1548,-2015,-2031,1715,1595,-2047,1714,-2063,1610,-2079,1609,-2095,1323,1323,1457,1457,1307,1307,1712,1547,1641,1700,1699,1594,1685,1625,1442,1442,1322,1322,-780,-973,-910,1279,1278,1277,1262,1276,1261,1275,1215,1260,1229,-959,974,974,989,989,-943,735,478,478,495,463,506,414,-1039,1003,958,1017,927,942,987,957,431,476,1272,1167,1228,-1183,1256,-1199,895,895,941,941,1242,1227,1212,1135,1014,1014,490,489,503,487,910,1013,985,925,863,894,970,955,1012,847,-1343,831,755,755,984,909,428,366,754,559,-1391,752,486,457,924,997,698,698,983,893,740,740,908,877,739,739,667,667,953,938,497,287,271,271,683,606,590,712,726,574,302,302,738,736,481,286,526,725,605,711,636,724,696,651,589,681,666,710,364,467,573,695,466,466,301,465,379,379,709,604,665,679,316,316,634,633,436,436,464,269,424,394,452,332,438,363,347,408,393,448,331,422,362,407,392,421,346,406,391,376,375,359,1441,1306,-2367,1290,-2383,1337,-2399,-2415,1426,1321,-2431,1411,1336,-2447,-2463,-2479,1169,1169,1049,1049,1424,1289,1412,1352,1319,-2495,1154,1154,1064,1064,1153,1153,416,390,360,404,403,389,344,374,373,343,358,372,327,357,342,311,356,326,1395,1394,1137,1137,1047,1047,1365,1392,1287,1379,1334,1364,1349,1378,1318,1363,792,792,792,792,1152,1152,1032,1032,1121,1121,1046,1046,1120,1120,1030,1030,-2895,1106,1061,1104,849,849,789,789,1091,1076,1029,1090,1060,1075,833,833,309,324,532,532,832,772,818,803,561,561,531,560,515,546,289,274,288,258,
        -250,-1179,-1579,-1836,-1996,-2124,-2253,-2333,-2413,-2477,-2542,-2574,-2607,-2622,-2655,1314,1313,1298,1312,1282,785,785,785,785,1040,1040,1025,1025,768,768,768,768,-766,-798,-830,-862,-895,-911,-927,-943,-959,-975,-991,-1007,-1023,-1039,-1055,-1070,1724,1647,-1103,-1119,1631,1767,1662,1738,1708,1723,-1135,1780,1615,1779,1599,1677,1646,1778,1583,-1151,1777,1567,1737,1692,1765,1722,1707,1630,1751,1661,1764,1614,1736,1676,1763,1750,1645,1598,1721,1691,1762,1706,1582,1761,1566,-1167,1749,1629,767,766,751,765,494,494,735,764,719,749,734,763,447,447,748,718,477,506,431,491,446,476,461,505,415,430,475,445,504,399,460,489,414,503,383,474,429,459,502,502,746,752,488,398,501,473,413,472,486,271,480,270,-1439,-1455,1357,-1471,-1487,-1503,1341,1325,-1519,1489,1463,1403,1309,-1535,1372,1448,1418,1476,1356,1462,1387,-1551,1475,1340,1447,1402,1386,-1567,1068,1068,1474,1461,455,380,468,440,395,425,410,454,364,467,466,464,453,269,409,448,268,432,1371,1473,1432,1417,1308,1460,1355,1446,1459,1431,1083,1083,1401,1416,1458,1445,1067,1067,1370,1457,1051,1051,1291,1430,1385,1444,1354,1415,1400,1443,1082,1082,1173,1113,1186,1066,1185,1050,-1967,1158,1128,1172,1097,1171,1081,-1983,1157,1112,416,266,375,400,1170,1142,1127,1065,793,793,1169,1033,1156,1096,1141,1111,1155,1080,1126,1140,898,898,808,808,897,897,792,792,1095,1152,1032,1125,1110,1139,1079,1124,882,807,838,881,853,791,-2319,867,368,263,822,852,837,866,806,865,-2399,851,352,262,534,534,821,836,594,594,549,549,593,593,533,533,848,773,579,579,564,578,548,563,276,276,577,576,306,291,516,560,305,305,275,259,
        -251,-892,-2058,-2620,-2828,-2957,-3023,-3039,1041,1041,1040,1040,769,769,769,769,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,-511,-527,-543,-559,1530,-575,-591,1528,1527,1407,1526,1391,1023,1023,1023,1023,1525,1375,1268,1268,1103,1103,1087,1087,1039,1039,1523,-604,815,815,815,815,510,495,509,479,508,463,507,447,431,505,415,399,-734,-782,1262,-815,1259,1244,-831,1258,1228,-847,-863,1196,-879,1253,987,987,748,-767,493,493,462,477,414,414,686,669,478,446,461,445,474,429,487,458,412,471,1266,1264,1009,1009,799,799,-1019,-1276,-1452,-1581,-1677,-1757,-1821,-1886,-1933,-1997,1257,1257,1483,1468,1512,1422,1497,1406,1467,1496,1421,1510,1134,1134,1225,1225,1466,1451,1374,1405,1252,1252,1358,1480,1164,1164,1251,1251,1238,1238,1389,1465,-1407,1054,1101,-1423,1207,-1439,830,830,1248,1038,1237,1117,1223,1148,1236,1208,411,426,395,410,379,269,1193,1222,1132,1235,1221,1116,976,976,1192,1162,1177,1220,1131,1191,963,963,-1647,961,780,-1663,558,558,994,993,437,408,393,407,829,978,813,797,947,-1743,721,721,377,392,844,950,828,890,706,706,812,859,796,960,948,843,934,874,571,571,-1919,690,555,689,421,346,539,539,944,779,918,873,932,842,903,888,570,570,931,917,674,674,-2575,1562,-2591,1609,-2607,1654,1322,1322,1441,1441,1696,1546,1683,1593,1669,1624,1426,1426,1321,1321,1639,1680,1425,1425,1305,1305,1545,1668,1608,1623,1667,1592,1638,1666,1320,1320,1652,1607,1409,1409,1304,1304,1288,1288,1664,1637,1395,1395,1335,1335,1622,1636,1394,1394,1319,1319,1606,1621,1392,1392,1137,1137,1137,1137,345,390,360,375,404,373,1047,-2751,-2767,-2783,1062,1121,1046,-2799,1077,-2815,1106,1061,789,789,1105,1104,263,355,310,340,325,354,352,262,339,324,1091,1076,1029,1090,1060,1075,833,833,788,788,1088,1028,818,818,803,803,561,561,531,531,816,771,546,546,289,274,288,258,
        -253,-317,-381,-446,-478,-509,1279,1279,-811,-1179,-1451,-1756,-1900,-2028,-2189,-2253,-2333,-2414,-2445,-2511,-2526,1313,1298,-2559,1041,1041,1040,1040,1025,1025,1024,1024,1022,1007,1021,991,1020,975,1019,959,687,687,1018,1017,671,671,655,655,1016,1015,639,639,758,758,623,623,757,607,756,591,755,575,754,559,543,543,1009,783,-575,-621,-685,-749,496,-590,750,749,734,748,974,989,1003,958,988,973,1002,942,987,957,972,1001,926,986,941,971,956,1000,910,985,925,999,894,970,-1071,-1087,-1102,1390,-1135,1436,1509,1451,1374,-1151,1405,1358,1480,1420,-1167,1507,1494,1389,1342,1465,1435,1450,1326,1505,1310,1493,1373,1479,1404,1492,1464,1419,428,443,472,397,736,526,464,464,486,457,442,471,484,482,1357,1449,1434,1478,1388,1491,1341,1490,1325,1489,1463,1403,1309,1477,1372,1448,1418,1433,1476,1356,1462,1387,-1439,1475,1340,1447,1402,1474,1324,1461,1371,1473,269,448,1432,1417,1308,1460,-1711,1459,-1727,1441,1099,1099,1446,1386,1431,1401,-1743,1289,1083,1083,1160,1160,1458,1445,1067,1067,1370,1457,1307,1430,1129,1129,1098,1098,268,432,267,416,266,400,-1887,1144,1187,1082,1173,1113,1186,1066,1050,1158,1128,1143,1172,1097,1171,1081,420,391,1157,1112,1170,1142,1127,1065,1169,1049,1156,1096,1141,1111,1155,1080,1126,1154,1064,1153,1140,1095,1048,-2159,1125,1110,1137,-2175,823,823,1139,1138,807,807,384,264,368,263,868,838,853,791,867,822,852,837,866,806,865,790,-2319,851,821,836,352,262,850,805,849,-2399,533,533,835,820,336,261,578,548,563,577,532,532,832,772,562,562,547,547,305,275,560,515,290,290,288,258 };
    static const drmp3_uint8 tab32[] = { 130,162,193,209,44,28,76,140,9,9,9,9,9,9,9,9,190,254,222,238,126,94,157,157,109,61,173,205};
    static const drmp3_uint8 tab33[] = { 252,236,220,204,188,172,156,140,124,108,92,76,60,44,28,12 };
    static const drmp3_int16 tabindex[2*16] = { 0,32,64,98,0,132,180,218,292,364,426,538,648,746,0,1126,1460,1460,1460,1460,1460,1460,1460,1460,1842,1842,1842,1842,1842,1842,1842,1842 };
    static const drmp3_uint8 g_linbits[] =  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,6,8,10,13,4,5,6,7,8,9,11,13 };

#define DRMP3_PEEK_BITS(n)    (bs_cache >> (32 - n))
#define DRMP3_FLUSH_BITS(n)   { bs_cache <<= (n); bs_sh += (n); }
#define DRMP3_CHECK_BITS      while (bs_sh >= 0) { bs_cache |= (drmp3_uint32)*bs_next_ptr++ << bs_sh; bs_sh -= 8; }
#define DRMP3_BSPOS           ((bs_next_ptr - bs->buf)*8 - 24 + bs_sh)

    float one = 0.0f;
    int ireg = 0, big_val_cnt = gr_info->big_values;
    const drmp3_uint8 *sfb = gr_info->sfbtab;
    const drmp3_uint8 *bs_next_ptr = bs->buf + bs->pos/8;
    drmp3_uint32 bs_cache = (((bs_next_ptr[0]*256u + bs_next_ptr[1])*256u + bs_next_ptr[2])*256u + bs_next_ptr[3]) << (bs->pos & 7);
    int pairs_to_decode, np, bs_sh = (bs->pos & 7) - 8;
    bs_next_ptr += 4;

    while (big_val_cnt > 0)
    {
        int tab_num = gr_info->table_select[ireg];
        int sfb_cnt = gr_info->region_count[ireg++];
        const drmp3_int16 *codebook = tabs + tabindex[tab_num];
        int linbits = g_linbits[tab_num];
        do
        {
            np = *sfb++ / 2;
            pairs_to_decode = DRMP3_MIN(big_val_cnt, np);
            one = *scf++;
            do
            {
                int j, w = 5;
                int leaf = codebook[DRMP3_PEEK_BITS(w)];
                while (leaf < 0)
                {
                    DRMP3_FLUSH_BITS(w);
                    w = leaf & 7;
                    leaf = codebook[DRMP3_PEEK_BITS(w) - (leaf >> 3)];
                }
                DRMP3_FLUSH_BITS(leaf >> 8);

                for (j = 0; j < 2; j++, dst++, leaf >>= 4)
                {
                    int lsb = leaf & 0x0F;
                    if (lsb == 15 && linbits)
                    {
                        lsb += DRMP3_PEEK_BITS(linbits);
                        DRMP3_FLUSH_BITS(linbits);
                        DRMP3_CHECK_BITS;
                        *dst = one*drmp3_L3_pow_43(lsb)*((int32_t)bs_cache < 0 ? -1: 1);
                    } else
                    {
                        *dst = g_drmp3_pow43[16 + lsb - 16*(bs_cache >> 31)]*one;
                    }
                    DRMP3_FLUSH_BITS(lsb ? 1 : 0);
                }
                DRMP3_CHECK_BITS;
            } while (--pairs_to_decode);
        } while ((big_val_cnt -= np) > 0 && --sfb_cnt >= 0);
    }

    for (np = 1 - big_val_cnt;; dst += 4)
    {
        const drmp3_uint8 *codebook_count1 = (gr_info->count1_table) ? tab33 : tab32;
        int leaf = codebook_count1[DRMP3_PEEK_BITS(4)];
        if (!(leaf & 8))
        {
            leaf = codebook_count1[(leaf >> 3) + (bs_cache << 4 >> (32 - (leaf & 3)))];
        }
        DRMP3_FLUSH_BITS(leaf & 7);
        if (DRMP3_BSPOS > layer3gr_limit)
        {
            break;
        }
#define DRMP3_RELOAD_SCALEFACTOR  if (!--np) { np = *sfb++/2; if (!np) break; one = *scf++; }
#define DRMP3_DEQ_COUNT1(s) if (leaf & (128 >> s)) { dst[s] = ((drmp3_int32)bs_cache < 0) ? -one : one; DRMP3_FLUSH_BITS(1) }
        DRMP3_RELOAD_SCALEFACTOR;
        DRMP3_DEQ_COUNT1(0);
        DRMP3_DEQ_COUNT1(1);
        DRMP3_RELOAD_SCALEFACTOR;
        DRMP3_DEQ_COUNT1(2);
        DRMP3_DEQ_COUNT1(3);
        DRMP3_CHECK_BITS;
    }

    bs->pos = layer3gr_limit;
}

static void drmp3_L3_midside_stereo(float *left, int n)
{
    int i = 0;
    float *right = left + 576;
#if DRMP3_HAVE_SIMD
    if (drmp3_have_simd()) for (; i < n - 3; i += 4)
    {
        drmp3_f4 vl = DRMP3_VLD(left + i);
        drmp3_f4 vr = DRMP3_VLD(right + i);
        DRMP3_VSTORE(left + i, DRMP3_VADD(vl, vr));
        DRMP3_VSTORE(right + i, DRMP3_VSUB(vl, vr));
    }
#endif
    for (; i < n; i++)
    {
        float a = left[i];
        float b = right[i];
        left[i] = a + b;
        right[i] = a - b;
    }
}

static void drmp3_L3_intensity_stereo_band(float *left, int n, float kl, float kr)
{
    int i;
    for (i = 0; i < n; i++)
    {
        left[i + 576] = left[i]*kr;
        left[i] = left[i]*kl;
    }
}

static void drmp3_L3_stereo_top_band(const float *right, const drmp3_uint8 *sfb, int nbands, int max_band[3])
{
    int i, k;

    max_band[0] = max_band[1] = max_band[2] = -1;

    for (i = 0; i < nbands; i++)
    {
        for (k = 0; k < sfb[i]; k += 2)
        {
            if (right[k] != 0 || right[k + 1] != 0)
            {
                max_band[i % 3] = i;
                break;
            }
        }
        right += sfb[i];
    }
}

static void drmp3_L3_stereo_process(float *left, const drmp3_uint8 *ist_pos, const drmp3_uint8 *sfb, const drmp3_uint8 *hdr, int max_band[3], int mpeg2_sh)
{
    static const float g_pan[7*2] = { 0,1,0.21132487f,0.78867513f,0.36602540f,0.63397460f,0.5f,0.5f,0.63397460f,0.36602540f,0.78867513f,0.21132487f,1,0 };
    unsigned i, max_pos = DRMP3_HDR_TEST_MPEG1(hdr) ? 7 : 64;

    for (i = 0; sfb[i]; i++)
    {
        unsigned ipos = ist_pos[i];
        if ((int)i > max_band[i % 3] && ipos < max_pos)
        {
            float kl, kr, s = DRMP3_HDR_TEST_MS_STEREO(hdr) ? 1.41421356f : 1;
            if (DRMP3_HDR_TEST_MPEG1(hdr))
            {
                kl = g_pan[2*ipos];
                kr = g_pan[2*ipos + 1];
            } else
            {
                kl = 1;
                kr = drmp3_L3_ldexp_q2(1, (ipos + 1) >> 1 << mpeg2_sh);
                if (ipos & 1)
                {
                    kl = kr;
                    kr = 1;
                }
            }
            drmp3_L3_intensity_stereo_band(left, sfb[i], kl*s, kr*s);
        } else if (DRMP3_HDR_TEST_MS_STEREO(hdr))
        {
            drmp3_L3_midside_stereo(left, sfb[i]);
        }
        left += sfb[i];
    }
}

static void drmp3_L3_intensity_stereo(float *left, drmp3_uint8 *ist_pos, const drmp3_L3_gr_info *gr, const drmp3_uint8 *hdr)
{
    int max_band[3], n_sfb = gr->n_long_sfb + gr->n_short_sfb;
    int i, max_blocks = gr->n_short_sfb ? 3 : 1;

    drmp3_L3_stereo_top_band(left + 576, gr->sfbtab, n_sfb, max_band);
    if (gr->n_long_sfb)
    {
        max_band[0] = max_band[1] = max_band[2] = DRMP3_MAX(DRMP3_MAX(max_band[0], max_band[1]), max_band[2]);
    }
    for (i = 0; i < max_blocks; i++)
    {
        int default_pos = DRMP3_HDR_TEST_MPEG1(hdr) ? 3 : 0;
        int itop = n_sfb - max_blocks + i;
        int prev = itop - max_blocks;
        ist_pos[itop] = (drmp3_uint8)(max_band[i] >= prev ? default_pos : ist_pos[prev]);
    }
    drmp3_L3_stereo_process(left, ist_pos, gr->sfbtab, hdr, max_band, gr[1].scalefac_compress & 1);
}

static void drmp3_L3_reorder(float *grbuf, float *scratch, const drmp3_uint8 *sfb)
{
    int i, len;
    float *src = grbuf, *dst = scratch;

    for (;0 != (len = *sfb); sfb += 3, src += 2*len)
    {
        for (i = 0; i < len; i++, src++)
        {
            *dst++ = src[0*len];
            *dst++ = src[1*len];
            *dst++ = src[2*len];
        }
    }
    memcpy(grbuf, scratch, (dst - scratch)*sizeof(float));
}

static void drmp3_L3_antialias(float *grbuf, int nbands)
{
    static const float g_aa[2][8] = {
        {0.85749293f,0.88174200f,0.94962865f,0.98331459f,0.99551782f,0.99916056f,0.99989920f,0.99999316f},
        {0.51449576f,0.47173197f,0.31337745f,0.18191320f,0.09457419f,0.04096558f,0.01419856f,0.00369997f}
    };

    for (; nbands > 0; nbands--, grbuf += 18)
    {
        int i = 0;
#if DRMP3_HAVE_SIMD
        if (drmp3_have_simd()) for (; i < 8; i += 4)
        {
            drmp3_f4 vu = DRMP3_VLD(grbuf + 18 + i);
            drmp3_f4 vd = DRMP3_VLD(grbuf + 14 - i);
            drmp3_f4 vc0 = DRMP3_VLD(g_aa[0] + i);
            drmp3_f4 vc1 = DRMP3_VLD(g_aa[1] + i);
            vd = DRMP3_VREV(vd);
            DRMP3_VSTORE(grbuf + 18 + i, DRMP3_VSUB(DRMP3_VMUL(vu, vc0), DRMP3_VMUL(vd, vc1)));
            vd = DRMP3_VADD(DRMP3_VMUL(vu, vc1), DRMP3_VMUL(vd, vc0));
            DRMP3_VSTORE(grbuf + 14 - i, DRMP3_VREV(vd));
        }
#endif
#ifndef DR_MP3_ONLY_SIMD
        for(; i < 8; i++)
        {
            float u = grbuf[18 + i];
            float d = grbuf[17 - i];
            grbuf[18 + i] = u*g_aa[0][i] - d*g_aa[1][i];
            grbuf[17 - i] = u*g_aa[1][i] + d*g_aa[0][i];
        }
#endif
    }
}

static void drmp3_L3_dct3_9(float *y)
{
    float s0, s1, s2, s3, s4, s5, s6, s7, s8, t0, t2, t4;

    s0 = y[0]; s2 = y[2]; s4 = y[4]; s6 = y[6]; s8 = y[8];
    t0 = s0 + s6*0.5f;
    s0 -= s6;
    t4 = (s4 + s2)*0.93969262f;
    t2 = (s8 + s2)*0.76604444f;
    s6 = (s4 - s8)*0.17364818f;
    s4 += s8 - s2;

    s2 = s0 - s4*0.5f;
    y[4] = s4 + s0;
    s8 = t0 - t2 + s6;
    s0 = t0 - t4 + t2;
    s4 = t0 + t4 - s6;

    s1 = y[1]; s3 = y[3]; s5 = y[5]; s7 = y[7];

    s3 *= 0.86602540f;
    t0 = (s5 + s1)*0.98480775f;
    t4 = (s5 - s7)*0.34202014f;
    t2 = (s1 + s7)*0.64278761f;
    s1 = (s1 - s5 - s7)*0.86602540f;

    s5 = t0 - s3 - t2;
    s7 = t4 - s3 - t0;
    s3 = t4 + s3 - t2;

    y[0] = s4 - s7;
    y[1] = s2 + s1;
    y[2] = s0 - s3;
    y[3] = s8 + s5;
    y[5] = s8 - s5;
    y[6] = s0 + s3;
    y[7] = s2 - s1;
    y[8] = s4 + s7;
}

static void drmp3_L3_imdct36(float *grbuf, float *overlap, const float *window, int nbands)
{
    int i, j;
    static const float g_twid9[18] = {
        0.73727734f,0.79335334f,0.84339145f,0.88701083f,0.92387953f,0.95371695f,0.97629601f,0.99144486f,0.99904822f,0.67559021f,0.60876143f,0.53729961f,0.46174861f,0.38268343f,0.30070580f,0.21643961f,0.13052619f,0.04361938f
    };

    for (j = 0; j < nbands; j++, grbuf += 18, overlap += 9)
    {
        float co[9], si[9];
        co[0] = -grbuf[0];
        si[0] = grbuf[17];
        for (i = 0; i < 4; i++)
        {
            si[8 - 2*i] =   grbuf[4*i + 1] - grbuf[4*i + 2];
            co[1 + 2*i] =   grbuf[4*i + 1] + grbuf[4*i + 2];
            si[7 - 2*i] =   grbuf[4*i + 4] - grbuf[4*i + 3];
            co[2 + 2*i] = -(grbuf[4*i + 3] + grbuf[4*i + 4]);
        }
        drmp3_L3_dct3_9(co);
        drmp3_L3_dct3_9(si);

        si[1] = -si[1];
        si[3] = -si[3];
        si[5] = -si[5];
        si[7] = -si[7];

        i = 0;

#if DRMP3_HAVE_SIMD
        if (drmp3_have_simd()) for (; i < 8; i += 4)
        {
            drmp3_f4 vovl = DRMP3_VLD(overlap + i);
            drmp3_f4 vc = DRMP3_VLD(co + i);
            drmp3_f4 vs = DRMP3_VLD(si + i);
            drmp3_f4 vr0 = DRMP3_VLD(g_twid9 + i);
            drmp3_f4 vr1 = DRMP3_VLD(g_twid9 + 9 + i);
            drmp3_f4 vw0 = DRMP3_VLD(window + i);
            drmp3_f4 vw1 = DRMP3_VLD(window + 9 + i);
            drmp3_f4 vsum = DRMP3_VADD(DRMP3_VMUL(vc, vr1), DRMP3_VMUL(vs, vr0));
            DRMP3_VSTORE(overlap + i, DRMP3_VSUB(DRMP3_VMUL(vc, vr0), DRMP3_VMUL(vs, vr1)));
            DRMP3_VSTORE(grbuf + i, DRMP3_VSUB(DRMP3_VMUL(vovl, vw0), DRMP3_VMUL(vsum, vw1)));
            vsum = DRMP3_VADD(DRMP3_VMUL(vovl, vw1), DRMP3_VMUL(vsum, vw0));
            DRMP3_VSTORE(grbuf + 14 - i, DRMP3_VREV(vsum));
        }
#endif
        for (; i < 9; i++)
        {
            float ovl  = overlap[i];
            float sum  = co[i]*g_twid9[9 + i] + si[i]*g_twid9[0 + i];
            overlap[i] = co[i]*g_twid9[0 + i] - si[i]*g_twid9[9 + i];
            grbuf[i]      = ovl*window[0 + i] - sum*window[9 + i];
            grbuf[17 - i] = ovl*window[9 + i] + sum*window[0 + i];
        }
    }
}

static void drmp3_L3_idct3(float x0, float x1, float x2, float *dst)
{
    float m1 = x1*0.86602540f;
    float a1 = x0 - x2*0.5f;
    dst[1] = x0 + x2;
    dst[0] = a1 + m1;
    dst[2] = a1 - m1;
}

static void drmp3_L3_imdct12(float *x, float *dst, float *overlap)
{
    static const float g_twid3[6] = { 0.79335334f,0.92387953f,0.99144486f, 0.60876143f,0.38268343f,0.13052619f };
    float co[3], si[3];
    int i;

    drmp3_L3_idct3(-x[0], x[6] + x[3], x[12] + x[9], co);
    drmp3_L3_idct3(x[15], x[12] - x[9], x[6] - x[3], si);
    si[1] = -si[1];

    for (i = 0; i < 3; i++)
    {
        float ovl  = overlap[i];
        float sum  = co[i]*g_twid3[3 + i] + si[i]*g_twid3[0 + i];
        overlap[i] = co[i]*g_twid3[0 + i] - si[i]*g_twid3[3 + i];
        dst[i]     = ovl*g_twid3[2 - i] - sum*g_twid3[5 - i];
        dst[5 - i] = ovl*g_twid3[5 - i] + sum*g_twid3[2 - i];
    }
}

static void drmp3_L3_imdct_short(float *grbuf, float *overlap, int nbands)
{
    for (;nbands > 0; nbands--, overlap += 9, grbuf += 18)
    {
        float tmp[18];
        memcpy(tmp, grbuf, sizeof(tmp));
        memcpy(grbuf, overlap, 6*sizeof(float));
        drmp3_L3_imdct12(tmp, grbuf + 6, overlap + 6);
        drmp3_L3_imdct12(tmp + 1, grbuf + 12, overlap + 6);
        drmp3_L3_imdct12(tmp + 2, overlap, overlap + 6);
    }
}

static void drmp3_L3_change_sign(float *grbuf)
{
    int b, i;
    for (b = 0, grbuf += 18; b < 32; b += 2, grbuf += 36)
        for (i = 1; i < 18; i += 2)
            grbuf[i] = -grbuf[i];
}

static void drmp3_L3_imdct_gr(float *grbuf, float *overlap, unsigned block_type, unsigned n_long_bands)
{
    static const float g_mdct_window[2][18] = {
        { 0.99904822f,0.99144486f,0.97629601f,0.95371695f,0.92387953f,0.88701083f,0.84339145f,0.79335334f,0.73727734f,0.04361938f,0.13052619f,0.21643961f,0.30070580f,0.38268343f,0.46174861f,0.53729961f,0.60876143f,0.67559021f },
        { 1,1,1,1,1,1,0.99144486f,0.92387953f,0.79335334f,0,0,0,0,0,0,0.13052619f,0.38268343f,0.60876143f }
    };
    if (n_long_bands)
    {
        drmp3_L3_imdct36(grbuf, overlap, g_mdct_window[0], n_long_bands);
        grbuf += 18*n_long_bands;
        overlap += 9*n_long_bands;
    }
    if (block_type == DRMP3_SHORT_BLOCK_TYPE)
        drmp3_L3_imdct_short(grbuf, overlap, 32 - n_long_bands);
    else
        drmp3_L3_imdct36(grbuf, overlap, g_mdct_window[block_type == DRMP3_STOP_BLOCK_TYPE], 32 - n_long_bands);
}

static void drmp3_L3_save_reservoir(drmp3dec *h, drmp3dec_scratch *s)
{
    int pos = (s->bs.pos + 7)/8u;
    int remains = s->bs.limit/8u - pos;
    if (remains > DRMP3_MAX_BITRESERVOIR_BYTES)
    {
        pos += remains - DRMP3_MAX_BITRESERVOIR_BYTES;
        remains = DRMP3_MAX_BITRESERVOIR_BYTES;
    }
    if (remains > 0)
    {
        memmove(h->reserv_buf, s->maindata + pos, remains);
    }
    h->reserv = remains;
}

static int drmp3_L3_restore_reservoir(drmp3dec *h, drmp3_bs *bs, drmp3dec_scratch *s, int main_data_begin)
{
    int frame_bytes = (bs->limit - bs->pos)/8;
    int bytes_have = DRMP3_MIN(h->reserv, main_data_begin);
    memcpy(s->maindata, h->reserv_buf + DRMP3_MAX(0, h->reserv - main_data_begin), DRMP3_MIN(h->reserv, main_data_begin));
    memcpy(s->maindata + bytes_have, bs->buf + bs->pos/8, frame_bytes);
    drmp3_bs_init(&s->bs, s->maindata, bytes_have + frame_bytes);
    return h->reserv >= main_data_begin;
}

static void drmp3_L3_decode(drmp3dec *h, drmp3dec_scratch *s, drmp3_L3_gr_info *gr_info, int nch)
{
    int ch;

    for (ch = 0; ch < nch; ch++)
    {
        int layer3gr_limit = s->bs.pos + gr_info[ch].part_23_length;
        drmp3_L3_decode_scalefactors(h->header, s->ist_pos[ch], &s->bs, gr_info + ch, s->scf, ch);
        drmp3_L3_huffman(s->grbuf[ch], &s->bs, gr_info + ch, s->scf, layer3gr_limit);
    }

    if (DRMP3_HDR_TEST_I_STEREO(h->header))
    {
        drmp3_L3_intensity_stereo(s->grbuf[0], s->ist_pos[1], gr_info, h->header);
    } else if (DRMP3_HDR_IS_MS_STEREO(h->header))
    {
        drmp3_L3_midside_stereo(s->grbuf[0], 576);
    }

    for (ch = 0; ch < nch; ch++, gr_info++)
    {
        int aa_bands = 31;
        int n_long_bands = (gr_info->mixed_block_flag ? 2 : 0) << (int)(DRMP3_HDR_GET_MY_SAMPLE_RATE(h->header) == 2);

        if (gr_info->n_short_sfb)
        {
            aa_bands = n_long_bands - 1;
            drmp3_L3_reorder(s->grbuf[ch] + n_long_bands*18, s->syn[0], gr_info->sfbtab + gr_info->n_long_sfb);
        }

        drmp3_L3_antialias(s->grbuf[ch], aa_bands);
        drmp3_L3_imdct_gr(s->grbuf[ch], h->mdct_overlap[ch], gr_info->block_type, n_long_bands);
        drmp3_L3_change_sign(s->grbuf[ch]);
    }
}

static void drmp3d_DCT_II(float *grbuf, int n)
{
    static const float g_sec[24] = {
        10.19000816f,0.50060302f,0.50241929f,3.40760851f,0.50547093f,0.52249861f,2.05778098f,0.51544732f,0.56694406f,1.48416460f,0.53104258f,0.64682180f,1.16943991f,0.55310392f,0.78815460f,0.97256821f,0.58293498f,1.06067765f,0.83934963f,0.62250412f,1.72244716f,0.74453628f,0.67480832f,5.10114861f
    };
    int i, k = 0;
#if DRMP3_HAVE_SIMD
    if (drmp3_have_simd()) for (; k < n; k += 4)
    {
        drmp3_f4 t[4][8], *x;
        float *y = grbuf + k;

        for (x = t[0], i = 0; i < 8; i++, x++)
        {
            drmp3_f4 x0 = DRMP3_VLD(&y[i*18]);
            drmp3_f4 x1 = DRMP3_VLD(&y[(15 - i)*18]);
            drmp3_f4 x2 = DRMP3_VLD(&y[(16 + i)*18]);
            drmp3_f4 x3 = DRMP3_VLD(&y[(31 - i)*18]);
            drmp3_f4 t0 = DRMP3_VADD(x0, x3);
            drmp3_f4 t1 = DRMP3_VADD(x1, x2);
            drmp3_f4 t2 = DRMP3_VMUL_S(DRMP3_VSUB(x1, x2), g_sec[3*i + 0]);
            drmp3_f4 t3 = DRMP3_VMUL_S(DRMP3_VSUB(x0, x3), g_sec[3*i + 1]);
            x[0] = DRMP3_VADD(t0, t1);
            x[8] = DRMP3_VMUL_S(DRMP3_VSUB(t0, t1), g_sec[3*i + 2]);
            x[16] = DRMP3_VADD(t3, t2);
            x[24] = DRMP3_VMUL_S(DRMP3_VSUB(t3, t2), g_sec[3*i + 2]);
        }
        for (x = t[0], i = 0; i < 4; i++, x += 8)
        {
            drmp3_f4 x0 = x[0], x1 = x[1], x2 = x[2], x3 = x[3], x4 = x[4], x5 = x[5], x6 = x[6], x7 = x[7], xt;
            xt = DRMP3_VSUB(x0, x7); x0 = DRMP3_VADD(x0, x7);
            x7 = DRMP3_VSUB(x1, x6); x1 = DRMP3_VADD(x1, x6);
            x6 = DRMP3_VSUB(x2, x5); x2 = DRMP3_VADD(x2, x5);
            x5 = DRMP3_VSUB(x3, x4); x3 = DRMP3_VADD(x3, x4);
            x4 = DRMP3_VSUB(x0, x3); x0 = DRMP3_VADD(x0, x3);
            x3 = DRMP3_VSUB(x1, x2); x1 = DRMP3_VADD(x1, x2);
            x[0] = DRMP3_VADD(x0, x1);
            x[4] = DRMP3_VMUL_S(DRMP3_VSUB(x0, x1), 0.70710677f);
            x5 = DRMP3_VADD(x5, x6);
            x6 = DRMP3_VMUL_S(DRMP3_VADD(x6, x7), 0.70710677f);
            x7 = DRMP3_VADD(x7, xt);
            x3 = DRMP3_VMUL_S(DRMP3_VADD(x3, x4), 0.70710677f);
            x5 = DRMP3_VSUB(x5, DRMP3_VMUL_S(x7, 0.198912367f)); /* rotate by PI/8 */
            x7 = DRMP3_VADD(x7, DRMP3_VMUL_S(x5, 0.382683432f));
            x5 = DRMP3_VSUB(x5, DRMP3_VMUL_S(x7, 0.198912367f));
            x0 = DRMP3_VSUB(xt, x6); xt = DRMP3_VADD(xt, x6);
            x[1] = DRMP3_VMUL_S(DRMP3_VADD(xt, x7), 0.50979561f);
            x[2] = DRMP3_VMUL_S(DRMP3_VADD(x4, x3), 0.54119611f);
            x[3] = DRMP3_VMUL_S(DRMP3_VSUB(x0, x5), 0.60134488f);
            x[5] = DRMP3_VMUL_S(DRMP3_VADD(x0, x5), 0.89997619f);
            x[6] = DRMP3_VMUL_S(DRMP3_VSUB(x4, x3), 1.30656302f);
            x[7] = DRMP3_VMUL_S(DRMP3_VSUB(xt, x7), 2.56291556f);
        }

        if (k > n - 3)
        {
#if DRMP3_HAVE_SSE
#define DRMP3_VSAVE2(i, v) _mm_storel_pi((__m64 *)(void*)&y[i*18], v)
#else
#define DRMP3_VSAVE2(i, v) vst1_f32((float32_t *)&y[i*18],  vget_low_f32(v))
#endif
            for (i = 0; i < 7; i++, y += 4*18)
            {
                drmp3_f4 s = DRMP3_VADD(t[3][i], t[3][i + 1]);
                DRMP3_VSAVE2(0, t[0][i]);
                DRMP3_VSAVE2(1, DRMP3_VADD(t[2][i], s));
                DRMP3_VSAVE2(2, DRMP3_VADD(t[1][i], t[1][i + 1]));
                DRMP3_VSAVE2(3, DRMP3_VADD(t[2][1 + i], s));
            }
            DRMP3_VSAVE2(0, t[0][7]);
            DRMP3_VSAVE2(1, DRMP3_VADD(t[2][7], t[3][7]));
            DRMP3_VSAVE2(2, t[1][7]);
            DRMP3_VSAVE2(3, t[3][7]);
        } else
        {
#define DRMP3_VSAVE4(i, v) DRMP3_VSTORE(&y[i*18], v)
            for (i = 0; i < 7; i++, y += 4*18)
            {
                drmp3_f4 s = DRMP3_VADD(t[3][i], t[3][i + 1]);
                DRMP3_VSAVE4(0, t[0][i]);
                DRMP3_VSAVE4(1, DRMP3_VADD(t[2][i], s));
                DRMP3_VSAVE4(2, DRMP3_VADD(t[1][i], t[1][i + 1]));
                DRMP3_VSAVE4(3, DRMP3_VADD(t[2][1 + i], s));
            }
            DRMP3_VSAVE4(0, t[0][7]);
            DRMP3_VSAVE4(1, DRMP3_VADD(t[2][7], t[3][7]));
            DRMP3_VSAVE4(2, t[1][7]);
            DRMP3_VSAVE4(3, t[3][7]);
        }
    } else
#endif
#ifdef DR_MP3_ONLY_SIMD
    {}
#else
    for (; k < n; k++)
    {
        float t[4][8], *x, *y = grbuf + k;

        for (x = t[0], i = 0; i < 8; i++, x++)
        {
            float x0 = y[i*18];
            float x1 = y[(15 - i)*18];
            float x2 = y[(16 + i)*18];
            float x3 = y[(31 - i)*18];
            float t0 = x0 + x3;
            float t1 = x1 + x2;
            float t2 = (x1 - x2)*g_sec[3*i + 0];
            float t3 = (x0 - x3)*g_sec[3*i + 1];
            x[0] = t0 + t1;
            x[8] = (t0 - t1)*g_sec[3*i + 2];
            x[16] = t3 + t2;
            x[24] = (t3 - t2)*g_sec[3*i + 2];
        }
        for (x = t[0], i = 0; i < 4; i++, x += 8)
        {
            float x0 = x[0], x1 = x[1], x2 = x[2], x3 = x[3], x4 = x[4], x5 = x[5], x6 = x[6], x7 = x[7], xt;
            xt = x0 - x7; x0 += x7;
            x7 = x1 - x6; x1 += x6;
            x6 = x2 - x5; x2 += x5;
            x5 = x3 - x4; x3 += x4;
            x4 = x0 - x3; x0 += x3;
            x3 = x1 - x2; x1 += x2;
            x[0] = x0 + x1;
            x[4] = (x0 - x1)*0.70710677f;
            x5 =  x5 + x6;
            x6 = (x6 + x7)*0.70710677f;
            x7 =  x7 + xt;
            x3 = (x3 + x4)*0.70710677f;
            x5 -= x7*0.198912367f;  /* rotate by PI/8 */
            x7 += x5*0.382683432f;
            x5 -= x7*0.198912367f;
            x0 = xt - x6; xt += x6;
            x[1] = (xt + x7)*0.50979561f;
            x[2] = (x4 + x3)*0.54119611f;
            x[3] = (x0 - x5)*0.60134488f;
            x[5] = (x0 + x5)*0.89997619f;
            x[6] = (x4 - x3)*1.30656302f;
            x[7] = (xt - x7)*2.56291556f;

        }
        for (i = 0; i < 7; i++, y += 4*18)
        {
            y[0*18] = t[0][i];
            y[1*18] = t[2][i] + t[3][i] + t[3][i + 1];
            y[2*18] = t[1][i] + t[1][i + 1];
            y[3*18] = t[2][i + 1] + t[3][i] + t[3][i + 1];
        }
        y[0*18] = t[0][7];
        y[1*18] = t[2][7] + t[3][7];
        y[2*18] = t[1][7];
        y[3*18] = t[3][7];
    }
#endif
}

#ifndef DR_MP3_FLOAT_OUTPUT
typedef drmp3_int16 drmp3d_sample_t;

static drmp3_int16 drmp3d_scale_pcm(float sample)
{
    if (sample >=  32766.5) return (drmp3_int16) 32767;
    if (sample <= -32767.5) return (drmp3_int16)-32768;
    drmp3_int16 s = (drmp3_int16)(sample + .5f);
    s -= (s < 0);   /* away from zero, to be compliant */
    return (drmp3_int16)s;
}
#else
typedef float drmp3d_sample_t;

static float drmp3d_scale_pcm(float sample)
{
    return sample*(1.f/32768.f);
}
#endif

static void drmp3d_synth_pair(drmp3d_sample_t *pcm, int nch, const float *z)
{
    float a;
    a  = (z[14*64] - z[    0]) * 29;
    a += (z[ 1*64] + z[13*64]) * 213;
    a += (z[12*64] - z[ 2*64]) * 459;
    a += (z[ 3*64] + z[11*64]) * 2037;
    a += (z[10*64] - z[ 4*64]) * 5153;
    a += (z[ 5*64] + z[ 9*64]) * 6574;
    a += (z[ 8*64] - z[ 6*64]) * 37489;
    a +=  z[ 7*64]             * 75038;
    pcm[0] = drmp3d_scale_pcm(a);

    z += 2;
    a  = z[14*64] * 104;
    a += z[12*64] * 1567;
    a += z[10*64] * 9727;
    a += z[ 8*64] * 64019;
    a += z[ 6*64] * -9975;
    a += z[ 4*64] * -45;
    a += z[ 2*64] * 146;
    a += z[ 0*64] * -5;
    pcm[16*nch] = drmp3d_scale_pcm(a);
}

static void drmp3d_synth(float *xl, drmp3d_sample_t *dstl, int nch, float *lins)
{
    int i;
    float *xr = xl + 576*(nch - 1);
    drmp3d_sample_t *dstr = dstl + (nch - 1);

    static const float g_win[] = {
        -1,26,-31,208,218,401,-519,2063,2000,4788,-5517,7134,5959,35640,-39336,74992,
        -1,24,-35,202,222,347,-581,2080,1952,4425,-5879,7640,5288,33791,-41176,74856,
        -1,21,-38,196,225,294,-645,2087,1893,4063,-6237,8092,4561,31947,-43006,74630,
        -1,19,-41,190,227,244,-711,2085,1822,3705,-6589,8492,3776,30112,-44821,74313,
        -1,17,-45,183,228,197,-779,2075,1739,3351,-6935,8840,2935,28289,-46617,73908,
        -1,16,-49,176,228,153,-848,2057,1644,3004,-7271,9139,2037,26482,-48390,73415,
        -2,14,-53,169,227,111,-919,2032,1535,2663,-7597,9389,1082,24694,-50137,72835,
        -2,13,-58,161,224,72,-991,2001,1414,2330,-7910,9592,70,22929,-51853,72169,
        -2,11,-63,154,221,36,-1064,1962,1280,2006,-8209,9750,-998,21189,-53534,71420,
        -2,10,-68,147,215,2,-1137,1919,1131,1692,-8491,9863,-2122,19478,-55178,70590,
        -3,9,-73,139,208,-29,-1210,1870,970,1388,-8755,9935,-3300,17799,-56778,69679,
        -3,8,-79,132,200,-57,-1283,1817,794,1095,-8998,9966,-4533,16155,-58333,68692,
        -4,7,-85,125,189,-83,-1356,1759,605,814,-9219,9959,-5818,14548,-59838,67629,
        -4,7,-91,117,177,-106,-1428,1698,402,545,-9416,9916,-7154,12980,-61289,66494,
        -5,6,-97,111,163,-127,-1498,1634,185,288,-9585,9838,-8540,11455,-62684,65290
    };
    float *zlin = lins + 15*64;
    const float *w = g_win;

    zlin[4*15]     = xl[18*16];
    zlin[4*15 + 1] = xr[18*16];
    zlin[4*15 + 2] = xl[0];
    zlin[4*15 + 3] = xr[0];

    zlin[4*31]     = xl[1 + 18*16];
    zlin[4*31 + 1] = xr[1 + 18*16];
    zlin[4*31 + 2] = xl[1];
    zlin[4*31 + 3] = xr[1];

    drmp3d_synth_pair(dstr, nch, lins + 4*15 + 1);
    drmp3d_synth_pair(dstr + 32*nch, nch, lins + 4*15 + 64 + 1);
    drmp3d_synth_pair(dstl, nch, lins + 4*15);
    drmp3d_synth_pair(dstl + 32*nch, nch, lins + 4*15 + 64);

#if DRMP3_HAVE_SIMD
    if (drmp3_have_simd()) for (i = 14; i >= 0; i--)
    {
#define DRMP3_VLOAD(k) drmp3_f4 w0 = DRMP3_VSET(*w++); drmp3_f4 w1 = DRMP3_VSET(*w++); drmp3_f4 vz = DRMP3_VLD(&zlin[4*i - 64*k]); drmp3_f4 vy = DRMP3_VLD(&zlin[4*i - 64*(15 - k)]);
#define DRMP3_V0(k) { DRMP3_VLOAD(k) b =               DRMP3_VADD(DRMP3_VMUL(vz, w1), DRMP3_VMUL(vy, w0)) ; a =               DRMP3_VSUB(DRMP3_VMUL(vz, w0), DRMP3_VMUL(vy, w1));  }
#define DRMP3_V1(k) { DRMP3_VLOAD(k) b = DRMP3_VADD(b, DRMP3_VADD(DRMP3_VMUL(vz, w1), DRMP3_VMUL(vy, w0))); a = DRMP3_VADD(a, DRMP3_VSUB(DRMP3_VMUL(vz, w0), DRMP3_VMUL(vy, w1))); }
#define DRMP3_V2(k) { DRMP3_VLOAD(k) b = DRMP3_VADD(b, DRMP3_VADD(DRMP3_VMUL(vz, w1), DRMP3_VMUL(vy, w0))); a = DRMP3_VADD(a, DRMP3_VSUB(DRMP3_VMUL(vy, w1), DRMP3_VMUL(vz, w0))); }
        drmp3_f4 a, b;
        zlin[4*i]     = xl[18*(31 - i)];
        zlin[4*i + 1] = xr[18*(31 - i)];
        zlin[4*i + 2] = xl[1 + 18*(31 - i)];
        zlin[4*i + 3] = xr[1 + 18*(31 - i)];
        zlin[4*i + 64] = xl[1 + 18*(1 + i)];
        zlin[4*i + 64 + 1] = xr[1 + 18*(1 + i)];
        zlin[4*i - 64 + 2] = xl[18*(1 + i)];
        zlin[4*i - 64 + 3] = xr[18*(1 + i)];

        DRMP3_V0(0) DRMP3_V2(1) DRMP3_V1(2) DRMP3_V2(3) DRMP3_V1(4) DRMP3_V2(5) DRMP3_V1(6) DRMP3_V2(7)

        {
#ifndef DR_MP3_FLOAT_OUTPUT
#if DRMP3_HAVE_SSE
            static const drmp3_f4 g_max = { 32767.0f, 32767.0f, 32767.0f, 32767.0f };
            static const drmp3_f4 g_min = { -32768.0f, -32768.0f, -32768.0f, -32768.0f };
            __m128i pcm8 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_max_ps(_mm_min_ps(a, g_max), g_min)),
                                           _mm_cvtps_epi32(_mm_max_ps(_mm_min_ps(b, g_max), g_min)));
            dstr[(15 - i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 1);
            dstr[(17 + i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 5);
            dstl[(15 - i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 0);
            dstl[(17 + i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 4);
            dstr[(47 - i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 3);
            dstr[(49 + i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 7);
            dstl[(47 - i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 2);
            dstl[(49 + i)*nch] = (drmp3_int16)_mm_extract_epi16(pcm8, 6);
#else
            int16x4_t pcma, pcmb;
            a = DRMP3_VADD(a, DRMP3_VSET(0.5f));
            b = DRMP3_VADD(b, DRMP3_VSET(0.5f));
            pcma = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(a), vreinterpretq_s32_u32(vcltq_f32(a, DRMP3_VSET(0)))));
            pcmb = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(b), vreinterpretq_s32_u32(vcltq_f32(b, DRMP3_VSET(0)))));
            vst1_lane_s16(dstr + (15 - i)*nch, pcma, 1);
            vst1_lane_s16(dstr + (17 + i)*nch, pcmb, 1);
            vst1_lane_s16(dstl + (15 - i)*nch, pcma, 0);
            vst1_lane_s16(dstl + (17 + i)*nch, pcmb, 0);
            vst1_lane_s16(dstr + (47 - i)*nch, pcma, 3);
            vst1_lane_s16(dstr + (49 + i)*nch, pcmb, 3);
            vst1_lane_s16(dstl + (47 - i)*nch, pcma, 2);
            vst1_lane_s16(dstl + (49 + i)*nch, pcmb, 2);
#endif
#else
            static const drmp3_f4 g_scale = { 1.0f/32768.0f, 1.0f/32768.0f, 1.0f/32768.0f, 1.0f/32768.0f };
            a = DRMP3_VMUL(a, g_scale);
            b = DRMP3_VMUL(b, g_scale);
#if DRMP3_HAVE_SSE
            _mm_store_ss(dstr + (15 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 1, 1, 1)));
            _mm_store_ss(dstr + (17 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(1, 1, 1, 1)));
            _mm_store_ss(dstl + (15 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 0, 0, 0)));
            _mm_store_ss(dstl + (17 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(0, 0, 0, 0)));
            _mm_store_ss(dstr + (47 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 3, 3, 3)));
            _mm_store_ss(dstr + (49 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 3, 3, 3)));
            _mm_store_ss(dstl + (47 - i)*nch, _mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 2, 2, 2)));
            _mm_store_ss(dstl + (49 + i)*nch, _mm_shuffle_ps(b, b, _MM_SHUFFLE(2, 2, 2, 2)));
#else
            vst1q_lane_f32(dstr + (15 - i)*nch, a, 1);
            vst1q_lane_f32(dstr + (17 + i)*nch, b, 1);
            vst1q_lane_f32(dstl + (15 - i)*nch, a, 0);
            vst1q_lane_f32(dstl + (17 + i)*nch, b, 0);
            vst1q_lane_f32(dstr + (47 - i)*nch, a, 3);
            vst1q_lane_f32(dstr + (49 + i)*nch, b, 3);
            vst1q_lane_f32(dstl + (47 - i)*nch, a, 2);
            vst1q_lane_f32(dstl + (49 + i)*nch, b, 2);
#endif
#endif /* DR_MP3_FLOAT_OUTPUT */
        }
    } else
#endif
#ifdef DR_MP3_ONLY_SIMD
    {}
#else
    for (i = 14; i >= 0; i--)
    {
#define DRMP3_LOAD(k) float w0 = *w++; float w1 = *w++; float *vz = &zlin[4*i - k*64]; float *vy = &zlin[4*i - (15 - k)*64];
#define DRMP3_S0(k) { int j; DRMP3_LOAD(k); for (j = 0; j < 4; j++) b[j]  = vz[j]*w1 + vy[j]*w0, a[j]  = vz[j]*w0 - vy[j]*w1; }
#define DRMP3_S1(k) { int j; DRMP3_LOAD(k); for (j = 0; j < 4; j++) b[j] += vz[j]*w1 + vy[j]*w0, a[j] += vz[j]*w0 - vy[j]*w1; }
#define DRMP3_S2(k) { int j; DRMP3_LOAD(k); for (j = 0; j < 4; j++) b[j] += vz[j]*w1 + vy[j]*w0, a[j] += vy[j]*w1 - vz[j]*w0; }
        float a[4], b[4];

        zlin[4*i]     = xl[18*(31 - i)];
        zlin[4*i + 1] = xr[18*(31 - i)];
        zlin[4*i + 2] = xl[1 + 18*(31 - i)];
        zlin[4*i + 3] = xr[1 + 18*(31 - i)];
        zlin[4*(i + 16)]   = xl[1 + 18*(1 + i)];
        zlin[4*(i + 16) + 1] = xr[1 + 18*(1 + i)];
        zlin[4*(i - 16) + 2] = xl[18*(1 + i)];
        zlin[4*(i - 16) + 3] = xr[18*(1 + i)];

        DRMP3_S0(0) DRMP3_S2(1) DRMP3_S1(2) DRMP3_S2(3) DRMP3_S1(4) DRMP3_S2(5) DRMP3_S1(6) DRMP3_S2(7)

        dstr[(15 - i)*nch] = drmp3d_scale_pcm(a[1]);
        dstr[(17 + i)*nch] = drmp3d_scale_pcm(b[1]);
        dstl[(15 - i)*nch] = drmp3d_scale_pcm(a[0]);
        dstl[(17 + i)*nch] = drmp3d_scale_pcm(b[0]);
        dstr[(47 - i)*nch] = drmp3d_scale_pcm(a[3]);
        dstr[(49 + i)*nch] = drmp3d_scale_pcm(b[3]);
        dstl[(47 - i)*nch] = drmp3d_scale_pcm(a[2]);
        dstl[(49 + i)*nch] = drmp3d_scale_pcm(b[2]);
    }
#endif
}

static void drmp3d_synth_granule(float *qmf_state, float *grbuf, int nbands, int nch, drmp3d_sample_t *pcm, float *lins)
{
    int i;
    for (i = 0; i < nch; i++)
    {
        drmp3d_DCT_II(grbuf + 576*i, nbands);
    }

    memcpy(lins, qmf_state, sizeof(float)*15*64);

    for (i = 0; i < nbands; i += 2)
    {
        drmp3d_synth(grbuf + i, pcm + 32*nch*i, nch, lins + i*64);
    }
#ifndef DR_MP3_NONSTANDARD_BUT_LOGICAL
    if (nch == 1)
    {
        for (i = 0; i < 15*64; i += 2)
        {
            qmf_state[i] = lins[nbands*64 + i];
        }
    } else
#endif
    {
        memcpy(qmf_state, lins + nbands*64, sizeof(float)*15*64);
    }
}

static int drmp3d_match_frame(const drmp3_uint8 *hdr, int mp3_bytes, int frame_bytes)
{
    int i, nmatch;
    for (i = 0, nmatch = 0; nmatch < DRMP3_MAX_FRAME_SYNC_MATCHES; nmatch++)
    {
        i += drmp3_hdr_frame_bytes(hdr + i, frame_bytes) + drmp3_hdr_padding(hdr + i);
        if (i + DRMP3_HDR_SIZE > mp3_bytes)
            return nmatch > 0;
        if (!drmp3_hdr_compare(hdr, hdr + i))
            return 0;
    }
    return 1;
}

static int drmp3d_find_frame(const drmp3_uint8 *mp3, int mp3_bytes, int *free_format_bytes, int *ptr_frame_bytes)
{
    int i, k;
    for (i = 0; i < mp3_bytes - DRMP3_HDR_SIZE; i++, mp3++)
    {
        if (drmp3_hdr_valid(mp3))
        {
            int frame_bytes = drmp3_hdr_frame_bytes(mp3, *free_format_bytes);
            int frame_and_padding = frame_bytes + drmp3_hdr_padding(mp3);

            for (k = DRMP3_HDR_SIZE; !frame_bytes && k < DRMP3_MAX_FREE_FORMAT_FRAME_SIZE && i + 2*k < mp3_bytes - DRMP3_HDR_SIZE; k++)
            {
                if (drmp3_hdr_compare(mp3, mp3 + k))
                {
                    int fb = k - drmp3_hdr_padding(mp3);
                    int nextfb = fb + drmp3_hdr_padding(mp3 + k);
                    if (i + k + nextfb + DRMP3_HDR_SIZE > mp3_bytes || !drmp3_hdr_compare(mp3, mp3 + k + nextfb))
                        continue;
                    frame_and_padding = k;
                    frame_bytes = fb;
                    *free_format_bytes = fb;
                }
            }

            if ((frame_bytes && i + frame_and_padding <= mp3_bytes &&
                drmp3d_match_frame(mp3, mp3_bytes - i, frame_bytes)) ||
                (!i && frame_and_padding == mp3_bytes))
            {
                *ptr_frame_bytes = frame_and_padding;
                return i;
            }
            *free_format_bytes = 0;
        }
    }
    *ptr_frame_bytes = 0;
    return i;
}

void drmp3dec_init(drmp3dec *dec)
{
    dec->header[0] = 0;
}

int drmp3dec_decode_frame(drmp3dec *dec, const unsigned char *mp3, int mp3_bytes, void *pcm, drmp3dec_frame_info *info)
{
    int i = 0, igr, frame_size = 0, success = 1;
    const drmp3_uint8 *hdr;
    drmp3_bs bs_frame[1];
    drmp3dec_scratch scratch;

    if (mp3_bytes > 4 && dec->header[0] == 0xff && drmp3_hdr_compare(dec->header, mp3))
    {
        frame_size = drmp3_hdr_frame_bytes(mp3, dec->free_format_bytes) + drmp3_hdr_padding(mp3);
        if (frame_size != mp3_bytes && (frame_size + DRMP3_HDR_SIZE > mp3_bytes || !drmp3_hdr_compare(mp3, mp3 + frame_size)))
        {
            frame_size = 0;
        }
    }
    if (!frame_size)
    {
        memset(dec, 0, sizeof(drmp3dec));
        i = drmp3d_find_frame(mp3, mp3_bytes, &dec->free_format_bytes, &frame_size);
        if (!frame_size || i + frame_size > mp3_bytes)
        {
            info->frame_bytes = i;
            return 0;
        }
    }

    hdr = mp3 + i;
    memcpy(dec->header, hdr, DRMP3_HDR_SIZE);
    info->frame_bytes = i + frame_size;
    info->channels = DRMP3_HDR_IS_MONO(hdr) ? 1 : 2;
    info->hz = drmp3_hdr_sample_rate_hz(hdr);
    info->layer = 4 - DRMP3_HDR_GET_LAYER(hdr);
    info->bitrate_kbps = drmp3_hdr_bitrate_kbps(hdr);

    drmp3_bs_init(bs_frame, hdr + DRMP3_HDR_SIZE, frame_size - DRMP3_HDR_SIZE);
    if (DRMP3_HDR_IS_CRC(hdr))
    {
        drmp3_bs_get_bits(bs_frame, 16);
    }

    if (info->layer == 3)
    {
        int main_data_begin = drmp3_L3_read_side_info(bs_frame, scratch.gr_info, hdr);
        if (main_data_begin < 0 || bs_frame->pos > bs_frame->limit)
        {
            drmp3dec_init(dec);
            return 0;
        }
        success = drmp3_L3_restore_reservoir(dec, bs_frame, &scratch, main_data_begin);
        if (success && pcm != NULL)
        {
            for (igr = 0; igr < (DRMP3_HDR_TEST_MPEG1(hdr) ? 2 : 1); igr++, pcm = DRMP3_OFFSET_PTR(pcm, sizeof(drmp3d_sample_t)*576*info->channels))
            {
                memset(scratch.grbuf[0], 0, 576*2*sizeof(float));
                drmp3_L3_decode(dec, &scratch, scratch.gr_info + igr*info->channels, info->channels);
                drmp3d_synth_granule(dec->qmf_state, scratch.grbuf[0], 18, info->channels, (drmp3d_sample_t*)pcm, scratch.syn[0]);
            }
        }
        drmp3_L3_save_reservoir(dec, &scratch);
    } else
    {
#ifdef DR_MP3_ONLY_MP3
        return 0;
#else
        if (pcm == NULL) {
            return drmp3_hdr_frame_samples(hdr);
        }

        drmp3_L12_scale_info sci[1];
        drmp3_L12_read_scale_info(hdr, bs_frame, sci);

        memset(scratch.grbuf[0], 0, 576*2*sizeof(float));
        for (i = 0, igr = 0; igr < 3; igr++)
        {
            if (12 == (i += drmp3_L12_dequantize_granule(scratch.grbuf[0] + i, bs_frame, sci, info->layer | 1)))
            {
                i = 0;
                drmp3_L12_apply_scf_384(sci, sci->scf + igr, scratch.grbuf[0]);
                drmp3d_synth_granule(dec->qmf_state, scratch.grbuf[0], 12, info->channels, (drmp3d_sample_t*)pcm, scratch.syn[0]);
                memset(scratch.grbuf[0], 0, 576*2*sizeof(float));
                pcm = DRMP3_OFFSET_PTR(pcm, sizeof(drmp3d_sample_t)*384*info->channels);
            }
            if (bs_frame->pos > bs_frame->limit)
            {
                drmp3dec_init(dec);
                return 0;
            }
        }
#endif
    }

    return success*drmp3_hdr_frame_samples(dec->header);
}

void drmp3dec_f32_to_s16(const float *in, drmp3_int16 *out, int num_samples)
{
    if(num_samples > 0)
    {
        int i = 0;
#if DRMP3_HAVE_SIMD
        int aligned_count = num_samples & ~7;
        for(; i < aligned_count; i+=8)
        {
            static const drmp3_f4 g_scale = { 32768.0f, 32768.0f, 32768.0f, 32768.0f };
            drmp3_f4 a = DRMP3_VMUL(DRMP3_VLD(&in[i  ]), g_scale);
            drmp3_f4 b = DRMP3_VMUL(DRMP3_VLD(&in[i+4]), g_scale);
#if DRMP3_HAVE_SSE
            static const drmp3_f4 g_max = { 32767.0f, 32767.0f, 32767.0f, 32767.0f };
            static const drmp3_f4 g_min = { -32768.0f, -32768.0f, -32768.0f, -32768.0f };
            __m128i pcm8 = _mm_packs_epi32(_mm_cvtps_epi32(_mm_max_ps(_mm_min_ps(a, g_max), g_min)),
                                           _mm_cvtps_epi32(_mm_max_ps(_mm_min_ps(b, g_max), g_min)));
            out[i  ] = (drmp3_int16)_mm_extract_epi16(pcm8, 0);
            out[i+1] = (drmp3_int16)_mm_extract_epi16(pcm8, 1);
            out[i+2] = (drmp3_int16)_mm_extract_epi16(pcm8, 2);
            out[i+3] = (drmp3_int16)_mm_extract_epi16(pcm8, 3);
            out[i+4] = (drmp3_int16)_mm_extract_epi16(pcm8, 4);
            out[i+5] = (drmp3_int16)_mm_extract_epi16(pcm8, 5);
            out[i+6] = (drmp3_int16)_mm_extract_epi16(pcm8, 6);
            out[i+7] = (drmp3_int16)_mm_extract_epi16(pcm8, 7);
#else
            int16x4_t pcma, pcmb;
            a = DRMP3_VADD(a, DRMP3_VSET(0.5f));
            b = DRMP3_VADD(b, DRMP3_VSET(0.5f));
            pcma = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(a), vreinterpretq_s32_u32(vcltq_f32(a, DRMP3_VSET(0)))));
            pcmb = vqmovn_s32(vqaddq_s32(vcvtq_s32_f32(b), vreinterpretq_s32_u32(vcltq_f32(b, DRMP3_VSET(0)))));
            vst1_lane_s16(out+i  , pcma, 0);
            vst1_lane_s16(out+i+1, pcma, 1);
            vst1_lane_s16(out+i+2, pcma, 2);
            vst1_lane_s16(out+i+3, pcma, 3);
            vst1_lane_s16(out+i+4, pcmb, 0);
            vst1_lane_s16(out+i+5, pcmb, 1);
            vst1_lane_s16(out+i+6, pcmb, 2);
            vst1_lane_s16(out+i+7, pcmb, 3);
#endif
        }
#endif
        for(; i < num_samples; i++)
        {
            float sample = in[i] * 32768.0f;
            if (sample >=  32766.5)
                out[i] = (drmp3_int16) 32767;
            else if (sample <= -32767.5)
                out[i] = (drmp3_int16)-32768;
            else
            {
                short s = (drmp3_int16)(sample + .5f);
                s -= (s < 0);   /* away from zero, to be compliant */
                out[i] = s;
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
//
// Main Public API
//
///////////////////////////////////////////////////////////////////////////////

#if defined(SIZE_MAX)
    #define DRMP3_SIZE_MAX  SIZE_MAX
#else
    #if defined(_WIN64) || defined(_LP64) || defined(__LP64__)
        #define DRMP3_SIZE_MAX  ((drmp3_uint64)0xFFFFFFFFFFFFFFFF)
    #else
        #define DRMP3_SIZE_MAX  0xFFFFFFFF
    #endif
#endif

// Options.
#ifndef DR_MP3_DEFAULT_CHANNELS
#define DR_MP3_DEFAULT_CHANNELS         2
#endif
#ifndef DR_MP3_DEFAULT_SAMPLE_RATE
#define DR_MP3_DEFAULT_SAMPLE_RATE      44100
#endif
#ifndef DRMP3_SEEK_LEADING_MP3_FRAMES
#define DRMP3_SEEK_LEADING_MP3_FRAMES   2
#endif


// Standard library stuff.
#ifndef DRMP3_ASSERT
#include <assert.h>
#define DRMP3_ASSERT(expression)           assert(expression)
#endif
#ifndef DRMP3_COPY_MEMORY
#define DRMP3_COPY_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#ifndef DRMP3_ZERO_MEMORY
#define DRMP3_ZERO_MEMORY(p, sz) memset((p), 0, (sz))
#endif
#define DRMP3_ZERO_OBJECT(p) DRMP3_ZERO_MEMORY((p), sizeof(*(p)))
#ifndef DRMP3_MALLOC
#define DRMP3_MALLOC(sz) malloc((sz))
#endif
#ifndef DRMP3_REALLOC
#define DRMP3_REALLOC(p, sz) realloc((p), (sz))
#endif
#ifndef DRMP3_FREE
#define DRMP3_FREE(p) free((p))
#endif

#define drmp3_assert        DRMP3_ASSERT
#define drmp3_copy_memory   DRMP3_COPY_MEMORY
#define drmp3_zero_memory   DRMP3_ZERO_MEMORY
#define drmp3_zero_object   DRMP3_ZERO_OBJECT
#define drmp3_malloc        DRMP3_MALLOC
#define drmp3_realloc       DRMP3_REALLOC

#define drmp3_countof(x)  (sizeof(x) / sizeof(x[0]))
#define drmp3_max(x, y)   (((x) > (y)) ? (x) : (y))
#define drmp3_min(x, y)   (((x) < (y)) ? (x) : (y))

#define DRMP3_DATA_CHUNK_SIZE  16384    // The size in bytes of each chunk of data to read from the MP3 stream. minimp3 recommends 16K.

static inline float drmp3_mix_f32(float x, float y, float a)
{
    return x*(1-a) + y*a;
}

static void drmp3_blend_f32(float* pOut, float* pInA, float* pInB, float factor, drmp3_uint32 channels)
{
    for (drmp3_uint32 i = 0; i < channels; ++i) {
        pOut[i] = drmp3_mix_f32(pInA[i], pInB[i], factor);
    }
}

void drmp3_src_cache_init(drmp3_src* pSRC, drmp3_src_cache* pCache)
{
    drmp3_assert(pSRC != NULL);
    drmp3_assert(pCache != NULL);

    pCache->pSRC = pSRC;
    pCache->cachedFrameCount = 0;
    pCache->iNextFrame = 0;
}

drmp3_uint64 drmp3_src_cache_read_frames(drmp3_src_cache* pCache, drmp3_uint64 frameCount, float* pFramesOut)
{
    drmp3_assert(pCache != NULL);
    drmp3_assert(pCache->pSRC != NULL);
    drmp3_assert(pCache->pSRC->onRead != NULL);
    drmp3_assert(frameCount > 0);
    drmp3_assert(pFramesOut != NULL);

    drmp3_uint32 channels = pCache->pSRC->config.channels;

    drmp3_uint64 totalFramesRead = 0;
    while (frameCount > 0) {
        // If there's anything in memory go ahead and copy that over first.
        drmp3_uint64 framesRemainingInMemory = pCache->cachedFrameCount - pCache->iNextFrame;
        drmp3_uint64 framesToReadFromMemory = frameCount;
        if (framesToReadFromMemory > framesRemainingInMemory) {
            framesToReadFromMemory = framesRemainingInMemory;
        }

        drmp3_copy_memory(pFramesOut, pCache->pCachedFrames + pCache->iNextFrame*channels, (drmp3_uint32)(framesToReadFromMemory * channels * sizeof(float)));
        pCache->iNextFrame += (drmp3_uint32)framesToReadFromMemory;

        totalFramesRead += framesToReadFromMemory;
        frameCount -= framesToReadFromMemory;
        if (frameCount == 0) {
            break;
        }


        // At this point there are still more frames to read from the client, so we'll need to reload the cache with fresh data.
        drmp3_assert(frameCount > 0);
        pFramesOut += framesToReadFromMemory * channels;

        pCache->iNextFrame = 0;
        pCache->cachedFrameCount = 0;

        drmp3_uint32 framesToReadFromClient = drmp3_countof(pCache->pCachedFrames) / pCache->pSRC->config.channels;
        if (framesToReadFromClient > pCache->pSRC->config.cacheSizeInFrames) {
            framesToReadFromClient = pCache->pSRC->config.cacheSizeInFrames;
        }

        pCache->cachedFrameCount = (drmp3_uint32)pCache->pSRC->onRead(pCache->pSRC, framesToReadFromClient, pCache->pCachedFrames, pCache->pSRC->pUserData);


        // Get out of this loop if nothing was able to be retrieved.
        if (pCache->cachedFrameCount == 0) {
            break;
        }
    }

    return totalFramesRead;
}


drmp3_uint64 drmp3_src_read_frames_passthrough(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut, drmp3_bool32 flush);
drmp3_uint64 drmp3_src_read_frames_linear(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut, drmp3_bool32 flush);

drmp3_bool32 drmp3_src_init(const drmp3_src_config* pConfig, drmp3_src_read_proc onRead, void* pUserData, drmp3_src* pSRC)
{
    if (pSRC == NULL) return DRMP3_FALSE;
    drmp3_zero_object(pSRC);

    if (pConfig == NULL || onRead == NULL) return DRMP3_FALSE;
    if (pConfig->channels == 0 || pConfig->channels > 2) return DRMP3_FALSE;

    pSRC->config = *pConfig;
    pSRC->onRead = onRead;
    pSRC->pUserData = pUserData;

    if (pSRC->config.cacheSizeInFrames > DRMP3_SRC_CACHE_SIZE_IN_FRAMES || pSRC->config.cacheSizeInFrames == 0) {
        pSRC->config.cacheSizeInFrames = DRMP3_SRC_CACHE_SIZE_IN_FRAMES;
    }

    drmp3_src_cache_init(pSRC, &pSRC->cache);
    return DRMP3_TRUE;
}

drmp3_bool32 drmp3_src_set_input_sample_rate(drmp3_src* pSRC, drmp3_uint32 sampleRateIn)
{
    if (pSRC == NULL) return DRMP3_FALSE;

    // Must have a sample rate of > 0.
    if (sampleRateIn == 0) {
        return DRMP3_FALSE;
    }

    pSRC->config.sampleRateIn = sampleRateIn;
    return DRMP3_TRUE;
}

drmp3_bool32 drmp3_src_set_output_sample_rate(drmp3_src* pSRC, drmp3_uint32 sampleRateOut)
{
    if (pSRC == NULL) return DRMP3_FALSE;

    // Must have a sample rate of > 0.
    if (sampleRateOut == 0) {
        return DRMP3_FALSE;
    }

    pSRC->config.sampleRateOut = sampleRateOut;
    return DRMP3_TRUE;
}

drmp3_uint64 drmp3_src_read_frames_ex(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut, drmp3_bool32 flush)
{
    if (pSRC == NULL || frameCount == 0 || pFramesOut == NULL) return 0;

    drmp3_src_algorithm algorithm = pSRC->config.algorithm;

    // Always use passthrough if the sample rates are the same.
    if (pSRC->config.sampleRateIn == pSRC->config.sampleRateOut) {
        algorithm = drmp3_src_algorithm_none;
    }

    // Could just use a function pointer instead of a switch for this...
    switch (algorithm)
    {
        case drmp3_src_algorithm_none:   return drmp3_src_read_frames_passthrough(pSRC, frameCount, pFramesOut, flush);
        case drmp3_src_algorithm_linear: return drmp3_src_read_frames_linear(pSRC, frameCount, pFramesOut, flush);
        default: return 0;
    }
}

drmp3_uint64 drmp3_src_read_frames(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut)
{
    return drmp3_src_read_frames_ex(pSRC, frameCount, pFramesOut, DRMP3_FALSE);
}

drmp3_uint64 drmp3_src_read_frames_passthrough(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut, drmp3_bool32 flush)
{
    drmp3_assert(pSRC != NULL);
    drmp3_assert(frameCount > 0);
    drmp3_assert(pFramesOut != NULL);

    (void)flush;    // Passthrough need not care about flushing.
    return pSRC->onRead(pSRC, frameCount, pFramesOut, pSRC->pUserData);
}

drmp3_uint64 drmp3_src_read_frames_linear(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut, drmp3_bool32 flush)
{
    drmp3_assert(pSRC != NULL);
    drmp3_assert(frameCount > 0);
    drmp3_assert(pFramesOut != NULL);

    // For linear SRC, the bin is only 2 frames: 1 prior, 1 future.

    // Load the bin if necessary.
    if (!pSRC->algo.linear.isPrevFramesLoaded) {
        drmp3_uint64 framesRead = drmp3_src_cache_read_frames(&pSRC->cache, 1, pSRC->bin);
        if (framesRead == 0) {
            return 0;
        }
        pSRC->algo.linear.isPrevFramesLoaded = DRMP3_TRUE;
    }
    if (!pSRC->algo.linear.isNextFramesLoaded) {
        drmp3_uint64 framesRead = drmp3_src_cache_read_frames(&pSRC->cache, 1, pSRC->bin + pSRC->config.channels);
        if (framesRead == 0) {
            return 0;
        }
        pSRC->algo.linear.isNextFramesLoaded = DRMP3_TRUE;
    }

    double factor = (double)pSRC->config.sampleRateIn / pSRC->config.sampleRateOut;

    drmp3_uint64 totalFramesRead = 0;
    while (frameCount > 0) {
        // The bin is where the previous and next frames are located.
        float* pPrevFrame = pSRC->bin;
        float* pNextFrame = pSRC->bin + pSRC->config.channels;

        drmp3_blend_f32((float*)pFramesOut, pPrevFrame, pNextFrame, (float)pSRC->algo.linear.alpha, pSRC->config.channels);

        pSRC->algo.linear.alpha += factor;

        // The new alpha value is how we determine whether or not we need to read fresh frames.
        drmp3_uint32 framesToReadFromClient = (drmp3_uint32)pSRC->algo.linear.alpha;
        pSRC->algo.linear.alpha = pSRC->algo.linear.alpha - framesToReadFromClient;

        for (drmp3_uint32 i = 0; i < framesToReadFromClient; ++i) {
            for (drmp3_uint32 j = 0; j < pSRC->config.channels; ++j) {
                pPrevFrame[j] = pNextFrame[j];
            }

            drmp3_uint64 framesRead = drmp3_src_cache_read_frames(&pSRC->cache, 1, pNextFrame);
            if (framesRead == 0) {
                for (drmp3_uint32 j = 0; j < pSRC->config.channels; ++j) {
                    pNextFrame[j] = 0;
                }

                if (pSRC->algo.linear.isNextFramesLoaded) {
                    pSRC->algo.linear.isNextFramesLoaded = DRMP3_FALSE;
                } else {
                    if (flush) {
                        pSRC->algo.linear.isPrevFramesLoaded = DRMP3_FALSE;
                    }
                }

                break;
            }
        }

        pFramesOut  = (drmp3_uint8*)pFramesOut + (1 * pSRC->config.channels * sizeof(float));
        frameCount -= 1;
        totalFramesRead += 1;

        // If there's no frames available we need to get out of this loop.
        if (!pSRC->algo.linear.isNextFramesLoaded && (!flush || !pSRC->algo.linear.isPrevFramesLoaded)) {
            break;
        }
    }

    return totalFramesRead;
}


static size_t drmp3__on_read(drmp3* pMP3, void* pBufferOut, size_t bytesToRead)
{
    size_t bytesRead = pMP3->onRead(pMP3->pUserData, pBufferOut, bytesToRead);
    pMP3->streamCursor += bytesRead;
    return bytesRead;
}

static drmp3_bool32 drmp3__on_seek(drmp3* pMP3, int offset, drmp3_seek_origin origin)
{
    drmp3_assert(offset >= 0);

    if (!pMP3->onSeek(pMP3->pUserData, offset, origin)) {
        return DRMP3_FALSE;
    }

    if (origin == drmp3_seek_origin_start) {
        pMP3->streamCursor = (drmp3_uint64)offset;
    } else {
        pMP3->streamCursor += offset;
    }

    return DRMP3_TRUE;
}

static drmp3_bool32 drmp3__on_seek_64(drmp3* pMP3, drmp3_uint64 offset, drmp3_seek_origin origin)
{
    if (offset <= 0x7FFFFFFF) {
        return drmp3__on_seek(pMP3, (int)offset, origin);
    }


    // Getting here "offset" is too large for a 32-bit integer. We just keep seeking forward until we hit the offset.
    if (!drmp3__on_seek(pMP3, 0x7FFFFFFF, drmp3_seek_origin_start)) {
        return DRMP3_FALSE;
    }

    offset -= 0x7FFFFFFF;
    while (offset > 0) {
        if (offset <= 0x7FFFFFFF) {
            if (!drmp3__on_seek(pMP3, (int)offset, drmp3_seek_origin_current)) {
                return DRMP3_FALSE;
            }
            offset = 0;
        } else {
            if (!drmp3__on_seek(pMP3, 0x7FFFFFFF, drmp3_seek_origin_current)) {
                return DRMP3_FALSE;
            }
            offset -= 0x7FFFFFFF;
        }
    }

    return DRMP3_TRUE;
}




static drmp3_uint32 drmp3_decode_next_frame_ex(drmp3* pMP3, drmp3d_sample_t* pPCMFrames, drmp3_bool32 discard)
{
    drmp3_assert(pMP3 != NULL);
    drmp3_assert(pMP3->onRead != NULL);

    if (pMP3->atEnd) {
        return 0;
    }

    drmp3_uint32 pcmFramesRead = 0;
    do {
        // minimp3 recommends doing data submission in 16K chunks. If we don't have at least 16K bytes available, get more.
        if (pMP3->dataSize < DRMP3_DATA_CHUNK_SIZE) {
            if (pMP3->dataCapacity < DRMP3_DATA_CHUNK_SIZE) {
                pMP3->dataCapacity = DRMP3_DATA_CHUNK_SIZE;
                drmp3_uint8* pNewData = (drmp3_uint8*)drmp3_realloc(pMP3->pData, pMP3->dataCapacity);
                if (pNewData == NULL) {
                    return 0; // Out of memory.
                }

                pMP3->pData = pNewData;
            }

            size_t bytesRead = drmp3__on_read(pMP3, pMP3->pData + pMP3->dataSize, (pMP3->dataCapacity - pMP3->dataSize));
            if (bytesRead == 0) {
                if (pMP3->dataSize == 0) {
                    pMP3->atEnd = DRMP3_TRUE;
                    return 0; // No data.
                }
            }

            pMP3->dataSize += bytesRead;
        }

        if (pMP3->dataSize > INT_MAX) {
            pMP3->atEnd = DRMP3_TRUE;
            return 0; // File too big.
        }

        drmp3dec_frame_info info;
        pcmFramesRead = drmp3dec_decode_frame(&pMP3->decoder, pMP3->pData, (int)pMP3->dataSize, pPCMFrames, &info);    // <-- Safe size_t -> int conversion thanks to the check above.

        // Consume the data.
        size_t leftoverDataSize = (pMP3->dataSize - (size_t)info.frame_bytes);
        if (info.frame_bytes > 0) {
            memmove(pMP3->pData, pMP3->pData + info.frame_bytes, leftoverDataSize);
            pMP3->dataSize = leftoverDataSize;
        }

        // pcmFramesRead will be equal to 0 if decoding failed. If it is zero and info.frame_bytes > 0 then we have successfully
        // decoded the frame. A special case is if we are wanting to discard the frame, in which case we return successfully.
        if (pcmFramesRead > 0 || (info.frame_bytes > 0 && discard)) {
            pcmFramesRead = drmp3_hdr_frame_samples(pMP3->decoder.header);
            pMP3->pcmFramesConsumedInMP3Frame = 0;
            pMP3->pcmFramesRemainingInMP3Frame = pcmFramesRead;
            pMP3->mp3FrameChannels = info.channels;
            pMP3->mp3FrameSampleRate = info.hz;
            drmp3_src_set_input_sample_rate(&pMP3->src, pMP3->mp3FrameSampleRate);
            break;
        } else if (info.frame_bytes == 0) {
            // Need more data. minimp3 recommends doing data submission in 16K chunks.
            if (pMP3->dataCapacity == pMP3->dataSize) {
                // No room. Expand.
                pMP3->dataCapacity += DRMP3_DATA_CHUNK_SIZE;
                drmp3_uint8* pNewData = (drmp3_uint8*)drmp3_realloc(pMP3->pData, pMP3->dataCapacity);
                if (pNewData == NULL) {
                    return 0; // Out of memory.
                }

                pMP3->pData = pNewData;
            }

            // Fill in a chunk.
            size_t bytesRead = drmp3__on_read(pMP3, pMP3->pData + pMP3->dataSize, (pMP3->dataCapacity - pMP3->dataSize));
            if (bytesRead == 0) {
                pMP3->atEnd = DRMP3_TRUE;
                return 0; // Error reading more data.
            }

            pMP3->dataSize += bytesRead;
        }
    } while (DRMP3_TRUE);

    return pcmFramesRead;
}

static drmp3_uint32 drmp3_decode_next_frame(drmp3* pMP3)
{
    drmp3_assert(pMP3 != NULL);
    return drmp3_decode_next_frame_ex(pMP3, (drmp3d_sample_t*)pMP3->pcmFrames, DRMP3_FALSE);
}

#if 0
static drmp3_uint32 drmp3_seek_next_frame(drmp3* pMP3)
{
    drmp3_assert(pMP3 != NULL);

    drmp3_uint32 pcmFrameCount = drmp3_decode_next_frame_ex(pMP3, NULL);
    if (pcmFrameCount == 0) {
        return 0;
    }

    // We have essentially just skipped past the frame, so just set the remaining samples to 0.
    pMP3->currentPCMFrame             += pcmFrameCount;
    pMP3->pcmFramesConsumedInMP3Frame  = pcmFrameCount;
    pMP3->pcmFramesRemainingInMP3Frame = 0;

    return pcmFrameCount;
}
#endif

static drmp3_uint64 drmp3_read_src(drmp3_src* pSRC, drmp3_uint64 frameCount, void* pFramesOut, void* pUserData)
{
    drmp3* pMP3 = (drmp3*)pUserData;
    drmp3_assert(pMP3 != NULL);
    drmp3_assert(pMP3->onRead != NULL);

    float* pFramesOutF = (float*)pFramesOut;
    drmp3_uint64 totalFramesRead = 0;

    while (frameCount > 0) {
        // Read from the in-memory buffer first.
        while (pMP3->pcmFramesRemainingInMP3Frame > 0 && frameCount > 0) {
            drmp3d_sample_t* frames = (drmp3d_sample_t*)pMP3->pcmFrames;
#ifndef DR_MP3_FLOAT_OUTPUT
            if (pMP3->mp3FrameChannels == 1) {
                if (pMP3->channels == 1) {
                    // Mono -> Mono.
                    pFramesOutF[0] = frames[pMP3->pcmFramesConsumedInMP3Frame] / 32768.0f;
                } else {
                    // Mono -> Stereo.
                    pFramesOutF[0] = frames[pMP3->pcmFramesConsumedInMP3Frame] / 32768.0f;
                    pFramesOutF[1] = frames[pMP3->pcmFramesConsumedInMP3Frame] / 32768.0f;
                }
            } else {
                if (pMP3->channels == 1) {
                    // Stereo -> Mono
                    float sample = 0;
                    sample += frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+0] / 32768.0f;
                    sample += frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+1] / 32768.0f;
                    pFramesOutF[0] = sample * 0.5f;
                } else {
                    // Stereo -> Stereo
                    pFramesOutF[0] = frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+0] / 32768.0f;
                    pFramesOutF[1] = frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+1] / 32768.0f;
                }
            }
#else
            if (pMP3->mp3FrameChannels == 1) {
                if (pMP3->channels == 1) {
                    // Mono -> Mono.
                    pFramesOutF[0] = frames[pMP3->pcmFramesConsumedInMP3Frame];
                } else {
                    // Mono -> Stereo.
                    pFramesOutF[0] = frames[pMP3->pcmFramesConsumedInMP3Frame];
                    pFramesOutF[1] = frames[pMP3->pcmFramesConsumedInMP3Frame];
                }
            } else {
                if (pMP3->channels == 1) {
                    // Stereo -> Mono
                    float sample = 0;
                    sample += frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+0];
                    sample += frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+1];
                    pFramesOutF[0] = sample * 0.5f;
                } else {
                    // Stereo -> Stereo
                    pFramesOutF[0] = frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+0];
                    pFramesOutF[1] = frames[(pMP3->pcmFramesConsumedInMP3Frame*pMP3->mp3FrameChannels)+1];
                }
            }
#endif

            pMP3->pcmFramesConsumedInMP3Frame += 1;
            pMP3->pcmFramesRemainingInMP3Frame -= 1;
            totalFramesRead += 1;
            frameCount -= 1;
            pFramesOutF += pSRC->config.channels;
        }

        if (frameCount == 0) {
            break;
        }

        drmp3_assert(pMP3->pcmFramesRemainingInMP3Frame == 0);

        // At this point we have exhausted our in-memory buffer so we need to re-fill. Note that the sample rate may have changed
        // at this point which means we'll also need to update our sample rate conversion pipeline.
        if (drmp3_decode_next_frame(pMP3) == 0) {
            break;
        }
    }

    return totalFramesRead;
}

drmp3_bool32 drmp3_init_internal(drmp3* pMP3, drmp3_read_proc onRead, drmp3_seek_proc onSeek, void* pUserData, const drmp3_config* pConfig)
{
    drmp3_assert(pMP3 != NULL);
    drmp3_assert(onRead != NULL);

    // This function assumes the output object has already been reset to 0. Do not do that here, otherwise things will break.
    drmp3dec_init(&pMP3->decoder);

    // The config can be null in which case we use defaults.
    drmp3_config config;
    if (pConfig != NULL) {
        config = *pConfig;
    } else {
        drmp3_zero_object(&config);
    }

    pMP3->channels = config.outputChannels;
    if (pMP3->channels == 0) {
        pMP3->channels = DR_MP3_DEFAULT_CHANNELS;
    }

    // Cannot have more than 2 channels.
    if (pMP3->channels > 2) {
        pMP3->channels = 2;
    }

    pMP3->sampleRate = config.outputSampleRate;
    if (pMP3->sampleRate == 0) {
        pMP3->sampleRate = DR_MP3_DEFAULT_SAMPLE_RATE;
    }

    pMP3->onRead = onRead;
    pMP3->onSeek = onSeek;
    pMP3->pUserData = pUserData;

    // We need a sample rate converter for converting the sample rate from the MP3 frames to the requested output sample rate.
    drmp3_src_config srcConfig;
    drmp3_zero_object(&srcConfig);
    srcConfig.sampleRateIn = DR_MP3_DEFAULT_SAMPLE_RATE;
    srcConfig.sampleRateOut = pMP3->sampleRate;
    srcConfig.channels = pMP3->channels;
    srcConfig.algorithm = drmp3_src_algorithm_linear;
    if (!drmp3_src_init(&srcConfig, drmp3_read_src, pMP3, &pMP3->src)) {
        drmp3_uninit(pMP3);
        return DRMP3_FALSE;
    }

    // Decode the first frame to confirm that it is indeed a valid MP3 stream.
    if (!drmp3_decode_next_frame(pMP3)) {
        drmp3_uninit(pMP3);
        return DRMP3_FALSE; // Not a valid MP3 stream.
    }

    return DRMP3_TRUE;
}

drmp3_bool32 drmp3_init(drmp3* pMP3, drmp3_read_proc onRead, drmp3_seek_proc onSeek, void* pUserData, const drmp3_config* pConfig)
{
    if (pMP3 == NULL || onRead == NULL) {
        return DRMP3_FALSE;
    }

    drmp3_zero_object(pMP3);
    return drmp3_init_internal(pMP3, onRead, onSeek, pUserData, pConfig);
}


static size_t drmp3__on_read_memory(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    drmp3* pMP3 = (drmp3*)pUserData;
    drmp3_assert(pMP3 != NULL);
    drmp3_assert(pMP3->memory.dataSize >= pMP3->memory.currentReadPos);

    size_t bytesRemaining = pMP3->memory.dataSize - pMP3->memory.currentReadPos;
    if (bytesToRead > bytesRemaining) {
        bytesToRead = bytesRemaining;
    }

    if (bytesToRead > 0) {
        drmp3_copy_memory(pBufferOut, pMP3->memory.pData + pMP3->memory.currentReadPos, bytesToRead);
        pMP3->memory.currentReadPos += bytesToRead;
    }

    return bytesToRead;
}

static drmp3_bool32 drmp3__on_seek_memory(void* pUserData, int byteOffset, drmp3_seek_origin origin)
{
    drmp3* pMP3 = (drmp3*)pUserData;
    drmp3_assert(pMP3 != NULL);

    if (origin == drmp3_seek_origin_current) {
        if (byteOffset > 0) {
            if (pMP3->memory.currentReadPos + byteOffset > pMP3->memory.dataSize) {
                byteOffset = (int)(pMP3->memory.dataSize - pMP3->memory.currentReadPos);  // Trying to seek too far forward.
            }
        } else {
            if (pMP3->memory.currentReadPos < (size_t)-byteOffset) {
                byteOffset = -(int)pMP3->memory.currentReadPos;  // Trying to seek too far backwards.
            }
        }

        // This will never underflow thanks to the clamps above.
        pMP3->memory.currentReadPos += byteOffset;
    } else {
        if ((drmp3_uint32)byteOffset <= pMP3->memory.dataSize) {
            pMP3->memory.currentReadPos = byteOffset;
        } else {
            pMP3->memory.currentReadPos = pMP3->memory.dataSize;  // Trying to seek too far forward.
        }
    }

    return DRMP3_TRUE;
}

drmp3_bool32 drmp3_init_memory(drmp3* pMP3, const void* pData, size_t dataSize, const drmp3_config* pConfig)
{
    if (pMP3 == NULL) {
        return DRMP3_FALSE;
    }

    drmp3_zero_object(pMP3);

    if (pData == NULL || dataSize == 0) {
        return DRMP3_FALSE;
    }

    pMP3->memory.pData = (const drmp3_uint8*)pData;
    pMP3->memory.dataSize = dataSize;
    pMP3->memory.currentReadPos = 0;

    return drmp3_init_internal(pMP3, drmp3__on_read_memory, drmp3__on_seek_memory, pMP3, pConfig);
}


#ifndef DR_MP3_NO_STDIO
#include <stdio.h>

static size_t drmp3__on_read_stdio(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    return fread(pBufferOut, 1, bytesToRead, (FILE*)pUserData);
}

static drmp3_bool32 drmp3__on_seek_stdio(void* pUserData, int offset, drmp3_seek_origin origin)
{
    return fseek((FILE*)pUserData, offset, (origin == drmp3_seek_origin_current) ? SEEK_CUR : SEEK_SET) == 0;
}

drmp3_bool32 drmp3_init_file(drmp3* pMP3, const char* filePath, const drmp3_config* pConfig)
{
    FILE* pFile;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (fopen_s(&pFile, filePath, "rb") != 0) {
        return DRMP3_FALSE;
    }
#else
    pFile = fopen(filePath, "rb");
    if (pFile == NULL) {
        return DRMP3_FALSE;
    }
#endif

    return drmp3_init(pMP3, drmp3__on_read_stdio, drmp3__on_seek_stdio, (void*)pFile, pConfig);
}
#endif

void drmp3_uninit(drmp3* pMP3)
{
    if (pMP3 == NULL) {
        return;
    }

#ifndef DR_MP3_NO_STDIO
    if (pMP3->onRead == drmp3__on_read_stdio) {
        fclose((FILE*)pMP3->pUserData);
    }
#endif

    drmp3_free(pMP3->pData);
}

drmp3_uint64 drmp3_read_pcm_frames_f32(drmp3* pMP3, drmp3_uint64 framesToRead, float* pBufferOut)
{
    if (pMP3 == NULL || pMP3->onRead == NULL) {
        return 0;
    }

    drmp3_uint64 totalFramesRead = 0;

    if (pBufferOut == NULL) {
        float temp[4096];
        while (framesToRead > 0) {
            drmp3_uint64 framesToReadRightNow = sizeof(temp)/sizeof(temp[0]) / pMP3->channels;
            if (framesToReadRightNow > framesToRead) {
                framesToReadRightNow = framesToRead;
            }

            drmp3_uint64 framesJustRead = drmp3_read_pcm_frames_f32(pMP3, framesToReadRightNow, temp);
            if (framesJustRead == 0) {
                break;
            }

            framesToRead -= framesJustRead;
            totalFramesRead += framesJustRead;
        }
    } else {
        totalFramesRead = drmp3_src_read_frames_ex(&pMP3->src, framesToRead, pBufferOut, DRMP3_TRUE);
        pMP3->currentPCMFrame += totalFramesRead;
    }

    return totalFramesRead;
}

void drmp3_reset(drmp3* pMP3)
{
    drmp3_assert(pMP3 != NULL);

    pMP3->pcmFramesConsumedInMP3Frame = 0;
    pMP3->pcmFramesRemainingInMP3Frame = 0;
    pMP3->currentPCMFrame = 0;
    pMP3->dataSize = 0;
    pMP3->atEnd = DRMP3_FALSE;
    pMP3->src.bin[0] = 0;
    pMP3->src.bin[1] = 0;
    pMP3->src.bin[2] = 0;
    pMP3->src.bin[3] = 0;
    pMP3->src.cache.cachedFrameCount = 0;
    pMP3->src.cache.iNextFrame = 0;
    pMP3->src.algo.linear.alpha = 0;
    pMP3->src.algo.linear.isNextFramesLoaded = 0;
    pMP3->src.algo.linear.isPrevFramesLoaded = 0;
    //drmp3_zero_object(&pMP3->decoder);
    drmp3dec_init(&pMP3->decoder);
}

drmp3_bool32 drmp3_seek_to_start_of_stream(drmp3* pMP3)
{
    drmp3_assert(pMP3 != NULL);
    drmp3_assert(pMP3->onSeek != NULL);

    // Seek to the start of the stream to begin with.
    if (!drmp3__on_seek(pMP3, 0, drmp3_seek_origin_start)) {
        return DRMP3_FALSE;
    }

    // Clear any cached data.
    drmp3_reset(pMP3);
    return DRMP3_TRUE;
}

float drmp3_get_cached_pcm_frame_count_from_src(drmp3* pMP3)
{
    return (pMP3->src.cache.cachedFrameCount - pMP3->src.cache.iNextFrame) + (float)pMP3->src.algo.linear.alpha;
}

float drmp3_get_pcm_frames_remaining_in_mp3_frame(drmp3* pMP3)
{
    float factor = (float)pMP3->src.config.sampleRateOut / (float)pMP3->src.config.sampleRateIn;
    float frameCountPreSRC = drmp3_get_cached_pcm_frame_count_from_src(pMP3) + pMP3->pcmFramesRemainingInMP3Frame;
    return frameCountPreSRC * factor;
}

// NOTE ON SEEKING
// ===============
// The seeking code below is a complete mess and is broken for cases when the sample rate changes. The problem
// is with the resampling and the crappy resampler used by dr_mp3. What needs to happen is the following:
//
// 1) The resampler needs to be replaced.
// 2) The resampler has state which needs to be updated whenever an MP3 frame is decoded outside of
//    drmp3_read_pcm_frames_f32(). The resampler needs an API to "flush" some imaginary input so that it's
//    state is updated accordingly.

drmp3_bool32 drmp3_seek_forward_by_pcm_frames__brute_force(drmp3* pMP3, drmp3_uint64 frameOffset)
{
#if 0
    // MP3 is a bit annoying when it comes to seeking because of the bit reservoir. It basically means that an MP3 frame can possibly
    // depend on some of the data of prior frames. This means it's not as simple as seeking to the first byte of the MP3 frame that
    // contains the sample because that MP3 frame will need the data from the previous MP3 frame (which we just seeked past!). To
    // resolve this we seek past a number of MP3 frames up to a point, and then read-and-discard the remainder.
    drmp3_uint64 maxFramesToReadAndDiscard = (drmp3_uint64)(DRMP3_MAX_PCM_FRAMES_PER_MP3_FRAME * 3 * ((float)pMP3->src.config.sampleRateOut / (float)pMP3->src.config.sampleRateIn));

    // Now get rid of leading whole frames.
    while (frameOffset > maxFramesToReadAndDiscard) {
        float        pcmFramesRemainingInCurrentMP3FrameF = drmp3_get_pcm_frames_remaining_in_mp3_frame(pMP3);
        drmp3_uint32 pcmFramesRemainingInCurrentMP3Frame  = (drmp3_uint32)pcmFramesRemainingInCurrentMP3FrameF;
        if (frameOffset > pcmFramesRemainingInCurrentMP3Frame) {
            frameOffset                       -= pcmFramesRemainingInCurrentMP3Frame;
            pMP3->currentPCMFrame             += pcmFramesRemainingInCurrentMP3Frame;
            pMP3->pcmFramesConsumedInMP3Frame += pMP3->pcmFramesRemainingInMP3Frame;
            pMP3->pcmFramesRemainingInMP3Frame = 0;
        } else {
            break;
        }

        drmp3_uint32 pcmFrameCount = drmp3_decode_next_frame_ex(pMP3, pMP3->pcmFrames, DRMP3_FALSE);
        if (pcmFrameCount == 0) {
            break;
        }
    }

    // The last step is to read-and-discard any remaining PCM frames to make it sample-exact.
    drmp3_uint64 framesRead = drmp3_read_pcm_frames_f32(pMP3, frameOffset, NULL);
    if (framesRead != frameOffset) {
        return DRMP3_FALSE;
    }
#else
    // Just using a dumb read-and-discard for now pending updates to the resampler.
    drmp3_uint64 framesRead = drmp3_read_pcm_frames_f32(pMP3, frameOffset, NULL);
    if (framesRead != frameOffset) {
        return DRMP3_FALSE;
    }
#endif

    return DRMP3_TRUE;
}

drmp3_bool32 drmp3_seek_to_pcm_frame__brute_force(drmp3* pMP3, drmp3_uint64 frameIndex)
{
    drmp3_assert(pMP3 != NULL);

    if (frameIndex == pMP3->currentPCMFrame) {
        return DRMP3_TRUE;
    }

    // If we're moving foward we just read from where we're at. Otherwise we need to move back to the start of
    // the stream and read from the beginning.
    //drmp3_uint64 framesToReadAndDiscard;
    if (frameIndex < pMP3->currentPCMFrame) {
        // Moving backward. Move to the start of the stream and then move forward.
        if (!drmp3_seek_to_start_of_stream(pMP3)) {
            return DRMP3_FALSE;
        }
    }

    drmp3_assert(frameIndex >= pMP3->currentPCMFrame);
    return drmp3_seek_forward_by_pcm_frames__brute_force(pMP3, (frameIndex - pMP3->currentPCMFrame));
}

drmp3_bool32 drmp3_find_closest_seek_point(drmp3* pMP3, drmp3_uint64 frameIndex, drmp3_uint32* pSeekPointIndex)
{
    drmp3_assert(pSeekPointIndex != NULL);

    *pSeekPointIndex = 0;

    if (frameIndex < pMP3->pSeekPoints[0].pcmFrameIndex) {
        return DRMP3_FALSE;
    }

    // Linear search for simplicity to begin with while I'm getting this thing working. Once it's all working change this to a binary search.
    for (drmp3_uint32 iSeekPoint = 0; iSeekPoint < pMP3->seekPointCount; ++iSeekPoint) {
        if (pMP3->pSeekPoints[iSeekPoint].pcmFrameIndex > frameIndex) {
            break;  // Found it.
        }

        *pSeekPointIndex = iSeekPoint;
    }

    return DRMP3_TRUE;
}

drmp3_bool32 drmp3_seek_to_pcm_frame__seek_table(drmp3* pMP3, drmp3_uint64 frameIndex)
{
    drmp3_assert(pMP3 != NULL);
    drmp3_assert(pMP3->pSeekPoints != NULL);
    drmp3_assert(pMP3->seekPointCount > 0);

    drmp3_seek_point seekPoint;

    // If there is no prior seekpoint it means the target PCM frame comes before the first seek point. Just assume a seekpoint at the start of the file in this case.
    drmp3_uint32 priorSeekPointIndex;
    if (drmp3_find_closest_seek_point(pMP3, frameIndex, &priorSeekPointIndex)) {
        seekPoint = pMP3->pSeekPoints[priorSeekPointIndex];
    } else {
        seekPoint.seekPosInBytes     = 0;
        seekPoint.pcmFrameIndex      = 0;
        seekPoint.mp3FramesToDiscard = 0;
        seekPoint.pcmFramesToDiscard = 0;
    }

    // First thing to do is seek to the first byte of the relevant MP3 frame.
    if (!drmp3__on_seek_64(pMP3, seekPoint.seekPosInBytes, drmp3_seek_origin_start)) {
        return DRMP3_FALSE; // Failed to seek.
    }

    // Clear any cached data.
    drmp3_reset(pMP3);

    // Whole MP3 frames need to be discarded first.
    for (drmp3_uint16 iMP3Frame = 0; iMP3Frame < seekPoint.mp3FramesToDiscard; ++iMP3Frame) {
        // Pass in non-null for the last frame because we want to ensure the sample rate converter is preloaded correctly.
        drmp3d_sample_t* pPCMFrames = NULL;
        if (iMP3Frame == seekPoint.mp3FramesToDiscard-1) {
            pPCMFrames = (drmp3d_sample_t*)pMP3->pcmFrames;
        }

        // We first need to decode the next frame, and then we need to flush the resampler.
        drmp3_uint32 pcmFramesReadPreSRC = drmp3_decode_next_frame_ex(pMP3, pPCMFrames, DRMP3_TRUE);
        if (pcmFramesReadPreSRC == 0) {
            return DRMP3_FALSE;
        }
    }

    // We seeked to an MP3 frame in the raw stream so we need to make sure the current PCM frame is set correctly.
    pMP3->currentPCMFrame = seekPoint.pcmFrameIndex - seekPoint.pcmFramesToDiscard;

    // Update resampler. This is wrong. Need to instead update it on a per MP3 frame basis. Also broken for cases when
    // the sample rate is being reduced in my testing. Should work fine when the input and output sample rate is the same
    // or a clean multiple.
    pMP3->src.algo.linear.alpha = pMP3->currentPCMFrame * ((double)pMP3->src.config.sampleRateIn / pMP3->src.config.sampleRateOut);
    pMP3->src.algo.linear.alpha = pMP3->src.algo.linear.alpha - (drmp3_uint32)(pMP3->src.algo.linear.alpha);
    if (pMP3->src.algo.linear.alpha > 0) {
        pMP3->src.algo.linear.isPrevFramesLoaded = 1;
    }

    // Now at this point we can follow the same process as the brute force technique where we just skip over unnecessary MP3 frames and then
    // read-and-discard at least 2 whole MP3 frames.
    drmp3_uint64 leftoverFrames = frameIndex - pMP3->currentPCMFrame;
    return drmp3_seek_forward_by_pcm_frames__brute_force(pMP3, leftoverFrames);
}

drmp3_bool32 drmp3_seek_to_pcm_frame(drmp3* pMP3, drmp3_uint64 frameIndex)
{
    if (pMP3 == NULL || pMP3->onSeek == NULL) {
        return DRMP3_FALSE;
    }

    if (frameIndex == 0) {
        return drmp3_seek_to_start_of_stream(pMP3);
    }

    // Use the seek table if we have one.
    if (pMP3->pSeekPoints != NULL && pMP3->seekPointCount > 0) {
        return drmp3_seek_to_pcm_frame__seek_table(pMP3, frameIndex);
    } else {
        return drmp3_seek_to_pcm_frame__brute_force(pMP3, frameIndex);
    }
}

drmp3_bool32 drmp3_get_mp3_and_pcm_frame_count(drmp3* pMP3, drmp3_uint64* pMP3FrameCount, drmp3_uint64* pPCMFrameCount)
{
    if (pMP3 == NULL) {
        return DRMP3_FALSE;
    }

    // The way this works is we move back to the start of the stream, iterate over each MP3 frame and calculate the frame count based
    // on our output sample rate, the seek back to the PCM frame we were sitting on before calling this function.

    // The stream must support seeking for this to work.
    if (pMP3->onSeek == NULL) {
        return DRMP3_FALSE;
    }

    // We'll need to seek back to where we were, so grab the PCM frame we're currently sitting on so we can restore later.
    drmp3_uint64 currentPCMFrame = pMP3->currentPCMFrame;

    if (!drmp3_seek_to_start_of_stream(pMP3)) {
        return DRMP3_FALSE;
    }

    drmp3_uint64 totalPCMFrameCount = 0;
    drmp3_uint64 totalMP3FrameCount = 0;

    float totalPCMFrameCountFractionalPart = 0; // <-- With resampling there will be a fractional part to each MP3 frame that we need to accumulate.
    for (;;) {
        drmp3_uint32 pcmFramesInCurrentMP3FrameIn = drmp3_decode_next_frame_ex(pMP3, NULL, DRMP3_FALSE);
        if (pcmFramesInCurrentMP3FrameIn == 0) {
            break;
        }

        float srcRatio = (float)pMP3->mp3FrameSampleRate / (float)pMP3->sampleRate;
        drmp3_assert(srcRatio > 0);

        float        pcmFramesInCurrentMP3FrameOutF = totalPCMFrameCountFractionalPart + (pcmFramesInCurrentMP3FrameIn / srcRatio);
        drmp3_uint32 pcmFramesInCurrentMP3FrameOut  = (drmp3_uint32)pcmFramesInCurrentMP3FrameOutF;
        totalPCMFrameCountFractionalPart = pcmFramesInCurrentMP3FrameOutF - pcmFramesInCurrentMP3FrameOut;
        totalPCMFrameCount += pcmFramesInCurrentMP3FrameOut;
        totalMP3FrameCount += 1;
    }

    // Finally, we need to seek back to where we were.
    if (!drmp3_seek_to_start_of_stream(pMP3)) {
        return DRMP3_FALSE;
    }

    if (!drmp3_seek_to_pcm_frame(pMP3, currentPCMFrame)) {
        return DRMP3_FALSE;
    }

    if (pMP3FrameCount != NULL) {
        *pMP3FrameCount = totalMP3FrameCount;
    }
    if (pPCMFrameCount != NULL) {
        *pPCMFrameCount = totalPCMFrameCount;
    }

    return DRMP3_TRUE;
}

drmp3_uint64 drmp3_get_pcm_frame_count(drmp3* pMP3)
{
    drmp3_uint64 totalPCMFrameCount;
    if (!drmp3_get_mp3_and_pcm_frame_count(pMP3, NULL, &totalPCMFrameCount)) {
        return 0;
    }

    return totalPCMFrameCount;
}

drmp3_uint64 drmp3_get_mp3_frame_count(drmp3* pMP3)
{
    drmp3_uint64 totalMP3FrameCount;
    if (!drmp3_get_mp3_and_pcm_frame_count(pMP3, &totalMP3FrameCount, NULL)) {
        return 0;
    }

    return totalMP3FrameCount;
}

void drmp3__accumulate_running_pcm_frame_count(drmp3* pMP3, drmp3_uint32 pcmFrameCountIn, drmp3_uint64* pRunningPCMFrameCount, float* pRunningPCMFrameCountFractionalPart)
{
    float srcRatio = (float)pMP3->mp3FrameSampleRate / (float)pMP3->sampleRate;
    drmp3_assert(srcRatio > 0);

    float        pcmFrameCountOutF = *pRunningPCMFrameCountFractionalPart + (pcmFrameCountIn / srcRatio);
    drmp3_uint32 pcmFrameCountOut  = (drmp3_uint32)pcmFrameCountOutF;
    *pRunningPCMFrameCountFractionalPart = pcmFrameCountOutF - pcmFrameCountOut;
    *pRunningPCMFrameCount += pcmFrameCountOut;
}

typedef struct
{
    drmp3_uint64 bytePos;
    drmp3_uint64 pcmFrameIndex; // <-- After sample rate conversion.
} drmp3__seeking_mp3_frame_info;

drmp3_bool32 drmp3_calculate_seek_points(drmp3* pMP3, drmp3_uint32* pSeekPointCount, drmp3_seek_point* pSeekPoints)
{
    if (pMP3 == NULL || pSeekPointCount == NULL || pSeekPoints == NULL) {
        return DRMP3_FALSE; // Invalid args.
    }

    drmp3_uint32 seekPointCount = *pSeekPointCount;
    if (seekPointCount == 0) {
        return DRMP3_FALSE;  // The client has requested no seek points. Consider this to be invalid arguments since the client has probably not intended this.
    }

    // We'll need to seek back to the current sample after calculating the seekpoints so we need to go ahead and grab the current location at the top.
    drmp3_uint64 currentPCMFrame = pMP3->currentPCMFrame;

    // We never do more than the total number of MP3 frames and we limit it to 32-bits.
    drmp3_uint64 totalMP3FrameCount;
    drmp3_uint64 totalPCMFrameCount;
    if (!drmp3_get_mp3_and_pcm_frame_count(pMP3, &totalMP3FrameCount, &totalPCMFrameCount)) {
        return DRMP3_FALSE;
    }

    // If there's less than DRMP3_SEEK_LEADING_MP3_FRAMES+1 frames we just report 1 seek point which will be the very start of the stream.
    if (totalMP3FrameCount < DRMP3_SEEK_LEADING_MP3_FRAMES+1) {
        seekPointCount = 1;
        pSeekPoints[0].seekPosInBytes     = 0;
        pSeekPoints[0].pcmFrameIndex      = 0;
        pSeekPoints[0].mp3FramesToDiscard = 0;
        pSeekPoints[0].pcmFramesToDiscard = 0;
    } else {
        if (seekPointCount > totalMP3FrameCount-1) {
            seekPointCount = (drmp3_uint32)totalMP3FrameCount-1;
        }

        drmp3_uint64 pcmFramesBetweenSeekPoints = totalPCMFrameCount / (seekPointCount+1);

        // Here is where we actually calculate the seek points. We need to start by moving the start of the stream. We then enumerate over each
        // MP3 frame.
        if (!drmp3_seek_to_start_of_stream(pMP3)) {
            return DRMP3_FALSE;
        }

        // We need to cache the byte positions of the previous MP3 frames. As a new MP3 frame is iterated, we cycle the byte positions in this
        // array. The value in the first item in this array is the byte position that will be reported in the next seek point.
        drmp3__seeking_mp3_frame_info mp3FrameInfo[DRMP3_SEEK_LEADING_MP3_FRAMES+1];

        drmp3_uint64 runningPCMFrameCount = 0;
        float runningPCMFrameCountFractionalPart = 0;

        // We need to initialize the array of MP3 byte positions for the leading MP3 frames.
        for (int iMP3Frame = 0; iMP3Frame < DRMP3_SEEK_LEADING_MP3_FRAMES+1; ++iMP3Frame) {
            // The byte position of the next frame will be the stream's cursor position, minus whatever is sitting in the buffer.
            drmp3_assert(pMP3->streamCursor >= pMP3->dataSize);
            mp3FrameInfo[iMP3Frame].bytePos       = pMP3->streamCursor - pMP3->dataSize;
            mp3FrameInfo[iMP3Frame].pcmFrameIndex = runningPCMFrameCount;

            // We need to get information about this frame so we can know how many samples it contained.
            drmp3_uint32 pcmFramesInCurrentMP3FrameIn = drmp3_decode_next_frame_ex(pMP3, NULL, DRMP3_FALSE);
            if (pcmFramesInCurrentMP3FrameIn == 0) {
                return DRMP3_FALSE; // This should never happen.
            }

            drmp3__accumulate_running_pcm_frame_count(pMP3, pcmFramesInCurrentMP3FrameIn, &runningPCMFrameCount, &runningPCMFrameCountFractionalPart);
        }

        // At this point we will have extracted the byte positions of the leading MP3 frames. We can now start iterating over each seek point and
        // calculate them.
        drmp3_uint64 nextTargetPCMFrame = 0;
        for (drmp3_uint32 iSeekPoint = 0; iSeekPoint < seekPointCount; ++iSeekPoint) {
            nextTargetPCMFrame += pcmFramesBetweenSeekPoints;

            for (;;) {
                if (nextTargetPCMFrame < runningPCMFrameCount) {
                    // The next seek point is in the current MP3 frame.
                    pSeekPoints[iSeekPoint].seekPosInBytes     = mp3FrameInfo[0].bytePos;
                    pSeekPoints[iSeekPoint].pcmFrameIndex      = nextTargetPCMFrame;
                    pSeekPoints[iSeekPoint].mp3FramesToDiscard = DRMP3_SEEK_LEADING_MP3_FRAMES;
                    pSeekPoints[iSeekPoint].pcmFramesToDiscard = (drmp3_uint16)(nextTargetPCMFrame - mp3FrameInfo[DRMP3_SEEK_LEADING_MP3_FRAMES-1].pcmFrameIndex);
                    break;
                } else {
                    // The next seek point is not in the current MP3 frame, so continue on to the next one. The first thing to do is cycle the cached
                    // MP3 frame info.
                    for (size_t i = 0; i < drmp3_countof(mp3FrameInfo)-1; ++i) {
                        mp3FrameInfo[i] = mp3FrameInfo[i+1];
                    }

                    // Cache previous MP3 frame info.
                    mp3FrameInfo[drmp3_countof(mp3FrameInfo)-1].bytePos       = pMP3->streamCursor - pMP3->dataSize;
                    mp3FrameInfo[drmp3_countof(mp3FrameInfo)-1].pcmFrameIndex = runningPCMFrameCount;

                    // Go to the next MP3 frame. This shouldn't ever fail, but just in case it does we just set the seek point and break. If it happens, it
                    // should only ever do it for the last seek point.
                    drmp3_uint32 pcmFramesInCurrentMP3FrameIn = drmp3_decode_next_frame_ex(pMP3, NULL, DRMP3_TRUE);
                    if (pcmFramesInCurrentMP3FrameIn == 0) {
                        pSeekPoints[iSeekPoint].seekPosInBytes     = mp3FrameInfo[0].bytePos;
                        pSeekPoints[iSeekPoint].pcmFrameIndex      = nextTargetPCMFrame;
                        pSeekPoints[iSeekPoint].mp3FramesToDiscard = DRMP3_SEEK_LEADING_MP3_FRAMES;
                        pSeekPoints[iSeekPoint].pcmFramesToDiscard = (drmp3_uint16)(nextTargetPCMFrame - mp3FrameInfo[DRMP3_SEEK_LEADING_MP3_FRAMES-1].pcmFrameIndex);
                        break;
                    }

                    drmp3__accumulate_running_pcm_frame_count(pMP3, pcmFramesInCurrentMP3FrameIn, &runningPCMFrameCount, &runningPCMFrameCountFractionalPart);
                }
            }
        }

        // Finally, we need to seek back to where we were.
        if (!drmp3_seek_to_start_of_stream(pMP3)) {
            return DRMP3_FALSE;
        }
        if (!drmp3_seek_to_pcm_frame(pMP3, currentPCMFrame)) {
            return DRMP3_FALSE;
        }
    }

    *pSeekPointCount = seekPointCount;
    return DRMP3_TRUE;
}

drmp3_bool32 drmp3_bind_seek_table(drmp3* pMP3, drmp3_uint32 seekPointCount, drmp3_seek_point* pSeekPoints)
{
    if (pMP3 == NULL) {
        return DRMP3_FALSE;
    }

    if (seekPointCount == 0 || pSeekPoints == NULL) {
        // Unbinding.
        pMP3->seekPointCount = 0;
        pMP3->pSeekPoints = NULL;
    } else {
        // Binding.
        pMP3->seekPointCount = seekPointCount;
        pMP3->pSeekPoints = pSeekPoints;
    }

    return DRMP3_TRUE;
}


float* drmp3__full_read_and_close_f32(drmp3* pMP3, drmp3_config* pConfig, drmp3_uint64* pTotalFrameCount)
{
    drmp3_assert(pMP3 != NULL);

    drmp3_uint64 totalFramesRead = 0;
    drmp3_uint64 framesCapacity = 0;
    float* pFrames = NULL;

    float temp[4096];
    for (;;) {
        drmp3_uint64 framesToReadRightNow = drmp3_countof(temp) / pMP3->channels;
        drmp3_uint64 framesJustRead = drmp3_read_pcm_frames_f32(pMP3, framesToReadRightNow, temp);
        if (framesJustRead == 0) {
            break;
        }

        // Reallocate the output buffer if there's not enough room.
        if (framesCapacity < totalFramesRead + framesJustRead) {
            framesCapacity *= 2;
            if (framesCapacity < totalFramesRead + framesJustRead) {
                framesCapacity = totalFramesRead + framesJustRead;
            }

            drmp3_uint64 newFramesBufferSize = framesCapacity*pMP3->channels*sizeof(float);
            if (newFramesBufferSize > DRMP3_SIZE_MAX) {
                break;
            }

            float* pNewFrames = (float*)drmp3_realloc(pFrames, (size_t)newFramesBufferSize);
            if (pNewFrames == NULL) {
                drmp3_free(pFrames);
                break;
            }

            pFrames = pNewFrames;
        }

        drmp3_copy_memory(pFrames + totalFramesRead*pMP3->channels, temp, (size_t)(framesJustRead*pMP3->channels*sizeof(float)));
        totalFramesRead += framesJustRead;

        // If the number of frames we asked for is less that what we actually read it means we've reached the end.
        if (framesJustRead != framesToReadRightNow) {
            break;
        }
    }

    if (pConfig != NULL) {
        pConfig->outputChannels = pMP3->channels;
        pConfig->outputSampleRate = pMP3->sampleRate;
    }

    drmp3_uninit(pMP3);

    if (pTotalFrameCount) *pTotalFrameCount = totalFramesRead;
    return pFrames;
}

float* drmp3_open_and_read_f32(drmp3_read_proc onRead, drmp3_seek_proc onSeek, void* pUserData, drmp3_config* pConfig, drmp3_uint64* pTotalFrameCount)
{
    drmp3 mp3;
    if (!drmp3_init(&mp3, onRead, onSeek, pUserData, pConfig)) {
        return NULL;
    }

    return drmp3__full_read_and_close_f32(&mp3, pConfig, pTotalFrameCount);
}

float* drmp3_open_memory_and_read_f32(const void* pData, size_t dataSize, drmp3_config* pConfig, drmp3_uint64* pTotalFrameCount)
{
    drmp3 mp3;
    if (!drmp3_init_memory(&mp3, pData, dataSize, pConfig)) {
        return NULL;
    }

    return drmp3__full_read_and_close_f32(&mp3, pConfig, pTotalFrameCount);
}

#ifndef DR_MP3_NO_STDIO
float* drmp3_open_file_and_read_f32(const char* filePath, drmp3_config* pConfig, drmp3_uint64* pTotalFrameCount)
{
    drmp3 mp3;
    if (!drmp3_init_file(&mp3, filePath, pConfig)) {
        return NULL;
    }

    return drmp3__full_read_and_close_f32(&mp3, pConfig, pTotalFrameCount);
}
#endif

void drmp3_free(void* p)
{
    DRMP3_FREE(p);
}

#endif /*DR_MP3_IMPLEMENTATION*/


// DIFFERENCES BETWEEN minimp3 AND dr_mp3
// ======================================
// - First, keep in mind that minimp3 (https://github.com/lieff/minimp3) is where all the real work was done. All of the
//   code relating to the actual decoding remains mostly unmodified, apart from some namespacing changes.
// - dr_mp3 adds a pulling style API which allows you to deliver raw data via callbacks. So, rather than pushing data
//   to the decoder, the decoder _pulls_ data from your callbacks.
// - In addition to callbacks, a decoder can be initialized from a block of memory and a file.
// - The dr_mp3 pull API reads PCM frames rather than whole MP3 frames.
// - dr_mp3 adds convenience APIs for opening and decoding entire files in one go.
// - dr_mp3 is fully namespaced, including the implementation section, which is more suitable when compiling projects
//   as a single translation unit (aka unity builds). At the time of writing this, a unity build is not possible when
//   using minimp3 in conjunction with stb_vorbis. dr_mp3 addresses this.


// REVISION HISTORY
// ================
//
// v0.4.2 - 2018-xx-xx
//   - Fix a warning.
//
// v0.4.1 - 2018-12-30
//   - Fix a warning.
//
// v0.4.0 - 2018-12-16
//   - API CHANGE: Rename some APIs:
//     - drmp3_read_f32 -> to drmp3_read_pcm_frames_f32
//     - drmp3_seek_to_frame -> drmp3_seek_to_pcm_frame
//     - drmp3_open_and_decode_f32 -> drmp3_open_and_read_f32
//     - drmp3_open_and_decode_memory_f32 -> drmp3_open_memory_and_read_f32
//     - drmp3_open_and_decode_file_f32 -> drmp3_open_file_and_read_f32
//   - Add drmp3_get_pcm_frame_count().
//   - Add drmp3_get_mp3_frame_count().
//   - Improve seeking performance.
//
// v0.3.2 - 2018-09-11
//   - Fix a couple of memory leaks.
//   - Bring up to date with minimp3.
//
// v0.3.1 - 2018-08-25
//   - Fix C++ build.
//
// v0.3.0 - 2018-08-25
//   - Bring up to date with minimp3. This has a minor API change: the "pcm" parameter of drmp3dec_decode_frame() has
//     been changed from short* to void* because it can now output both s16 and f32 samples, depending on whether or
//     not the DR_MP3_FLOAT_OUTPUT option is set.
//
// v0.2.11 - 2018-08-08
//   - Fix a bug where the last part of a file is not read.
//
// v0.2.10 - 2018-08-07
//   - Improve 64-bit detection.
//
// v0.2.9 - 2018-08-05
//   - Fix C++ build on older versions of GCC.
//   - Bring up to date with minimp3.
//
// v0.2.8 - 2018-08-02
//   - Fix compilation errors with older versions of GCC.
//
// v0.2.7 - 2018-07-13
//   - Bring up to date with minimp3.
//
// v0.2.6 - 2018-07-12
//   - Bring up to date with minimp3.
//
// v0.2.5 - 2018-06-22
//   - Bring up to date with minimp3.
//
// v0.2.4 - 2018-05-12
//   - Bring up to date with minimp3.
//
// v0.2.3 - 2018-04-29
//   - Fix TCC build.
//
// v0.2.2 - 2018-04-28
//   - Fix bug when opening a decoder from memory.
//
// v0.2.1 - 2018-04-27
//   - Efficiency improvements when the decoder reaches the end of the stream.
//
// v0.2 - 2018-04-21
//   - Bring up to date with minimp3.
//   - Start using major.minor.revision versioning.
//
// v0.1d - 2018-03-30
//   - Bring up to date with minimp3.
//
// v0.1c - 2018-03-11
//   - Fix C++ build error.
//
// v0.1b - 2018-03-07
//   - Bring up to date with minimp3.
//
// v0.1a - 2018-02-28
//   - Fix compilation error on GCC/Clang.
//   - Fix some warnings.
//
// v0.1 - 2018-02-xx
//   - Initial versioned release.


/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

/*
    https://github.com/lieff/minimp3
    To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide.
    This software is distributed without any warranty.
    See <http://creativecommons.org/publicdomain/zero/1.0/>.
*/
