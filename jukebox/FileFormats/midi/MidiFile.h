// Amalgamation of source code from https://github.com/craigsapp/midifile

//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Website:       http://midifile.sapp.org
// Syntax:        C++11
// vim:           ts=3 noexpandtab
//

#include <vector>
#include <string>
#include <istream>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <map>
#include <cctype>
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <iomanip>
#include <stdlib.h> /* needed for MinGW */

#define TIME_STATE_DELTA       0
#define TIME_STATE_ABSOLUTE    1

#define TRACK_STATE_SPLIT      0
#define TRACK_STATE_JOINED     1

#define OPTION_BOOLEAN_TYPE   'b'
#define OPTION_CHAR_TYPE      'c'
#define OPTION_DOUBLE_TYPE    'd'
#define OPTION_FLOAT_TYPE     'f'
#define OPTION_INT_TYPE       'i'
#define OPTION_STRING_TYPE    's'
#define OPTION_UNKNOWN_TYPE   'x'

namespace smf {
	class MidiFile;
}
std::ostream& operator<<(std::ostream& out, smf::MidiFile& aMidiFile);

namespace smf {

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;

class MidiMessage : public std::vector<uchar> {

	public:
		               MidiMessage          (void);
		               MidiMessage          (int command);
		               MidiMessage          (int command, int p1);
		               MidiMessage          (int command, int p1, int p2);
		               MidiMessage          (const MidiMessage& message);
		               MidiMessage          (const std::vector<uchar>& message);
		               MidiMessage          (const std::vector<char>& message);
		               MidiMessage          (const std::vector<int>& message);

		              ~MidiMessage          ();

		MidiMessage&   operator=            (const MidiMessage& message);
		MidiMessage&   operator=            (const std::vector<uchar>& bytes);
		MidiMessage&   operator=            (const std::vector<char>& bytes);
		MidiMessage&   operator=            (const std::vector<int>& bytes);

		void           sortTrack            (void);
		void           sortTrackWithSequence(void);

		// data access convenience functions (returns -1 if not present):
		int            getP0                (void) const;
		int            getP1                (void) const;
		int            getP2                (void) const;
		int            getP3                (void) const;
		void           setP0                (int value);
		void           setP1                (int value);
		void           setP2                (int value);
		void           setP3                (int value);

		int            getSize              (void) const;
		void           setSize              (int asize);
		int            setSizeToCommand     (void);
		int            resizeToCommand      (void);

		// note-message convenience functions:
		int            getKeyNumber         (void) const;
		int            getVelocity          (void) const;
		void           setKeyNumber         (int value);
		void           setVelocity          (int value);
		void           setSpelling          (int base7, int accidental);
		void           getSpelling          (int& base7, int& accidental);

		// controller-message convenience functions:
		int            getControllerNumber  (void) const;
		int            getControllerValue   (void) const;

		int            getCommandNibble     (void) const;
		int            getCommandByte       (void) const;
		int            getChannelNibble     (void) const;
		int            getChannel           (void) const;

		void           setCommandByte       (int value);
		void           setCommand           (int value);
		void           setCommand           (int value, int p1);
		void           setCommand           (int value, int p1, int p2);
		void           setCommandNibble     (int value);
		void           setChannelNibble     (int value);
		void           setChannel           (int value);
		void           setParameters        (int p1, int p2);
		void           setParameters        (int p1);
		void           setMessage           (const std::vector<uchar>& message);
		void           setMessage           (const std::vector<char>& message);
		void           setMessage           (const std::vector<int>& message);

		// message-type convenience functions:
		bool           isMetaMessage        (void) const;
		bool           isMeta               (void) const;
		bool           isNoteOff            (void) const;
		bool           isNoteOn             (void) const;
		bool           isNote               (void) const;
		bool           isAftertouch         (void) const;
		bool           isController         (void) const;
		bool           isTimbre             (void) const;
		bool           isPatchChange        (void) const;
		bool           isPressure           (void) const;
		bool           isPitchbend          (void) const;
		bool           isEmpty              (void) const;

		// helper functions to create various MidiMessages:
		void           makeNoteOn           (int channel, int key, int velocity);
		void           makeNoteOff          (int channel, int key, int velocity);
		void           makeNoteOff          (int channel, int key);
		void           makeNoteOff          (void);
		void           makePatchChange      (int channel, int patchnum);
		void           makeTimbre           (int channel, int patchnum);
		void           makeController       (int channel, int num, int value);

		// helper functions to create various continuous controller messages:
		void           makeSustain          (int channel, int value);
		void           makeSustainPedal     (int channel, int value);
		void           makeSustainOn        (int channel);
		void           makeSustainPedalOn   (int channel);
		void           makeSustainOff       (int channel);
		void           makeSustainPedalOff  (int channel);

		// meta-message creation and helper functions:
		void           makeMetaMessage      (int mnum, const std::string& data);
		void           makeText             (const std::string& name);
		void           makeCopyright        (const std::string& text);
		void           makeTrackName        (const std::string& name);
		void           makeInstrumentName   (const std::string& name);
		void           makeLyric            (const std::string& text);
		void           makeMarker           (const std::string& text);
		void           makeCue              (const std::string& text);
		void           makeTimeSignature    (int top, int bottom,
		                                     int clocksPerClick = 24,
		                                     int num32dsPerQuarter = 8);

		void           makeTempo            (double tempo) { setTempo(tempo); }
		int            getTempoMicro        (void) const;
		int            getTempoMicroseconds (void) const;
		double         getTempoSeconds      (void) const;
		double         getTempoBPM          (void) const;
		double         getTempoTPS          (int tpq) const;
		double         getTempoSPT          (int tpq) const;

		int            getMetaType          (void) const;
		bool           isText               (void) const;
		bool           isCopyright          (void) const;
		bool           isTrackName          (void) const;
		bool           isInstrumentName     (void) const;
		bool           isLyricText          (void) const;
		bool           isMarkerText         (void) const;
		bool           isTempo              (void) const;
		bool           isTimeSignature      (void) const;
		bool           isKeySignature       (void) const;
		bool           isEndOfTrack         (void) const;

		std::string    getMetaContent       (void);
		void           setMetaContent       (const std::string& content);
		void           setTempo             (double tempo);
		void           setTempoMicroseconds (int microseconds);
		void           setMetaTempo         (double tempo);

};

class MidiEvent : public MidiMessage {
	public:
		           MidiEvent             (void);
		           MidiEvent             (int command);
		           MidiEvent             (int command, int param1);
		           MidiEvent             (int command, int param1, int param2);
		           MidiEvent             (const MidiMessage& message);
		           MidiEvent             (const MidiEvent& mfevent);
		           MidiEvent             (int aTime, int aTrack,
		                                  std::vector<uchar>& message);

		          ~MidiEvent             ();

		MidiEvent& operator=             (const MidiEvent& mfevent);
		MidiEvent& operator=             (const MidiMessage& message);
		MidiEvent& operator=             (const std::vector<uchar>& bytes);
		MidiEvent& operator=             (const std::vector<char>& bytes);
		MidiEvent& operator=             (const std::vector<int>& bytes);

		void       clearVariables        (void);

		// functions related to event linking (note-ons to note-offs).
		void       unlinkEvent           (void);
		void       unlinkEvents          (void);
		void       linkEvent             (MidiEvent* mev);
		void       linkEvents            (MidiEvent* mev);
		void       linkEvent             (MidiEvent& mev);
		void       linkEvents            (MidiEvent& mev);
		int        isLinked              (void) const;
		MidiEvent* getLinkedEvent        (void);
		const MidiEvent* getLinkedEvent  (void) const;
		int        getTickDuration       (void) const;
		double     getDurationInSeconds  (void) const;

		int        tick;     // delta or absolute MIDI ticks
		int        track;    // [original] track number of event in MIDI file
		double     seconds;  // calculated time in sec. (after doTimeAnalysis())
		int        seq;      // sorting sequence number of event

	private:
		MidiEvent* m_eventlink;  // used to match note-ons and note-offs

};

class MidiEventList {
	public:
		                 MidiEventList      (void);
		                 MidiEventList      (const MidiEventList& other);
		                 MidiEventList      (MidiEventList&& other);

		                ~MidiEventList      ();

		MidiEventList&   operator=          (MidiEventList& other);
		MidiEvent&       operator[]         (int index);
		const MidiEvent& operator[]         (int index) const;

		MidiEvent&       back               (void);
		const MidiEvent& back               (void) const;
		MidiEvent&       last               (void);
		const MidiEvent& last               (void) const;
		MidiEvent&       getEvent           (int index);
		const MidiEvent& getEvent           (int index) const;
		void             clear              (void);
		void             reserve            (int rsize);
		int              getEventCount      (void) const;
		int              getSize            (void) const;
		int              size               (void) const;
		void             removeEmpties      (void);
		int              linkNotePairs      (void);
		int              linkEventPairs     (void);
		void             clearLinks         (void);
		void             clearSequence      (void);
		int              markSequence       (int sequence = 1);

		int              push               (MidiEvent& event);
		int              push_back          (MidiEvent& event);
		int              append             (MidiEvent& event);

		// careful when using these, intended for internal use in MidiFile class:
		void             detach             (void);
		int              push_back_no_copy  (MidiEvent* event);

		// access to the list of MidiEvents for sorting with an external function:
		MidiEvent**      data               (void);

	protected:
		std::vector<MidiEvent*> list;

	private:
		void             sort                (void);

	// MidiFile class calls sort()
	friend class MidiFile;
};

int eventcompare(const void* a, const void* b);

class _TickTime {
	public:
		int    tick;
		double seconds;
};

class MidiFile {
	public:
		               MidiFile                    (void);
		               MidiFile                    (const std::string& filename);
		               MidiFile                    (std::istream& input);
		               MidiFile                    (const MidiFile& other);
		               MidiFile                    (MidiFile&& other);

		              ~MidiFile                    ();

		MidiFile&      operator=                   (const MidiFile& other);
		MidiFile&      operator=                   (MidiFile&& other);

		// reading/writing functions:
		bool           read                        (const std::string& filename);
		bool           read                        (std::istream& instream);
		bool           write                       (const std::string& filename);
		bool           write                       (std::ostream& out);
		bool           writeHex                    (const std::string& filename,
		                                            int width = 25);
		bool           writeHex                    (std::ostream& out,
		                                            int width = 25);
		bool           writeBinasc                 (const std::string& filename);
		bool           writeBinasc                 (std::ostream& out);
		bool           writeBinascWithComments     (const std::string& filename);
		bool           writeBinascWithComments     (std::ostream& out);
		bool           status                      (void) const;

		// track-related functions:
		const MidiEventList& operator[]            (int aTrack) const;
		MidiEventList&   operator[]                (int aTrack);
		int              getTrackCount             (void) const;
		int              getNumTracks              (void) const;
		int              size                      (void) const;
		void             removeEmpties             (void);

		// tick-related functions:
		void             makeDeltaTicks            (void);
		void             deltaTicks                (void);
		void             makeAbsoluteTicks         (void);
		void             absoluteTicks             (void);
		int              getTickState              (void) const;
		bool             isDeltaTicks              (void) const;
		bool             isAbsoluteTicks           (void) const;

		// join/split track functionality:
		void             joinTracks                (void);
		void             splitTracks               (void);
		void             splitTracksByChannel      (void);
		int              getTrackState             (void) const;
		int              hasJoinedTracks           (void) const;
		int              hasSplitTracks            (void) const;
		int              getSplitTrack             (int track, int index) const;
		int              getSplitTrack             (int index) const;

		// track sorting funcionality:
		void             sortTrack                 (int track);
		void             sortTracks                (void);
		void             markSequence              (void);
		void             markSequence              (int track, int sequence = 1);
		void             clearSequence             (void);
		void             clearSequence             (int track);

		// track manipulation functionality:
		int              addTrack                  (void);
		int              addTrack                  (int count);
		int              addTracks                 (int count);
		void             deleteTrack               (int aTrack);
		void             mergeTracks               (int aTrack1, int aTrack2);
		int              getTrackCountAsType1      (void);

		// ticks-per-quarter related functions:
		void             setMillisecondTicks       (void);
		int              getTicksPerQuarterNote    (void) const;
		int              getTPQ                    (void) const;
		void             setTicksPerQuarterNote    (int ticks);
		void             setTPQ                    (int ticks);

		// physical-time analysis functions:
		void             doTimeAnalysis            (void);
		double           getTimeInSeconds          (int aTrack, int anIndex);
		double           getTimeInSeconds          (int tickvalue);
		double           getAbsoluteTickTime       (double starttime);
		int              getFileDurationInTicks    (void);
		double           getFileDurationInQuarters (void);
		double           getFileDurationInSeconds  (void);

		// note-analysis functions:
		int              linkNotePairs             (void);
		int              linkEventPairs            (void);
		void             clearLinks                (void);

		// filename functions:
		void             setFilename               (const std::string& aname);
		const char*      getFilename               (void) const;

		// event functionality:
		MidiEvent*       addEvent                  (int aTrack, int aTick,
		                                            std::vector<uchar>& midiData);
		MidiEvent*       addEvent                  (MidiEvent& mfevent);
		MidiEvent*       addEvent                  (int aTrack, MidiEvent& mfevent);
		MidiEvent&       getEvent                  (int aTrack, int anIndex);
		const MidiEvent& getEvent                  (int aTrack, int anIndex) const;
		int              getEventCount             (int aTrack) const;
		int              getNumEvents              (int aTrack) const;
		void             allocateEvents            (int track, int aSize);
		void             erase                     (void);
		void             clear                     (void);
		void             clear_no_deallocate       (void);

		// MIDI message adding convenience functions:
		MidiEvent*        addNoteOn               (int aTrack, int aTick,
		                                           int aChannel, int key,
		                                           int vel);
		MidiEvent*        addNoteOff              (int aTrack, int aTick,
		                                           int aChannel, int key,
		                                           int vel);
		MidiEvent*        addNoteOff              (int aTrack, int aTick,
		                                           int aChannel, int key);
		MidiEvent*        addController           (int aTrack, int aTick,
		                                           int aChannel, int num,
		                                           int value);
		MidiEvent*        addPatchChange          (int aTrack, int aTick,
		                                           int aChannel, int patchnum);
		MidiEvent*        addTimbre               (int aTrack, int aTick,
		                                           int aChannel, int patchnum);
		MidiEvent*        addPitchBend            (int aTrack, int aTick,
		                                           int aChannel, double amount);

		// Controller message adding convenience functions:
		MidiEvent*        addSustain              (int aTrack, int aTick,
		                                           int aChannel, int value);
		MidiEvent*        addSustainPedal         (int aTrack, int aTick,
		                                           int aChannel, int value);
		MidiEvent*        addSustainOn            (int aTrack, int aTick,
		                                           int aChannel);
		MidiEvent*        addSustainPedalOn       (int aTrack, int aTick,
		                                           int aChannel);
		MidiEvent*        addSustainOff           (int aTrack, int aTick,
		                                           int aChannel);
		MidiEvent*        addSustainPedalOff      (int aTrack, int aTick,
		                                           int aChannel);

		// Meta-event adding convenience functions:
		MidiEvent*         addMetaEvent           (int aTrack, int aTick,
		                                           int aType,
		                                           std::vector<uchar>& metaData);
		MidiEvent*         addMetaEvent           (int aTrack, int aTick,
		                                           int aType,
		                                           const std::string& metaData);
		MidiEvent*         addText                (int aTrack, int aTick,
		                                           const std::string& text);
		MidiEvent*         addCopyright           (int aTrack, int aTick,
		                                           const std::string& text);
		MidiEvent*         addTrackName           (int aTrack, int aTick,
		                                           const std::string& name);
		MidiEvent*         addInstrumentName      (int aTrack, int aTick,
		                                           const std::string& name);
		MidiEvent*         addLyric               (int aTrack, int aTick,
		                                           const std::string& text);
		MidiEvent*         addMarker              (int aTrack, int aTick,
		                                           const std::string& text);
		MidiEvent*         addCue                 (int aTrack, int aTick,
		                                           const std::string& text);
		MidiEvent*         addTempo               (int aTrack, int aTick,
		                                           double aTempo);
		MidiEvent*         addTimeSignature       (int aTrack, int aTick,
		                                           int top, int bottom,
		                                           int clocksPerClick = 24,
		                                           int num32dsPerQuarter = 8);
		MidiEvent*         addCompoundTimeSignature(int aTrack, int aTick,
		                                           int top, int bottom,
		                                           int clocksPerClick = 36,
		                                           int num32dsPerQuarter = 8);

		uchar              readByte               (std::istream& input);

		// static functions:
		static ushort        readLittleEndian2Bytes  (std::istream& input);
		static ulong         readLittleEndian4Bytes  (std::istream& input);
		static std::ostream& writeLittleEndianUShort (std::ostream& out,
		                                              ushort value);
		static std::ostream& writeBigEndianUShort    (std::ostream& out,
		                                              ushort value);
		static std::ostream& writeLittleEndianShort  (std::ostream& out,
		                                              short value);
		static std::ostream& writeBigEndianShort     (std::ostream& out,
		                                              short value);
		static std::ostream& writeLittleEndianULong  (std::ostream& out,
		                                              ulong value);
		static std::ostream& writeBigEndianULong     (std::ostream& out,
		                                              ulong value);
		static std::ostream& writeLittleEndianLong   (std::ostream& out,
		                                              long value);
		static std::ostream& writeBigEndianLong      (std::ostream& out,
		                                              long value);
		static std::ostream& writeLittleEndianFloat  (std::ostream& out,
		                                              float value);
		static std::ostream& writeBigEndianFloat     (std::ostream& out,
		                                              float value);
		static std::ostream& writeLittleEndianDouble (std::ostream& out,
		                                              double value);
		static std::ostream& writeBigEndianDouble    (std::ostream& out,
		                                              double value);

	protected:
		// m_events == Lists of MidiEvents for each MIDI file track.
		std::vector<MidiEventList*> m_events;

		// m_ticksPerQuarterNote == A value for the MIDI file header
		// which represents the number of ticks in a quarter note
		// that are used as units for the delta times for MIDI events
		// in MIDI file track data.
		int m_ticksPerQuarterNote = 120;

		// m_trackCount == the number of tracks in the file.
		int m_trackCount = 1;

		// m_theTrackState == state variable for whether the tracks
		// are joined or split.
		int m_theTrackState = TRACK_STATE_SPLIT;

		// m_theTimeState == state variable for whether the MidiEvent::tick
		// variable contain absolute ticks since the start of the file's
		// time, or delta ticks since the last MIDI event in the track.
		int m_theTimeState = TIME_STATE_ABSOLUTE;

		// m_readFileName == the filename of the last file read into
		// the object.
		std::string m_readFileName;

		// m_timemapvalid ==
		bool m_timemapvalid = false;

		// m_timemap ==
		std::vector<_TickTime> m_timemap;

		// m_rwstatus == True if last read was successful, false if a problem.
		bool m_rwstatus = true;

		// m_linkedEventQ == True if link analysis has been done.
		bool m_linkedEventsQ = false;

	private:
		int        extractMidiData                 (std::istream& inputfile,
		                                            std::vector<uchar>& array,
		                                            uchar& runningCommand);
		ulong      readVLValue                     (std::istream& inputfile);
		ulong      unpackVLV                       (uchar a = 0, uchar b = 0,
		                                            uchar c = 0, uchar d = 0,
		                                            uchar e = 0);
		void       writeVLValue                    (long aValue,
		                                            std::vector<uchar>& data);
		int        makeVLV                         (uchar *buffer, int number);
		static int ticksearch                      (const void* A, const void* B);
		static int secondsearch                    (const void* A, const void* B);
		void       buildTimeMap                    (void);
		double     linearTickInterpolationAtSecond (double seconds);
		double     linearSecondInterpolationAtTick (int ticktime);
};

class Binasc {

	public:
		                     Binasc                  (void);

		                    ~Binasc                  ();

		// functions for setting options:
		int                  setLineLength           (int length);
		int                  getLineLength           (void);
		int                  setLineBytes            (int length);
		int                  getLineBytes            (void);
		void                 setComments             (int state);
		void                 setCommentsOn           (void);
		void                 setCommentsOff          (void);
		int                  getComments             (void);
		void                 setBytes                (int state);
		void                 setBytesOn              (void);
		void                 setBytesOff             (void);
		int                  getBytes                (void);
		void                 setMidi                 (int state);
		void                 setMidiOn               (void);
		void                 setMidiOff              (void);
		int                  getMidi                 (void);

		// functions for converting into a binary file:
		int                  writeToBinary           (const std::string& outfile,
		                                              const std::string& infile);
		int                  writeToBinary           (const std::string& outfile,
		                                              std::istream& input);
		int                  writeToBinary           (std::ostream& out,
		                                              const std::string& infile);
		int                  writeToBinary           (std::ostream& out,
		                                              std::istream& input);

		// functions for converting into an ASCII file with hex bytes:
		int                  readFromBinary          (const std::string&
		                                              outfile,
		                                              const std::string& infile);
		int                  readFromBinary          (const std::string& outfile,
		                                              std::istream& input);
		int                  readFromBinary          (std::ostream& out,
		                                              const std::string& infile);
		int                  readFromBinary          (std::ostream& out,
		                                              std::istream& input);

		// static functions for writing ordered bytes:
		static std::ostream& writeLittleEndianUShort (std::ostream& out,
		                                              ushort value);
		static std::ostream& writeBigEndianUShort    (std::ostream& out,
		                                              ushort value);
		static std::ostream& writeLittleEndianShort  (std::ostream& out,
		                                              short value);
		static std::ostream& writeBigEndianShort     (std::ostream& out,
		                                              short value);
		static std::ostream& writeLittleEndianULong  (std::ostream& out,
		                                              ulong value);
		static std::ostream& writeBigEndianULong     (std::ostream& out,
		                                              ulong value);
		static std::ostream& writeLittleEndianLong   (std::ostream& out,
		                                              long value);
		static std::ostream& writeBigEndianLong      (std::ostream& out,
		                                              long value);
		static std::ostream& writeLittleEndianFloat  (std::ostream& out,
		                                              float value);
		static std::ostream& writeBigEndianFloat     (std::ostream& out,
		                                              float value);
		static std::ostream& writeLittleEndianDouble (std::ostream& out,
		                                              double value);
		static std::ostream& writeBigEndianDouble    (std::ostream& out,
		                                              double value);

		static std::string   keyToPitchName          (int key);

	protected:
		int m_bytesQ;       // option for printing hex bytes in ASCII output.
		int m_commentsQ;    // option for printing comments in ASCII output.
		int m_midiQ;        // output ASCII data as parsed MIDI file.
		int m_maxLineLength;// number of character in ASCII output on a line.
		int m_maxLineBytes; // number of hex bytes in ASCII output on a line.

	private:
		// helper functions for reading ASCII content to conver to binary:
		int                  processLine             (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processAsciiWord        (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processStringWord       (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processBinaryWord       (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processDecimalWord      (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processHexWord          (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processVlvWord          (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processMidiPitchBendWord(std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);
		int                  processMidiTempoWord    (std::ostream& out,
		                                              const std::string& input,
		                                              int lineNum);

		// helper functions for reading binary content to convert to ASCII:
		int  outputStyleAscii   (std::ostream& out, std::istream& input);
		int  outputStyleBinary  (std::ostream& out, std::istream& input);
		int  outputStyleBoth    (std::ostream& out, std::istream& input);
		int  outputStyleMidi    (std::ostream& out, std::istream& input);

		// MIDI parsing helper functions:
		int  readMidiEvent  (std::ostream& out, std::istream& infile,
		                     int& trackbytes, int& command);
		int  getVLV         (std::istream& infile, int& trackbytes);
		int  getWord        (std::string& word, const std::string& input,
		                     const std::string& terminators, int index);

};

class Option_register {
	public:
		                  Option_register    (void);
		                  Option_register    (const std::string& aDefinition,
		                                      char aType,
		                                      const std::string& aDefaultOption);
		                  Option_register    (const std::string& aDefinition,
		                                      char aType,
		                                      const std::string& aDefaultOption,
		                                      const std::string& aModifiedOption);

		                 ~Option_register    ();

	  void               clearModified      (void);
	  const std::string& getDefinition      (void);
	  const std::string& getDefault         (void);
	  const std::string& getOption          (void);
	  const std::string& getModified        (void);
	  const std::string& getDescription     (void);
	  bool               isModified         (void);
	  char               getType            (void);
	  void               reset              (void);
	  void               setDefault         (const std::string& aString);
	  void               setDefinition      (const std::string& aString);
	  void               setDescription     (const std::string& aString);
	  void               setModified        (const std::string& aString);
	  void               setType            (char aType);
	  std::ostream&      print              (std::ostream& out);

	protected:
		std::string       definition;
		std::string       description;
		std::string       defaultOption;
		std::string       modifiedOption;
		bool              modifiedQ;
		char              type;

};

class Options {
	public:
		                   Options           (void);
		                   Options           (int argc, char** argv);

		                  ~Options           ();

		int                argc              (void) const;
		const std::vector<std::string>& argv (void) const;
		int                define            (const std::string& aDefinition);
		int                define            (const std::string& aDefinition,
		                                      const std::string& description);
		const std::string& getArg            (int index);
		const std::string& getArgument       (int index);
		int                getArgCount       (void);
		int                getArgumentCount  (void);
		const std::vector<std::string>& getArgList      (void);
		const std::vector<std::string>& getArgumentList (void);
		bool               getBoolean        (const std::string& optionName);
		std::string        getCommand        (void);
		const std::string& getCommandLine    (void);
		std::string        getDefinition     (const std::string& optionName);
		double             getDouble         (const std::string& optionName);
		char               getFlag           (void);
		char               getChar           (const std::string& optionName);
		float              getFloat          (const std::string& optionName);
		int                getInt            (const std::string& optionName);
		int                getInteger        (const std::string& optionName);
		std::string        getString         (const std::string& optionName);
		char               getType           (const std::string& optionName);
		int                optionsArg        (void);
		std::ostream&      print             (std::ostream& out);
		std::ostream&      printOptionList   (std::ostream& out);
		std::ostream&      printOptionListBooleanState(std::ostream& out);
		void               process           (int error_check = 1,
		                                      int suppress = 0);
		void               process           (int argc, char** argv,
		                                      int error_check = 1,
		                                      int suppress = 0);
		void               reset             (void);
		void               xverify           (int argc, char** argv,
		                                      int error_check = 1,
		                                      int suppress = 0);
		void               xverify           (int error_check = 1,
		                                      int suppress = 0);
		void               setFlag           (char aFlag);
		void               setModified       (const std::string& optionName,
		                                      const std::string& optionValue);
		void               setOptions        (int argc, char** argv);
		void               appendOptions     (int argc, char** argv);
		void               appendOptions     (const std::string& strang);
		void               appendOptions     (const std::vector<std::string>& argv);
		std::ostream&      printRegister     (std::ostream& out);
		bool               isDefined         (const std::string& name);

	protected:
		int                           m_options_error_check;  // verify command
		int                           m_oargc;
		std::vector<std::string>      m_oargv;
		std::string                   m_commandString;
		char                          m_optionFlag;
		std::vector<std::string>      m_argument;

		std::vector<Option_register*> m_optionRegister;
		std::map<std::string, int>    m_optionList;

		int                           m_processedQ;
		int                           m_suppressQ;       // prevent --options
		int                           m_optionsArgument; // --options present

		std::vector<std::string>      m_extraArgv;
		std::vector<std::string>      m_extraArgv_strings;

	private:
		int                getRegIndex       (const std::string& optionName);
		int                optionQ           (const std::string& aString,
		                                      int& argp);
		int                storeOption       (int gargp, int& position,
		                                      int& running);

};

///////////////////////////////////////////////////////////////////////////
//
// Option_register class function definitions.
//


//////////////////////////////
//
// Option_register::Option_register -- Constructor.
//

Option_register::Option_register(void) {
   type = 's';
   modifiedQ = 0;
}


Option_register::Option_register(const std::string& aDefinition, char aType,
      const std::string& aDefaultOption) {
   type = 's';
   modifiedQ = 0;
   setType(aType);
   setDefinition(aDefinition);
   setDefault(aDefaultOption);
}

Option_register::Option_register(const std::string& aDefinition, char aType,
      const std::string& aDefaultOption, const std::string& aModifiedOption) {
   type = 's';
   modifiedQ = 0;
   setType(aType);
   setDefinition(aDefinition);
   setDefault(aDefaultOption);
   setModified(aModifiedOption);
}

//////////////////////////////
//
// Option_register::~Option_register -- Destructor.
//

Option_register::~Option_register() {
   // do nothing
}

//////////////////////////////
//
// Option_register::clearModified -- Clear any changes in the option value.
//

void Option_register::clearModified(void) {
   modifiedOption.clear();
   modifiedQ = 0;
}

//////////////////////////////
//
// Option_register::getDefinition -- Returns the initial definition.
//	string used to define this entry.
//

const std::string& Option_register::getDefinition(void) {
   return definition;
}

//////////////////////////////
//
// Option_register::getDescription -- Return the textual description
//      of the entry.
//

const std::string& Option_register::getDescription(void) {
   return description;
}

//////////////////////////////
//
// Option_register::getDefault --  Return the default value string.
//

const std::string& Option_register::getDefault(void) {
   return defaultOption;
}

//////////////////////////////
//
// Option_register::getModified -- Return the modified option string.
//

const std::string& Option_register::getModified(void) {
   return modifiedOption;
}

//////////////////////////////
//
// Option_register::isModified -- Return true if option has been
//    set on the command-line.
//

bool Option_register::isModified(void) {
   return modifiedQ;
}

//////////////////////////////
//
// Option_register::getType -- Return the data type of the option.
//

char Option_register::getType(void) {
   return type;
}

//////////////////////////////
//
// Option_register::getOption -- return the modified option
//  	or the default option if no modified option.
//

const std::string& Option_register::getOption(void) {
   if (isModified()) {
      return getModified();
   } else {
      return getDefault();
   }
}

//////////////////////////////
//
// Option_register::reset -- deallocate space for all
//	strings in object.  (but default string is set to "")
//

void Option_register::reset(void) {
   definition.clear();
   defaultOption.clear();
   modifiedOption.clear();
}

//////////////////////////////
//
// Option_register::setDefault -- Set the default value.
//

void Option_register::setDefault(const std::string& aString) {
   defaultOption = aString;
}

//////////////////////////////
//
// Option_register::setDefinition -- Set the option definition.
//

void Option_register::setDefinition(const std::string& aString) {
   definition = aString;
}

//////////////////////////////
//
// Option_register::setDescription -- Set the textual description.
//

void Option_register::setDescription(const std::string& aString) {
   description = aString;
}

//////////////////////////////
//
// Option_register::setModified -- Set the modified value.
//

void Option_register::setModified(const std::string& aString) {
   modifiedOption = aString;
   modifiedQ = 1;
}

//////////////////////////////
//
// Option_register::setType -- Set the option type.
//

void Option_register::setType(char aType) {
   type = aType;
}

//////////////////////////////
//
// Option_register::print -- Print the state of the option registery entry.
//     Useul for debugging.
//

std::ostream& Option_register::print(std::ostream& out) {
   out << "definition:\t"     << definition     << std::endl;
   out << "description:\t"    << description    << std::endl;
   out << "defaultOption:\t"  << defaultOption  << std::endl;
   out << "modifiedOption:\t" << modifiedOption << std::endl;
   out << "modifiedQ:\t\t"    << modifiedQ      << std::endl;
   out << "type:\t\t"         << type           << std::endl;
   return out;
};

///////////////////////////////////////////////////////////////////////////
//
// Options class function definitions.
//

//////////////////////////////
//
// Options::Options -- Constructor.
//

Options::Options(void) {
   m_oargc               = -1;
   m_suppressQ           =  0;
   m_processedQ          =  0;
   m_optionsArgument     =  0;
   m_options_error_check =  1;
   m_optionFlag          = '-';

   m_extraArgv.reserve(100);
   m_extraArgv_strings.reserve(100);
}

Options::Options(int argc, char** argv) {
   m_oargc               = -1;
   m_suppressQ           =  0;
   m_processedQ          =  0;
   m_optionsArgument     =  0;
   m_options_error_check =  1;
   m_optionFlag          = '-';

   m_extraArgv.reserve(100);
   m_extraArgv_strings.reserve(100);

   setOptions(argc, argv);
}

//////////////////////////////
//
// Options::~Options -- Destructor.
//

Options::~Options() {
   reset();
}

//////////////////////////////
//
// Options::argc -- returns the argument count as input from main().
//

int Options::argc(void) const {
   return m_oargc;
}

//////////////////////////////
//
// Options::argv -- returns the arguments strings as input from main().
//

const std::vector<std::string>& Options::argv(void) const {
   return m_oargv;
}

//////////////////////////////
//
// Options::define -- store an option definition in the registry.  Option
//     definitions have this sructure:
//        option-name|alias-name1|alias-name2=option-type:option-default
// option-name :: name of the option (one or more character, not including
//      spaces or equal signs.
// alias-name  :: equivalent name(s) of the option.
// option-type :: single charater indicating the option data type.
// option-default :: default value for option if no given on the command-line.
//

int Options::define(const std::string& aDefinition) {
   Option_register* definitionEntry = NULL;

   // Error if definition string doesn't contain an equals sign
   auto location = aDefinition.find("=");
   if (location == std::string::npos) {
      std::cerr << "Error: no \"=\" in option definition: " << aDefinition << std::endl;
      exit(1);
   }

   std::string aliases = aDefinition.substr(0, location);
   std::string rest    = aDefinition.substr(location+1);
   std::string otype   = rest;
   std::string ovalue  = "";

   location = rest.find(":");
   if (location != std::string::npos) {
      otype  = rest.substr(0, location);
      ovalue = rest.substr(location+1);
   }

   // Remove anyspaces in the option type field
   otype.erase(remove_if(otype.begin(), otype.end(), ::isspace), otype.end());

   // Option types are only a single charater (b, i, d, c or s)
   if (otype.size() != 1) {
      std::cerr << "Error: option type is invalid: " << otype
           << " in option definition: " << aDefinition << std::endl;
      exit(1);
   }

   // Check to make sure that the type is known
   if (otype[0] != OPTION_STRING_TYPE  &&
       otype[0] != OPTION_INT_TYPE     &&
       otype[0] != OPTION_FLOAT_TYPE   &&
       otype[0] != OPTION_DOUBLE_TYPE  &&
       otype[0] != OPTION_BOOLEAN_TYPE &&
       otype[0] != OPTION_CHAR_TYPE ) {
      std::cerr << "Error: unknown option type \'" << otype[0]
           << "\' in defintion: " << aDefinition << std::endl;
      exit(1);
   }

   // Set up space for a option entry in the registry
   definitionEntry = new Option_register(aDefinition, otype[0], ovalue);

   int definitionIndex = (int)m_optionRegister.size();

   // Store option aliases
   std::string optionName;
   unsigned int i;
   aliases += '|';
   for (i=0; i<aliases.size(); i++) {
      if (::isspace(aliases[i])) {
         continue;
      } else if (aliases[i] == '|') {
         if (isDefined(optionName)) {
            std::cerr << "Option \"" << optionName << "\" from definition:" << std::endl;
            std::cerr << "\t" << aDefinition << std::endl;
            std::cerr << "is already defined in: " << std::endl;
            std::cerr << "\t" << getDefinition(optionName) << std::endl;
            exit(1);
         }
         if (optionName.size() > 0) {
            m_optionList[optionName] = definitionIndex;
         }
         optionName.clear();
      } else {
         optionName += aliases[i];
      }
   }

   // Store definition in registry and return its indexed location.
   // This location will be used to link option aliases to the main
   // command name.
   m_optionRegister.push_back(definitionEntry);
   return definitionIndex;
}

int Options::define(const std::string& aDefinition,
		const std::string& aDescription) {
   int index = define(aDefinition);
   m_optionRegister[index]->setDescription(aDescription);
   return index;
}



//////////////////////////////
//
// Options::isDefined -- Return true if option is present in registry.
//

bool Options::isDefined(const std::string& name) {
   return m_optionList.find(name) == m_optionList.end() ? false : true;
}



//////////////////////////////
//
// Options::getArg -- returns the specified argument.
//	argurment 0 is the command name.
//

const std::string& Options::getArg(int index) {
   if (index < 0 || index >= (int)m_argument.size()) {
      std::cerr << "Error: m_argument " << index << " does not exist." << std::endl;
      exit(1);
   }
   return m_argument[index];
}

// Alias:

const std::string& Options::getArgument(int index) {
   return getArg(index);
}



//////////////////////////////
//
// Options::getArgCount --  number of arguments on command line.
//	does not count the options or the command name.
//

int Options::getArgCount(void) {
   return ((int)m_argument.size()) - 1;
}

// Alias:

int Options::getArgumentCount(void) {
   return getArgCount();
}



//////////////////////////////
//
// Options::getArgList -- return a string vector of the arguments
//     after the options have been parsed out of it.
//

const std::vector<std::string>& Options::getArgList(void) {
   return m_argument;
}

// Alias:

const std::vector<std::string>& Options::getArgumentList(void) {
   return getArgList();
}



//////////////////////////////
//
// Options::getBoolean --  returns true if the option was
//	used on the command line.
//

bool Options::getBoolean(const std::string& optionName) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return 0;
   }
   return m_optionRegister[index]->isModified();
}



//////////////////////////////
//
// Options::getCommand -- returns argv[0] (the first string
//     in the original argv list.
//

std::string Options::getCommand(void) {
   if (m_argument.size() == 0) {
      return "";
   } else {
      return m_argument[0];
   }
}



//////////////////////////////
//
// Options::getCommandLine -- returns a string which contains the
//     command-line call to the program.  Deal with spaces in arguments...
//

const std::string& Options::getCommandLine(void) {
   if (m_commandString.size()) {
      return m_commandString;
   }

   m_commandString = m_oargv[0];

   int i;
   for (i=1; i<m_oargc; i++) {
      m_commandString += " ";
      m_commandString += m_oargv[i];
   }

   return m_commandString;
}




//////////////////////////////
//
// Options::getDefinition -- returns the definition for the specified
//      option name.  Returns empty string if there is no entry for
//      the option name.  spaces count in the input option name.
//

std::string Options::getDefinition(const std::string& optionName) {
   auto it = m_optionList.find(optionName);
   if (it == m_optionList.end()) {
      return "";
   } else {
      return m_optionRegister[it->second]->getDefinition();
   }
}



//////////////////////////////
//
// Options::getDouble -- returns the double float associated
//	with the given option.  Returns 0 if there is no
//	number associated with the option.
//

double Options::getDouble(const std::string& optionName) {
   return strtod(getString(optionName).c_str(), (char**)NULL);
}



//////////////////////////////
//
// Options::getChar -- Return the first character in the option string;
//      If the length is zero, then return '\0'.
//

char Options::getChar(const std::string& optionName) {
   return getString(optionName).c_str()[0];
}



//////////////////////////////
//
// Options::getFloat -- Return the floating point number
//	associated with the given option.
//

float Options::getFloat(const std::string& optionName) {
   return (float)getDouble(optionName);
}



//////////////////////////////
//
// Options::getInt -- Return the integer argument.  Can handle
//	hexadecimal, decimal, and octal written in standard
//	C syntax.
//

int Options::getInt(const std::string& optionName) {
   return (int)strtol(getString(optionName).c_str(), (char**)NULL, 0);
}

int Options::getInteger(const std::string& optionName) {
   return getInt(optionName);
}



//////////////////////////////
//
// Options::getString -- Return the option argument string.
//

std::string Options::getString(const std::string& optionName) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return "UNKNOWN OPTION";
   } else {
      return m_optionRegister[index]->getOption();
   }
}



//////////////////////////////
//
// Options::optionsArg -- Return true if --options is present
//    on the command line, otherwise returns false.
//

int Options::optionsArg(void) {
   return m_optionsArgument;
}



//////////////////////////////
//
// Options::print -- Print a list of the defined options.
//

std::ostream& Options::print(std::ostream& out) {
   for (unsigned int i=0; i<m_optionRegister.size(); i++) {
      out << m_optionRegister[i]->getDefinition() << "\t"
           << m_optionRegister[i]->getDescription() << std::endl;
   }
   return out;
}



//////////////////////////////
//
// Options::reset -- Clear all defined options.
//

void Options::reset(void) {
   unsigned int i;
   for (i=0; i<m_optionRegister.size(); i++) {
      delete m_optionRegister[i];
      m_optionRegister[i] = NULL;
   }
   m_optionRegister.clear();

   m_argument.clear();
   m_commandString.clear();
   m_extraArgv.clear();
   m_extraArgv_strings.clear();

   m_oargc = -1;
   m_oargv.clear();
}



//////////////////////////////
//
// Options::getFlag -- Set the character which is usually set to a dash.
//

char Options::getFlag(void) {
   return m_optionFlag;
}



//////////////////////////////
//
// Options::setFlag -- Set the character used to indicate an
//	option.  For unix this is usually '-', in MS-DOS,
//	this is usually '/';  But the syntax of the Options
//	class is for Unix-style options.
//

void Options::setFlag(char aFlag) {
   m_optionFlag = aFlag;
}



//////////////////////////////
//
// Options::setModified --
//

void Options::setModified(const std::string& optionName,
		const std::string& aString) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return;
   }

   m_optionRegister[getRegIndex(optionName)]->setModified(aString);
}




//////////////////////////////
//
// Options::setOptions --  Store the input list of options.
//

void Options::setOptions(int argc, char** argv) {
   m_processedQ = 0;

   m_extraArgv.resize(argc);
   m_extraArgv_strings.resize(argc);
   int oldsize = 0;

   int i;
   for (i=0; i<argc; i++) {
      m_extraArgv_strings[i+oldsize] = argv[i];
      m_extraArgv[i] = m_extraArgv_strings[i];
   }

   m_oargc  = (int)m_extraArgv.size();
   m_oargv  = m_extraArgv;
}



//////////////////////////////
//
// Options::appendOptions -- Add argc and argv data to the current
//      list residing inside the Options class variable.
//

void Options::appendOptions(int argc, char** argv) {
   m_processedQ = 0;

   // data used to be stored directly here:
   //gargc = argc;
   //gargv = argv;
   // but now gets interfaced to: m_extraArgv and m_extraArgv_strings:

   int oldsize = (int)m_extraArgv.size();
   m_extraArgv.resize(oldsize + argc);
   m_extraArgv_strings.resize(oldsize + argc);

   int i;
   for (i=0; i<argc; i++) {
      m_extraArgv_strings[i+oldsize] = argv[i];
      m_extraArgv[i+oldsize] = m_extraArgv_strings[i+oldsize];
   }

   m_oargc = (int)m_extraArgv.size();
   m_oargv = m_extraArgv;
}


void Options::appendOptions(const std::vector<std::string>& argv) {
   m_processedQ = 0;

   int oldsize = (int)m_extraArgv.size();
   m_extraArgv.resize(oldsize + argv.size());
   m_extraArgv_strings.resize(oldsize + argv.size());

   unsigned int i;
   for (i=0; i<argv.size(); i++) {
      m_extraArgv_strings[i+oldsize] = argv[i];
      m_extraArgv[i+oldsize] = m_extraArgv_strings[i+oldsize];
   }

   m_oargc = (int)m_extraArgv.size();
   m_oargv = m_extraArgv;
}



//////////////////////////////
//
// Options::appendOptions -- parse the string like command-line arguments.
//   Either double or single quotes can be used to encapsulate
//   a command-line token.  If double quotes are used to encapsulate,
//   then you will not have to back quote single quotes inside the
//   token string, but you will have to backslash double quotes:
//      "-T \"\"" but "-T ''"
//   Likewise for single quotes in reverse with double quotes:
//      '-T \'\'' is equal to: '-T ""'
//

void Options::appendOptions(const std::string& strang) {
   int i;
   int doublequote = 0;
   int singlequote = 0;

   std::vector<std::string> tokens;
   std::vector<std::string> tempargv;
   std::string tempvalue;

   tokens.reserve(100);
   tempargv.reserve(100);
   tempvalue.reserve(1000);

   char ch;

   int length = (int)strang.size();
   for (i=0; i<length; i++) {

      if (!singlequote && (strang[i] == '"')) {
         if ((i>0) && (strang[i-1] != '\\')) {
            doublequote = !doublequote;
            if (doublequote == 0) {
               // finished a doublequoted section of data, so store
               // even if it is the empty string
               ch = '\0';
               tempvalue += (ch);
               tokens.push_back(tempvalue);
               tempvalue.clear();
               continue;
            } else {
               // don't store the leading ":
               continue;
            }
         }
      } else if (!doublequote && (strang[i] == '\'')) {
         if ((i>0) && (strang[i-1] != '\\')) {
            singlequote = !singlequote;
            if (singlequote == 0) {
               // finished a singlequote section of data, so store
               // even if it is the empty string
               ch = '\0';
               tempvalue += ch;
               tokens.push_back(tempvalue);
               tempvalue.clear();
               continue;
            } else {
               // don't store the leading ":
               continue;
            }
         }
      }


      if ((!doublequote && !singlequote) && std::isspace(strang[i])) {
         if (tempvalue.size() > 0) {
        	ch = '\0';
            tempvalue += ch;
            tokens.push_back(tempvalue);
            tempvalue.clear();
         }
      } else {
         ch = strang[i];
         tempvalue += ch;
      }
   }
   if (tempvalue.size() > 0) {
      tokens.push_back(tempvalue);
      tempvalue.clear();
   }

   // now that the input string has been chopped up into pieces,
   // assemble the argv structure

   tempargv.reserve(tokens.size());
   for (i=0; i<(int)tempargv.size(); i++) {
      tempargv[i] = tokens[i];
   }

   // now store the argv and argc data in opts:

   // now store the parsed command-line simulated tokens
   // for actual storage:
   appendOptions(tempargv);
}



//////////////////////////////
//
// Options:getType -- Return the type of the option.
//

char Options::getType(const std::string& optionName) {
   int index = getRegIndex(optionName);
   if (index < 0) {
      return -1;
   } else {
      return m_optionRegister[getRegIndex(optionName)]->getType();
   }
}



//////////////////////////////
//
// Options::process -- Same as xverify.
//   	default values: error_check = 1, suppress = 0;
//

void Options::process(int argc, char** argv, int error_check, int suppress) {
   setOptions(argc, argv);
   xverify(error_check, suppress);
}


void Options::process(int error_check, int suppress) {
   xverify(error_check, suppress);
}



//////////////////////////////
//
// Options::xverify --
//	default value: error_check = 1, suppress = 0;
//

void Options::xverify(int error_check, int suppress) {
   m_options_error_check = error_check;
   int gargp = 1;
   int optionend = 0;

   if (suppress) {
      m_suppressQ = 1;
   } else {
      m_suppressQ = 0;
   }

   // if calling xverify again, must remove previous argument list.
   if (m_argument.size() != 0) {
      m_argument.clear();
   }

   m_argument.push_back(m_oargv[0]);
   int oldgargp;
   int position = 0;
   int running = 0;
   while (gargp < m_oargc && optionend == 0) {
      if (optionQ(m_oargv[gargp], gargp)) {
         oldgargp = gargp;
         gargp = storeOption(gargp, position, running);
         if (gargp != oldgargp) {
            running = 0;
            position = 0;
         }
      } else {
         if (m_oargv[gargp].size() == 2 && m_oargv[gargp][0] == getFlag() &&
            m_oargv[gargp][2] == getFlag() ) {
               optionend = 1;
            gargp++;
            break;
         } else {                          // this is an argument
            m_argument.push_back(m_oargv[gargp]);
            gargp++;
         }
      }
   }

   while (gargp < m_oargc) {
      m_argument.push_back(m_oargv[gargp]);
      gargp++;
   }

}


void Options::xverify(int argc, char** argv, int error_check, int suppress) {
   setOptions(argc, argv);
   xverify(error_check, suppress);
}




///////////////////////////////////////////////////////////////////////////
//
// private functions
//


//////////////////////////////
//
// Options::getRegIndex -- returns the index of the option associated
//	with this name.
//

int Options::getRegIndex(const std::string& optionName) {
   if (m_suppressQ && (optionName == "options")) {
         return -1;
   }

   if (optionName == "options") {
      print(std::cout);
      exit(0);
   }


   auto it = m_optionList.find(optionName);
   if (it == m_optionList.end()) {
      if (m_options_error_check) {
         std::cerr << "Error: unknown option \"" << optionName << "\"." << std::endl;
         print(std::cout);
         exit(1);
      } else {
         return -1;
      }
   } else {
      return it->second;
   }
}



//////////////////////////////
//
// Options::optionQ --  returns true if the string is an option
//	"--" is not an option, also '-' is not an option.
//	aString is assumed to not be NULL.
//

int Options::optionQ(const std::string& aString, int& argp) {
   if (aString[0] == getFlag()) {
      if (aString[1] == '\0') {
         argp++;
         return 0;
      } else if (aString[1] == getFlag()) {
         if (aString[2] == '\0') {
            argp++;
            return 0;
         } else {
            return 1;
         }
      } else {
         return 1;
      }
   } else {
      return 0;
   }
}



//////////////////////////////
//
// Options::storeOption --
//

#define OPTION_FORM_SHORT     0
#define OPTION_FORM_LONG      1
#define OPTION_FORM_CONTINUE  2

int Options::storeOption(int gargp, int& position, int& running) {
   int optionForm;
   char tempname[1024];
   char optionType = '\0';

   if (running) {
      optionForm = OPTION_FORM_CONTINUE;
   } else if (m_oargv[gargp][1] == getFlag()) {
      optionForm = OPTION_FORM_LONG;
   } else {
      optionForm = OPTION_FORM_SHORT;
   }

   switch (optionForm) {
      case OPTION_FORM_CONTINUE:
         position++;
         tempname[0] = m_oargv[gargp][position];
         tempname[1] = '\0';
         optionType = getType(tempname);
         if (optionType != OPTION_BOOLEAN_TYPE) {
            running = 0;
            position++;
         }
         break;
      case OPTION_FORM_SHORT:
         position = 1;
         tempname[0] = m_oargv[gargp][position];
         tempname[1] = '\0';
         optionType = getType(tempname);
         if (optionType != OPTION_BOOLEAN_TYPE) {
            position++;
         }
         break;
      case OPTION_FORM_LONG:
         position = 2;
         while (m_oargv[gargp][position] != '=' &&
               m_oargv[gargp][position] != '\0') {
            tempname[position-2] = m_oargv[gargp][position];
            position++;
         }
         tempname[position-2] = '\0';
         optionType = getType(tempname);
         if (optionType == -1) {         // suppressed --options option
            m_optionsArgument = 1;
            break;
         }
         if (m_oargv[gargp][position] == '=') {
            if (optionType == OPTION_BOOLEAN_TYPE) {
               std::cerr << "Error: boolean variable cannot have any options: "
                    << tempname << std::endl;
               exit(1);
            }
            position++;
         }
         break;
   }

   if (optionType == -1) {              // suppressed --options option
      m_optionsArgument = 1;
      gargp++;
      position = 0;
      return gargp;
   }

   if (m_oargv[gargp][position] == '\0' &&
         optionType != OPTION_BOOLEAN_TYPE) {
      gargp++;
      position = 0;
   }

   if (optionForm != OPTION_FORM_LONG && optionType == OPTION_BOOLEAN_TYPE &&
         m_oargv[gargp][position+1] != '\0') {
      running = 1;
   } else if (optionType == OPTION_BOOLEAN_TYPE &&
         m_oargv[gargp][position+1] == '\0') {
      running = 0;
   }

   if (gargp >= m_oargc) {
      std::cerr << "Error: last option requires a parameter" << std::endl;
      exit(1);
   }
   setModified(tempname, &m_oargv[gargp][position]);

   if (!running) {
      gargp++;
   }
   return gargp;
}



//////////////////////////////
//
// Options::printOptionList --
//

std::ostream& Options::printOptionList(std::ostream& out) {
   for (auto it = m_optionList.begin(); it != m_optionList.end(); it++) {
      out << it->first << "\t" << it->second << std::endl;
   }
   return out;
}



//////////////////////////////
//
// Options::printOptionBooleanState --
//

std::ostream& Options::printOptionListBooleanState(std::ostream& out) {
   for (auto it = m_optionList.begin(); it != m_optionList.end(); it++) {
      out << it->first << "\t"
          << m_optionRegister[it->second]->isModified() << std::endl;
   }
   return out;
}



//////////////////////////////
//
// Options::printRegister --
//

std::ostream& Options::printRegister(std::ostream& out) {
   for (auto it = m_optionRegister.begin(); it != m_optionRegister.end(); it++) {
      (*it)->print(out);
   }
   return out;
}


//////////////////////////////
//
// Binasc::Binasc -- Constructor: set the default option values.
//

Binasc::Binasc(void) {
	m_bytesQ    = 1; // for printing HEX bytes when converting to ASCII
	m_commentsQ = 0; // for printing text comments when converting to ASCII
	m_midiQ     = 0; // for printing ASCII as parsed MIDI file.
	m_maxLineLength = 75;
	m_maxLineBytes  = 25;
}



//////////////////////////////
//
// Binasc::~Binasc -- Destructor.
//

Binasc::~Binasc() {
	// do nothing
}



//////////////////////////////
//
// Binasc::setLineLength -- Set the maximum length of a line when converting
//    binary content into ASCII bytes.  If the input size is less than one,
//    set to the default value of 75 characters per line.
//

int Binasc::setLineLength(int length) {
	if (length < 1) {
		m_maxLineLength = 75;
	} else {
		m_maxLineLength = length;
	}
	return m_maxLineLength;
}



//////////////////////////////
//
// Binasc::getLineLength -- Set the maximum length of a line when converting
//    binary content into ASCII bytes.
//

int Binasc::getLineLength(void) {
	return m_maxLineLength;
}



//////////////////////////////
//
// Binasc::setLineBytes -- Set the maximum number of hex bytes in ASCII output.
//    If the input size is less than one, set to the default value of 25
//    hex bytes per line.
//

int Binasc::setLineBytes(int length) {
	if (length < 1) {
		m_maxLineBytes = 25;
	} else {
		m_maxLineBytes = length;
	}
	return m_maxLineBytes;
}



//////////////////////////////
//
// Binasc::getLineBytes -- Get the maximum number of hex bytes in ASCII output.
//

int Binasc::getLineBytes(void) {
	return m_maxLineLength;
}



//////////////////////////////
//
// Binasc::setComments -- Display or not display printable characters
//    as comments when converting binary files to ASCII byte codes.
//

void Binasc::setComments(int state) {
	m_commentsQ = state ? 1 : 0;
}


void Binasc::setCommentsOn(void) {
	setComments(true);
}


void Binasc::setCommentsOff(void) {
	setComments(false);
}



//////////////////////////////
//
// Binasc::getComments -- Get the comment display style for
//    showing comments in ASCII output;
//

int Binasc::getComments(void) {
	return m_commentsQ;
}



//////////////////////////////
//
// Binasc::setBytes -- Display or not display hex codes (only
//    print ASCII printable characters).
//

void Binasc::setBytes(int state) {
	m_bytesQ = state ? 1 : 0;
}


void Binasc::setBytesOn(void) {
	setBytes(true);
}


void Binasc::setBytesOff(void) {
	setBytes(false);
}


//////////////////////////////
//
// Binasc::getBytes -- Get hex byte display status.
//

int Binasc::getBytes(void) {
	return m_bytesQ;
}


//////////////////////////////
//
// Binasc::setMidi -- Display or not display parsed MIDI data.
//

void Binasc::setMidi(int state) {
	m_midiQ = state ? 1 : 0;
}


void Binasc::setMidiOn(void) {
	setMidi(true);
}


void Binasc::setMidiOff(void) {
	setMidi(false);
}



//////////////////////////////
//
// Binasc::getMidi -- Get the MIDI file printing style option state.
//

int Binasc::getMidi(void) {
	return m_midiQ;
}



//////////////////////////////
//
// Binasc::writeToBinary -- Convert an ASCII representation of bytes into
//     the binary file that it describes.  Returns 0 if there was a problem
//     otherwise returns 1.
//

int Binasc::writeToBinary(const std::string& outfile,
		const std::string& infile) {
	std::ifstream input;
	input.open(infile.c_str());
	if (!input.is_open()) {
		std::cerr << "Cannot open " << infile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	std::ofstream output;
	output.open(outfile.c_str());
	if (!output.is_open()) {
		std::cerr << "Cannot open " << outfile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	int status = writeToBinary(output, input);
	input.close();
	output.close();
	return status;
}


int Binasc::writeToBinary(const std::string& outfile, std::istream& input) {
	std::ofstream output;
	output.open(outfile.c_str());
	if (!output.is_open()) {
		std::cerr << "Cannot open " << outfile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	int status = writeToBinary(output, input);
	output.close();
	return status;
}


int Binasc::writeToBinary(std::ostream& out, const std::string& infile) {
	std::ifstream input;
	input.open(infile.c_str());
	if (!input.is_open()) {
		std::cerr << "Cannot open " << infile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	int status = writeToBinary(out, input);
	input.close();
	return status;
}


int Binasc::writeToBinary(std::ostream& out, std::istream& input) {
	char inputLine[1024] = {0};    // current line being processed
	int  lineNum = 0;              // current line number

	input.getline(inputLine, 1024, '\n');
	lineNum++;
	while (!input.eof()) {
		int status = processLine(out, inputLine, lineNum);
		if (!status) {
			return 0;
		}
		input.getline(inputLine, 1024, '\n');
		lineNum++;
	}
	return 1;
}



//////////////////////////////
//
// Binasc::readFromBinary -- convert an ASCII representation of bytes into
//     the binary file that it describes.
//

int Binasc::readFromBinary(const std::string& outfile, const std::string& infile) {
	std::ifstream input;
	input.open(infile.c_str());
	if (!input.is_open()) {
		std::cerr << "Cannot open " << infile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	std::ofstream output;
	output.open(outfile.c_str());
	if (!output.is_open()) {
		std::cerr << "Cannot open " << outfile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	int status = readFromBinary(output, input);
	input.close();
	output.close();
	return status;
}


int Binasc::readFromBinary(const std::string& outfile, std::istream& input) {
	std::ofstream output;
	output.open(outfile.c_str());
	if (!output.is_open()) {
		std::cerr << "Cannot open " << outfile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	int status = readFromBinary(output, input);
	output.close();
	return status;
}


int Binasc::readFromBinary(std::ostream& out, const std::string& infile) {
	std::ifstream input;
	input.open(infile.c_str());
	if (!input.is_open()) {
		std::cerr << "Cannot open " << infile
		          << " for reading in binasc." << std::endl;
		return 0;
	}

	int status = readFromBinary(out, input);
	input.close();
	return status;
}


int Binasc::readFromBinary(std::ostream& out, std::istream& input) {
	int status;
	if (m_midiQ) {
		status = outputStyleMidi(out, input);
	} else if (!m_bytesQ) {
		status = outputStyleAscii(out, input);
	} else if (m_bytesQ && m_commentsQ) {
		status = outputStyleBoth(out, input);
	} else {
		status = outputStyleBinary(out, input);
	}
	return status;
}



///////////////////////////////////////////////////////////////////////////
//
// protected functions --
//

//////////////////////////////
//
// Binasc::outputStyleAscii -- read an input file and output bytes in ascii
//    form, not displaying any blank lines.  Output words are not
//    broken unless they are longer than 75 characters.
//

int Binasc::outputStyleAscii(std::ostream& out, std::istream& input) {
	uchar outputWord[256] = {0};   // storage for current word
	int index     = 0;             // current length of word
	int lineCount = 0;             // current length of line
	int type      = 0;             // 0=space, 1=printable
	uchar ch;                      // current input byte

	ch = input.get();
	while (!input.eof()) {
		int lastType = type;
		type = (isprint(ch) && !isspace(ch)) ? 1 : 0;

		if ((type == 1) && (lastType == 0)) {
			// start of a new word.  check where to put old word
			if (index + lineCount >= m_maxLineLength) {  // put on next line
				outputWord[index] = '\0';
				out << '\n' << outputWord;
				lineCount = index;
				index = 0;
			} else {                                   // put on current line
				outputWord[index] = '\0';
				if (lineCount != 0) {
					out << ' ';
					lineCount++;
				}
				out << outputWord;
				lineCount += index;
				index = 0;
			}
		}
		if (type == 1) {
			outputWord[index++] = ch;
		}
		ch = input.get();
	}

	if (index != 0) {
		out << std::endl;
	}

	return 1;
}



//////////////////////////////
//
// Binasc::outputStyleBinary -- read an input binary file and output bytes
//     in ascii form, hexadecimal numbers only.
//

int Binasc::outputStyleBinary(std::ostream& out, std::istream& input) {
	int currentByte = 0;    // current byte output in line
	uchar ch;               // current input byte

	ch = input.get();
	if (input.eof()) {
		std::cerr << "End of the file right away!" << std::endl;
		return 0;
	}

	while (!input.eof()) {
		if (ch < 0x10) {
			out << '0';
		}
		out << std::hex << (int)ch << ' ';
		currentByte++;
		if (currentByte >= m_maxLineBytes) {
			out << '\n';
			currentByte = 0;
		}
		ch = input.get();
	}

	if (currentByte != 0) {
		out << std::endl;
	}

	return 1;
}



//////////////////////////////
//
// Binasc::outputStyleBoth -- read an input file and output bytes in ASCII
//     form with both hexadecimal numbers and ascii representation
//

int Binasc::outputStyleBoth(std::ostream& out, std::istream& input) {
	uchar asciiLine[256] = {0};    // storage for output line
	int currentByte = 0;           // current byte output in line
	int index = 0;                 // current character in asciiLine
	uchar ch;                      // current input byte

	ch = input.get();
	while (!input.eof()) {
		if (index == 0) {
			asciiLine[index++] = ';';
			out << ' ';
		}
		if (ch < 0x10) {
			out << '0';
		}
		out << std::hex << (int)ch << ' ';
		currentByte++;

		asciiLine[index++] = ' ';
		if (isprint(ch)) {
			asciiLine[index++] = ch;
		} else {
			asciiLine[index++] = ' ';
		}
		asciiLine[index++] = ' ';

		if (currentByte >= m_maxLineBytes) {
			out << '\n';
			asciiLine[index] = '\0';
			out << asciiLine << "\n\n";
			currentByte = 0;
			index = 0;
		}
		ch = input.get();
	}

	if (currentByte != 0) {
		out << '\n';
		asciiLine[index] = '\0';
		out << asciiLine << '\n' << std::endl;
	}

	return 1;
}



///////////////////////////////
//
// processLine -- read a line of input and output any specified bytes
//

int Binasc::processLine(std::ostream& out, const std::string& input,
		int lineCount) {
	int status = 1;
	int i = 0;
	int length = (int)input.size();
	std::string word;
	while (i<length) {
		if ((input[i] == ';') || (input[i] == '#') || (input[i] == '/')) {
			// comment to end of line, so ignore
			return 1;
		} else if ((input[i] == ' ') || (input[i] == '\n')
				|| (input[i] == '\t')) {
			// ignore whitespace
			i++;
			continue;
		} else if (input[i] == '+') {
			i = getWord(word, input, " \n\t", i);
			status = processAsciiWord(out, word, lineCount);
		} else if (input[i] == '"') {
			i = getWord(word, input, "\"", i);
			status = processStringWord(out, word, lineCount);
		} else if (input[i] == 'v') {
			i = getWord(word, input, " \n\t", i);
			status = processVlvWord(out, word, lineCount);
		} else if (input[i] == 'p') {
			i = getWord(word, input, " \n\t", i);
			status = processMidiPitchBendWord(out, word, lineCount);
		} else if (input[i] == 't') {
			i = getWord(word, input, " \n\t", i);
			status = processMidiTempoWord(out, word, lineCount);
		} else {
			i = getWord(word, input, " \n\t", i);
			if (word.find('\'') != std::string::npos) {
				status = processDecimalWord(out, word, lineCount);
			} else if ((word.find(',') != std::string::npos)
					|| (word.size() > 2)) {
				status = processBinaryWord(out, word, lineCount);
			} else {
				status = processHexWord(out, word, lineCount);
			}
		}

		if (status == 0) {
			return 0;
		}

	}

	return 1;
}



//////////////////////////////
//
// Binasc::getWord -- extract a sub string, stopping at any of the given
//   terminator characters.
//

int Binasc::getWord(std::string& word, const std::string& input,
		const std::string& terminators, int index) {
	word.resize(0);
	int i = index;
	int escape = 0;
	int ecount = 0;
	if (terminators.find('"') != std::string::npos) {
		escape = 1;
	}
	while (i < (int)input.size()) {
		if (escape && input[i] == '\"') {
			ecount++;
			i++;
			if (ecount >= 2) {
				break;
			}
		}
		if (escape && (i<(int)input.size()-1) && (input[i] == '\\')
				&& (input[i+1] == '"')) {
			word.push_back(input[i+1]);
			i += 2;
		} else if (terminators.find(input[i]) == std::string::npos) {
			word.push_back(input[i]);
			i++;
		} else {
			i++;
			return i;
		}
	}
	return i;
}



///////////////////////////////
//
// Binasc::getVLV -- read a Variable-Length Value from the file
//

int Binasc::getVLV(std::istream& infile, int& trackbytes) {
	int output = 0;
	uchar ch;
	infile.read((char*)&ch, 1);
	trackbytes++;
	output = (output << 7) | (0x7f & ch);
	while (ch >= 0x80) {
		infile.read((char*)&ch, 1);
		trackbytes++;
		output = (output << 7) | (0x7f & ch);
	}
	return output;
}



//////////////////////////////
//
// Binasc::readMidiEvent -- Read a delta time and then a MIDI message
//     (or meta message).  Returns 1 if not end-of-track meta message;
//     0 otherwise.
//

int Binasc::readMidiEvent(std::ostream& out, std::istream& infile,
		int& trackbytes, int& command) {

	// Read and print Variable Length Value for delta ticks
	int vlv = getVLV(infile, trackbytes);

	std::stringstream output;

	output << "v" << std::dec << vlv << "\t";

	std::string comment;

	int status = 1;
	uchar ch;
	char byte1, byte2;
	infile.read((char*)&ch, 1);
	trackbytes++;
	if (ch < 0x80) {
		// running status: command byte is previous one in data stream
		output << "   ";
	} else {
		// midi command byte
		output << std::hex << (int)ch;
		command = ch;
		infile.read((char*)&ch, 1);
		trackbytes++;
	}
	byte1 = ch;
	switch (command & 0xf0) {
		case 0x80:    // note-off: 2 bytes
			output << " '" << std::dec << (int)byte1;
			infile.read((char*)&ch, 1);
			trackbytes++;
			byte2 = ch;
			output << " '" << std::dec << (int)byte2;
			if (m_commentsQ) {
				comment += "note-off " + keyToPitchName(byte1);
			}
			break;
		case 0x90:    // note-on: 2 bytes
			output << " '" << std::dec << (int)byte1;
			infile.read((char*)&ch, 1);
			trackbytes++;
			byte2 = ch;
			output << " '" << std::dec << (int)byte2;
			if (m_commentsQ) {
				if (byte2 == 0) {
					comment += "note-off " + keyToPitchName(byte1);
				} else {
					comment += "note-on " + keyToPitchName(byte1);
				}
			}
			break;
		case 0xA0:    // aftertouch: 2 bytes
			output << " '" << std::dec << (int)byte1;
			infile.read((char*)&ch, 1);
			trackbytes++;
			byte2 = ch;
			output << " '" << std::dec << (int)byte2;
			if (m_commentsQ) {
				comment += "after-touch";
			}
			break;
		case 0xB0:    // continuous controller: 2 bytes
			output << " '" << std::dec << (int)byte1;
			infile.read((char*)&ch, 1);
			trackbytes++;
			byte2 = ch;
			output << " '" << std::dec << (int)byte2;
			if (m_commentsQ) {
				comment += "controller";
			}
			break;
		case 0xE0:    // pitch-bend: 2 bytes
			output << " '" << std::dec << (int)byte1;
			infile.read((char*)&ch, 1);
			trackbytes++;
			byte2 = ch;
			output << " '" << std::dec << (int)byte2;
			if (m_commentsQ) {
				comment += "pitch-bend";
			}
			break;
		case 0xC0:    // patch change: 1 bytes
			output << " '" << std::dec << (int)byte1;
			if (m_commentsQ) {
				output << "\t";
				comment += "patch-change";
			}
			break;
		case 0xD0:    // channel pressure: 1 bytes
			output << " '" << std::dec << (int)byte1;
			if (m_commentsQ) {
				comment += "channel pressure";
			}
			break;
		case 0xF0:    // various system bytes: variable bytes
			switch (command) {
				case 0xf0:
					break;
				case 0xf7:
					// Read the first byte which is either 0xf0 or 0xf7.
					// Then a VLV byte count for the number of bytes
					// that remain in the message will follow.
					// Then read that number of bytes.
					{
					infile.putback(byte1);
					trackbytes--;
					int length = getVLV(infile, trackbytes);
					output << " v" << std::dec << length;
					for (int i=0; i<length; i++) {
						infile.read((char*)&ch, 1);
						trackbytes++;
						if (ch < 0x10) {
						   output << " 0" << std::hex << (int)ch;
						} else {
						   output << " " << std::hex << (int)ch;
						}
					}
					}
					break;
				case 0xf1:
					break;
				case 0xf2:
					break;
				case 0xf3:
					break;
				case 0xf4:
					break;
				case 0xf5:
					break;
				case 0xf6:
					break;
				case 0xf8:
					break;
				case 0xf9:
					break;
				case 0xfa:
					break;
				case 0xfb:
					break;
				case 0xfc:
					break;
				case 0xfd:
					break;
				case 0xfe:
					std::cerr << "Error command not yet handled" << std::endl;
					return 0;
					break;
				case 0xff:  // meta message
					{
					int metatype = ch;
					output << " " << std::hex << metatype;
					int length = getVLV(infile, trackbytes);
					output << " v" << std::dec << length;
					switch (metatype) {

						case 0x00:  // sequence number
						   // display two-byte big-endian decimal value.
						   {
						   infile.read((char*)&ch, 1);
						   trackbytes++;
						   int number = ch;
						   infile.read((char*)&ch, 1);
						   trackbytes++;
						   number = (number << 8) | ch;
						   output << " 2'" << number;
						   }
						   break;

						case 0x20: // MIDI channel prefix
						case 0x21: // MIDI port
						   // display single-byte decimal number
						   infile.read((char*)&ch, 1);
						   trackbytes++;
						   output << " '" << (int)ch;
						   break;

						case 0x51: // Tempo
						    // display tempo as "t" word.
						    {
						    int number = 0;
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    number = (number << 8) | ch;
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    number = (number << 8) | ch;
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    number = (number << 8) | ch;
						    double tempo = 1000000.0 / number * 60.0;
						    output << " t" << tempo;
						    }
						    break;

						case 0x54: // SMPTE offset
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // hour
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // minutes
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // seconds
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // frames
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // subframes
						    break;

						case 0x58: // time signature
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // numerator
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // denominator power
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // clocks per beat
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // 32nd notes per beat
						    break;

						case 0x59: // key signature
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // accidentals
						    infile.read((char*)&ch, 1);
						    trackbytes++;
						    output << " '" << (int)ch;  // mode
						    break;

						case 0x01: // text
						case 0x02: // copyright
						case 0x03: // track name
						case 0x04: // instrument name
						case 0x05: // lyric
						case 0x06: // marker
						case 0x07: // cue point
						case 0x08: // program name
						case 0x09: // device name
						   output << " \"";
						   for (int i=0; i<length; i++) {
						      infile.read((char*)&ch, 1);
						      trackbytes++;
						      output << (char)ch;
						   }
						   output << "\"";
						   break;
						default:
						   for (int i=0; i<length; i++) {
						      infile.read((char*)&ch, 1);
						      trackbytes++;
						      output << " ";
						      if (ch < 0x10) {
						         output << "0";
						      }
						      output << std::hex << (int)ch;
						   }
					}
					switch (metatype) {
						case 0x00: comment += "sequence number";     break;
						case 0x01: comment += "text";                break;
						case 0x02: comment += "copyright notice";    break;
						case 0x03: comment += "track name";          break;
						case 0x04: comment += "instrument name";     break;
						case 0x05: comment += "lyric";               break;
						case 0x06: comment += "marker";              break;
						case 0x07: comment += "cue point";           break;
						case 0x08: comment += "program name";        break;
						case 0x09: comment += "device name";         break;
						case 0x20: comment += "MIDI channel prefix"; break;
						case 0x21: comment += "MIDI port";           break;
						case 0x51: comment += "tempo";               break;
						case 0x54: comment += "SMPTE offset";        break;
						case 0x58: comment += "time signature";      break;
						case 0x59: comment += "key signature";       break;
						case 0x7f: comment += "system exclusive";    break;
						case 0x2f:
						   status = 0;
						   comment += "end-of-track";
						   break;
						default:
						   comment += "meta-message";
					}
					}
					break;

			}
			break;
	}

	out << output.str();
	if (m_commentsQ) {
		out << "\t; " << comment;
	}

	return status;
}



/////////////////////////////
//
// Binasc::keyToPitchName -- Convert a MIDI key number to scientific
//     pitch notation.
//

std::string Binasc::keyToPitchName(int key) {
	int pc = key % 12;
	int octave = key / 12 - 1;
	std::stringstream output;
	switch (pc) {
		case  0: output << "C";  break;
		case  1: output << "C#"; break;
		case  2: output << "D";  break;
		case  3: output << "D#"; break;
		case  4: output << "E";  break;
		case  5: output << "F";  break;
		case  6: output << "F#"; break;
		case  7: output << "G";  break;
		case  8: output << "G#"; break;
		case  9: output << "A";  break;
		case 10: output << "A#"; break;
		case 11: output << "B";  break;
	}
	output << octave;
	return output.str().c_str();
}



//////////////////////////////
//
// Binasc::outputStyleMidi -- Read an input file and output bytes parsed
//     as a MIDI file (return false if not a MIDI file).
//

int Binasc::outputStyleMidi(std::ostream& out, std::istream& input) {
	uchar ch;                      // current input byte
	std::stringstream tempout;
	input.read((char*)&ch, 1);

	if (input.eof()) {
		std::cerr << "End of the file right away!" << std::endl;
		return 0;
	}

	// Read the MIDI file header:

	// The first four bytes must be the characters "MThd"
	if (ch != 'M') { std::cerr << "Not a MIDI file M" << std::endl; return 0; }
	input.read((char*)&ch, 1);
	if (ch != 'T') { std::cerr << "Not a MIDI file T" << std::endl; return 0; }
	input.read((char*)&ch, 1);
	if (ch != 'h') { std::cerr << "Not a MIDI file h" << std::endl; return 0; }
	input.read((char*)&ch, 1);
	if (ch != 'd') { std::cerr << "Not a MIDI file d" << std::endl; return 0; }
	tempout << "\"MThd\"";
	if (m_commentsQ) {
		tempout << "\t\t\t; MIDI header chunk marker";
	}
	tempout << std::endl;

	// The next four bytes are a big-endian byte count for the header
	// which should nearly always be "6".
	int headersize = 0;
	input.read((char*)&ch, 1); headersize = (headersize << 8) | ch;
	input.read((char*)&ch, 1); headersize = (headersize << 8) | ch;
	input.read((char*)&ch, 1); headersize = (headersize << 8) | ch;
	input.read((char*)&ch, 1); headersize = (headersize << 8) | ch;
	tempout << "4'" << headersize;
	if (m_commentsQ) {
		tempout << "\t\t\t; bytes to follow in header chunk";
	}
	tempout << std::endl;

	// First number in header is two-byte file type.
	int filetype = 0;
	input.read((char*)&ch, 1);
	filetype = (filetype << 8) | ch;
	input.read((char*)&ch, 1);
	filetype = (filetype << 8) | ch;
	tempout << "2'" << filetype;
	if (m_commentsQ) {
		tempout << "\t\t\t; file format: Type-" << filetype << " (";
		switch (filetype) {
			case 0:  tempout << "single track"; break;
			case 1:  tempout << "multitrack";   break;
			case 2:  tempout << "multisegment"; break;
			default: tempout << "unknown";      break;
		}
		tempout << ")";
	}
	tempout << std::endl;

	// Second number in header is two-byte trackcount.
	int trackcount = 0;
	input.read((char*)&ch, 1);
	trackcount = (trackcount << 8) | ch;
	input.read((char*)&ch, 1);
	trackcount = (trackcount << 8) | ch;
	tempout << "2'" << trackcount;
	if (m_commentsQ) {
		tempout << "\t\t\t; number of tracks";
	}
	tempout << std::endl;

	// Third number is divisions.  This can be one of two types:
	// regular: top bit is 0: number of ticks per quarter note
	// SMPTE:   top bit is 1: first byte is negative frames, second is
	//          ticks per frame.
	uchar byte1;
	uchar byte2;
	input.read((char*)&byte1, 1);
	input.read((char*)&byte2, 1);
	if (byte1 & 0x80) {
		// SMPTE divisions
		tempout << "'-" << 0xff - (ulong)byte1 + 1;
		if (m_commentsQ) {
			tempout << "\t\t\t; SMPTE frames/second";
		}
		tempout << std::endl;
		tempout << "'" << std::dec << (long)byte2;
		if (m_commentsQ) {
			tempout << "\t\t\t; subframes per frame";
		}
		tempout << std::endl;
	} else {
		// regular divisions
		int divisions = 0;
		divisions = (divisions << 8) | byte1;
		divisions = (divisions << 8) | byte2;
		tempout << "2'" << divisions;
		if (m_commentsQ) {
			tempout << "\t\t\t; ticks per quarter note";
		}
		tempout << std::endl;
	}

	// Print any strange bytes in header:
	int i;
	for (i=0; i<headersize - 6; i++) {
		input.read((char*)&ch, 1);
		if (ch < 0x10) {
			tempout << '0';
		}
		tempout << std::hex << (int)ch;
	}
	if (headersize - 6 > 0) {
		tempout << "\t\t\t; unknown header bytes";
		tempout << std::endl;
	}

	for (i=0; i<trackcount; i++) {
		tempout << "\n;;; TRACK "
				  << i << " ----------------------------------" << std::endl;

		input.read((char*)&ch, 1);
		// The first four bytes of a track must be the characters "MTrk"
		if (ch != 'M') { std::cerr << "Not a MIDI file M2" << std::endl; return 0; }
		input.read((char*)&ch, 1);
		if (ch != 'T') { std::cerr << "Not a MIDI file T2" << std::endl; return 0; }
		input.read((char*)&ch, 1);
		if (ch != 'r') { std::cerr << "Not a MIDI file r" << std::endl; return 0; }
		input.read((char*)&ch, 1);
		if (ch != 'k') { std::cerr << "Not a MIDI file k" << std::endl; return 0; }
		tempout << "\"MTrk\"";
		if (m_commentsQ) {
			tempout << "\t\t\t; MIDI track chunk marker";
		}
		tempout << std::endl;

		// The next four bytes are a big-endian byte count for the track
		int tracksize = 0;
		input.read((char*)&ch, 1); tracksize = (tracksize << 8) | ch;
		input.read((char*)&ch, 1); tracksize = (tracksize << 8) | ch;
		input.read((char*)&ch, 1); tracksize = (tracksize << 8) | ch;
		input.read((char*)&ch, 1); tracksize = (tracksize << 8) | ch;
		tempout << "4'" << tracksize;
		if (m_commentsQ) {
			tempout << "\t\t\t; bytes to follow in track chunk";
		}
		tempout << std::endl;

		int trackbytes = 0;
		int command = 0;

		// process MIDI events until the end of the track
		while (readMidiEvent(tempout, input, trackbytes, command)) {
			tempout << "\n";
		};
		tempout << "\n";

		if (trackbytes != tracksize) {
			tempout << "; TRACK SIZE ERROR, ACTUAL SIZE: " << trackbytes << std::endl;
		}
	}

	// print #define definitions if requested.


	// print main content of MIDI file parsing:
	out << tempout.str();
	return 1;
}



//////////////////////////////
//
// Binasc::processDecimalWord -- interprets a decimal word into
//     constituent bytes
//

int Binasc::processDecimalWord(std::ostream& out, const std::string& word,
		int lineNum) {
	int length = (int)word.size();        // length of ascii binary number
	int byteCount = -1;              // number of bytes to output
	int quoteIndex = -1;             // index of decimal specifier
	int signIndex = -1;              // index of any sign for number
	int periodIndex = -1;            // index of period for floating point
	int endianIndex = -1;            // index of little endian specifier
	int i = 0;

	// make sure that all characters are valid
	for (i=0; i<length; i++) {
		switch (word[i]) {
			case '\'':
				if (quoteIndex != -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "extra quote in decimal number" << std::endl;
					return 0;
				} else {
					quoteIndex = i;
				}
				break;
			case '-':
				if (signIndex != -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "cannot have more than two minus signs in number"
						  << std::endl;
					return 0;
				} else {
					signIndex = i;
				}
				if (i == 0 || word[i-1] != '\'') {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "minus sign must immediately follow quote mark" << std::endl;
					return 0;
				}
				break;
			case '.':
				if (quoteIndex == -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "cannot have decimal marker before quote" << std::endl;
					return 0;
				}
				if (periodIndex != -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "extra period in decimal number" << std::endl;
					return 0;
				} else {
					periodIndex = i;
				}
				break;
			case 'u':
			case 'U':
				if (quoteIndex != -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "cannot have endian specified after quote" << std::endl;
					return 0;
				}
				if (endianIndex != -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "extra \"u\" in decimal number" << std::endl;
					return 0;
				} else {
					endianIndex = i;
				}
				break;
			case '8':
			case '1': case '2': case '3': case '4':
				if (quoteIndex == -1 && byteCount != -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "invalid byte specificaton before quote in "
						  << "decimal number" << std::endl;
					return 0;
				} else if (quoteIndex == -1) {
					byteCount = word[i] - '0';
				}
				break;
			case '0': case '5': case '6': case '7': case '9':
				if (quoteIndex == -1) {
					std::cerr << "Error on line " << lineNum << " at token: " << word
						  << std::endl;
					std::cerr << "cannot have numbers before quote in decimal number"
						  << std::endl;
					return 0;
				}
				break;
			default:
				std::cerr << "Error on line " << lineNum << " at token: " << word
					  << std::endl;
				std::cerr << "Invalid character in decimal number"
						  " (character number " << i <<")" << std::endl;
				return 0;
		}
	}

	// there must be a quote character to indicate a decimal number
	// and there must be a decimal number after the quote
	if (quoteIndex == -1) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "there must be a quote to signify a decimal number" << std::endl;
		return 0;
	} else if (quoteIndex == length - 1) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "there must be a decimal number after the quote" << std::endl;
		return 0;
	}

	// 8 byte decimal output can only occur if reading a double number
	if (periodIndex == -1 && byteCount == 8) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "only floating-point numbers can use 8 bytes" << std::endl;
		return 0;
	}

	// default size for floating point numbers is 4 bytes
	if (periodIndex != -1) {
		if (byteCount == -1) {
			byteCount = 4;
		}
	}

	// process any floating point numbers possibilities
	if (periodIndex != -1) {
		double doubleOutput = atof(&word[quoteIndex+1]);
		float  floatOutput  = (float)doubleOutput;
		switch (byteCount) {
			case 4:
			  if (endianIndex == -1) {
				  writeBigEndianFloat(out, floatOutput);
			  } else {
				  writeLittleEndianFloat(out, floatOutput);
			  }
			  return 1;
			  break;
			case 8:
			  if (endianIndex == -1) {
				  writeBigEndianDouble(out, doubleOutput);
			  } else {
				  writeLittleEndianDouble(out, doubleOutput);
			  }
			  return 1;
			  break;
			default:
				std::cerr << "Error on line " << lineNum << " at token: " << word
					  << std::endl;
				std::cerr << "floating-point numbers can be only 4 or 8 bytes" << std::endl;
				return 0;
		}
	}

	// process any integer decimal number possibilities

	// default integer size is one byte, if size is not specified, then
	// the number must be in the one byte range and cannot overflow
	// the byte if the size of the decimal number is not specified
	if (byteCount == -1) {
		if (signIndex != -1) {
			long tempLong = atoi(&word[quoteIndex + 1]);
			if (tempLong > 127 || tempLong < -128) {
				std::cerr << "Error on line " << lineNum << " at token: " << word
					  << std::endl;
				std::cerr << "Decimal number out of range from -128 to 127" << std::endl;
				return 0;
			}
			char charOutput = (char)tempLong;
			out << charOutput;
			return 1;
		} else {
			ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
			uchar ucharOutput = (uchar)tempLong;
			if (tempLong > 255) { // || (tempLong < 0)) {
				std::cerr << "Error on line " << lineNum << " at token: " << word
					  << std::endl;
				std::cerr << "Decimal number out of range from 0 to 255" << std::endl;
				return 0;
			}
			out << ucharOutput;
			return 1;
		}
	}

	// left with an integer number with a specified number of bytes
	switch (byteCount) {
		case 1:
			if (signIndex != -1) {
				long tempLong = atoi(&word[quoteIndex + 1]);
				char charOutput = (char)tempLong;
				out << charOutput;
				return 1;
			} else {
				ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
				uchar ucharOutput = (uchar)tempLong;
				out << ucharOutput;
				return 1;
			}
			break;
		case 2:
			if (signIndex != -1) {
				long tempLong = atoi(&word[quoteIndex + 1]);
				short shortOutput = (short)tempLong;
				if (endianIndex == -1) {
					writeBigEndianShort(out, shortOutput);
				} else {
					writeLittleEndianShort(out, shortOutput);
				}
				return 1;
			} else {
				ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
				ushort ushortOutput = (ushort)tempLong;
				if (endianIndex == -1) {
					writeBigEndianUShort(out, ushortOutput);
				} else {
					writeLittleEndianUShort(out, ushortOutput);
				}
				return 1;
			}
			break;
		case 3:
			{
			if (signIndex != -1) {
				std::cerr << "Error on line " << lineNum << " at token: " << word
					  << std::endl;
				std::cerr << "negative decimal numbers cannot be stored in 3 bytes"
					  << std::endl;
				return 0;
			}
			ulong tempLong = (ulong)atoi(&word[quoteIndex + 1]);
			uchar byte1 = (uchar)((tempLong & 0x00ff0000) >> 16);
			uchar byte2 = (uchar)((tempLong & 0x0000ff00) >> 8);
			uchar byte3 = (uchar)((tempLong & 0x000000ff));
			if (endianIndex == -1) {
				out << byte1;
				out << byte2;
				out << byte3;
			} else {
				out << byte3;
				out << byte2;
				out << byte1;
			}
			return 1;
			}
			break;
		case 4:
			if (signIndex != -1) {
				long tempLong = atoi(&word[quoteIndex + 1]);
				if (endianIndex == -1) {
					writeBigEndianLong(out, tempLong);
				} else {
					writeLittleEndianLong(out, tempLong);
				}
				return 1;
			} else {
				ulong tempuLong = (ulong)atoi(&word[quoteIndex + 1]);
				if (endianIndex == -1) {
					writeBigEndianULong(out, tempuLong);
				} else {
					writeLittleEndianULong(out, tempuLong);
				}
				return 1;
			}
			break;
		default:
			std::cerr << "Error on line " << lineNum << " at token: " << word
				  << std::endl;
			std::cerr << "invalid byte count specification for decimal number" << std::endl;
			return 0;
	}

	return 1;
}



//////////////////////////////
//
// Binasc::processHexWord -- interprets a hexadecimal word and converts into
//     its binary byte form.
//

int Binasc::processHexWord(std::ostream& out, const std::string& word,
		int lineNum) {
	int length = (int)word.size();
	uchar outputByte;

	if (length > 2) {
		std::cerr << "Error on line " << lineNum << " at token: " << word << std::endl;
		std::cerr << "Size of hexadecimal number is too large.  Max is ff." << std::endl;
		return 0;
	}

	if (!isxdigit(word[0]) || (length == 2 && !isxdigit(word[1]))) {
		std::cerr << "Error on line " << lineNum << " at token: " << word << std::endl;
		std::cerr << "Invalid character in hexadecimal number." << std::endl;
		return 0;
	}

	outputByte = (uchar)strtol(word.c_str(), (char**)NULL, 16);
	out << outputByte;
	return 1;
}



//////////////////////////////
//
// Binasc::processStringWord -- interprets a binary word into
//     its constituent byte
//

int Binasc::processStringWord(std::ostream& out, const std::string& word,
		int /* lineNum */) {
	out << word;
	return 1;
}



//////////////////////////////
//
// Binasc::processAsciiWord -- interprets a binary word into
//     its constituent byte
//

int Binasc::processAsciiWord(std::ostream& out, const std::string& word,
		int lineNum) {
	int length = (int)word.size();
	uchar outputByte;

	if (word[0] != '+') {
		std::cerr << "Error on line " << lineNum << " at token: " << word << std::endl;
		std::cerr << "character byte must start with \'+\' sign: " << std::endl;
		return 0;
	}

	if (length > 2) {
		std::cerr << "Error on line " << lineNum << " at token: " << word << std::endl;
		std::cerr << "character byte word is too long -- specify only one character"
			  << std::endl;
		return 0;
	}

	if (length == 2) {
		outputByte = (uchar)word[1];
	} else {
		outputByte = ' ';
	}
	out << outputByte;
	return 1;
}



//////////////////////////////
//
// Binasc::processBinaryWord -- interprets a binary word into
//     its constituent byte
//

int Binasc::processBinaryWord(std::ostream& out, const std::string& word,
		int lineNum) {
	int length = (int)word.size();        // length of ascii binary number
	int commaIndex = -1;             // index location of comma in number
	int leftDigits = -1;             // number of digits to left of comma
	int rightDigits = -1;            // number of digits to right of comma
	int i = 0;

	// make sure that all characters are valid
	for (i=0; i<length; i++) {
		if (word [i] == ',') {
			if (commaIndex != -1) {
				std::cerr << "Error on line " << lineNum << " at token: " << word
					  << std::endl;
				std::cerr << "extra comma in binary number" << std::endl;
				return 0;
			} else {
				commaIndex = i;
			}
		} else if (!(word[i] == '1' || word[i] == '0')) {
			std::cerr << "Error on line " << lineNum << " at token: " << word
				  << std::endl;
			std::cerr << "Invalid character in binary number"
					  " (character is " << word[i] <<")" << std::endl;
			return 0;
		}
	}

	// comma cannot start or end number
	if (commaIndex == 0) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "cannot start binary number with a comma" << std::endl;
		return 0;
	} else if (commaIndex == length - 1 ) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "cannot end binary number with a comma" << std::endl;
		return 0;
	}

	// figure out how many digits there are in binary number
	// number must be able to fit into one byte.
	if (commaIndex != -1) {
		leftDigits = commaIndex;
		rightDigits = length - commaIndex - 1;
	} else if (length > 8) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "too many digits in binary number" << std::endl;
		return 0;
	}
	// if there is a comma, then there cannot be more than 4 digits on a side
	if (leftDigits > 4) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "too many digits to left of comma" << std::endl;
		return 0;
	}
	if (rightDigits > 4) {
		std::cerr << "Error on line " << lineNum << " at token: " << word
			  << std::endl;
		std::cerr << "too many digits to right of comma" << std::endl;
		return 0;
	}

	// OK, we have a valid binary number, so calculate the byte

	uchar output = 0;

	// if no comma in binary number
	if (commaIndex == -1) {
		for (i=0; i<length; i++) {
			output = output << 1;
			output |= word[i] - '0';
		}
	}
	// if comma in binary number
	else {
		for (i=0; i<leftDigits; i++) {
			output = output << 1;
			output |= word[i] - '0';
		}
		output = output << (4-rightDigits);
		for (i=0+commaIndex+1; i<rightDigits+commaIndex+1; i++) {
			output = output << 1;
			output |= word[i] - '0';
		}
	}

	// send the byte to the output
	out << output;
	return 1;
}



//////////////////////////////
//
// Binasc::processVlvWord -- print a number in Variable Length Value form.
//   The int is split into 7-bit groupings, the MSB's that are zero
//   are dropped.  A continuation bit is added as the MSbit to each
//   7-bit grouping.  The continuation bit is "1" if there is another
//   byte in the VLV; "0" for the last byte.  VLVs are always
//   big-endian.  The input word starts with the character "v" followed
//   without space by an integer.
//

int Binasc::processVlvWord(std::ostream& out, const std::string& word,
		int lineNum) {
	if (word.size() < 2) {
		std::cerr << "Error on line: " << lineNum
			  << ": 'v' needs to be followed immediately by a decimal digit"
			  << std::endl;
		return 0;
	}
	if (!isdigit(word[1])) {
		std::cerr << "Error on line: " << lineNum
			  << ": 'v' needs to be followed immediately by a decimal digit"
			  << std::endl;
		return 0;
	}
	ulong value = atoi(&word[1]);

	uchar byte[5];
	byte[0] = (value >> 28) & 0x7f;
	byte[1] = (value >> 21) & 0x7f;
	byte[2] = (value >> 14) & 0x7f;
	byte[3] = (value >>  7) & 0x7f;
	byte[4] = (value >>  0) & 0x7f;

	int i;
	int flag = 0;
	for (i=0; i<4; i++) {
		if (byte[i] != 0) {
			flag = 1;
		}
		if (flag) {
			byte[i] |= 0x80;
		}
	}

	for (i=0; i<5; i++) {
		if (byte[i] >= 0x80 || i == 4) {
			out << byte[i];
		}
	}

	return 1;
}



////////////////////////////
//
// Binasc::processMidiTempoWord -- convert a floating point tempo into
//   a three-byte number of microseconds per beat per minute value.
//

int Binasc::processMidiTempoWord(std::ostream& out, const std::string& word,
		int lineNum) {
	if (word.size() < 2) {
		std::cerr << "Error on line: " << lineNum
			  << ": 't' needs to be followed immediately by "
			  << "a floating-point number" << std::endl;
		return 0;
	}
	if (!(isdigit(word[1]) || word[1] == '.' || word[1] == '-'
			|| word[1] == '+')) {
		std::cerr << "Error on line: " << lineNum
			  << ": 't' needs to be followed immediately by "
			  << "a floating-point number" << std::endl;
		return 0;
	}
	double value = strtod(&word[1], NULL);

	if (value < 0.0) {
		value = -value;
	}

	int intval = int(60.0 * 1000000.0 / value + 0.5);

	uchar byte0 = intval & 0xff;
	uchar byte1 = (intval >>  8) & 0xff;
	uchar byte2 = (intval >> 16) & 0xff;
	out << byte2 << byte1 << byte0;
	return 1;
}



////////////////////////////
//
// Binasc::processMidiPitchBendWord -- convert a floating point number in
//   the range from +1.0 to -1.0 into a 14-point integer with -1.0 mapping
//   to 0 and +1.0 mapping to 2^15-1.  This integer will be packed into
//   two bytes, with the LSB coming first and containing the bottom
//   7-bits of the 14-bit value, then the MSB coming second and containing
//   the top 7-bits of the 14-bit value.

int Binasc::processMidiPitchBendWord(std::ostream& out, const std::string& word,
		int lineNum) {
	if (word.size() < 2) {
		std::cerr << "Error on line: " << lineNum
			  << ": 'p' needs to be followed immediately by "
			  << "a floating-point number" << std::endl;
		return 0;
	}
	if (!(isdigit(word[1]) || word[1] == '.' || word[1] == '-'
			|| word[1] == '+')) {
		std::cerr << "Error on line: " << lineNum
			  << ": 'p' needs to be followed immediately by "
			  << "a floating-point number" << std::endl;
		return 0;
	}
	double value = strtod(&word[1], NULL);

	if (value > 1.0) {
		value = 1.0;
	}
	if (value < -1.0) {
		value = -1.0;
	}

	int intval = (int)(((1 << 13)-0.5)  * (value + 1.0) + 0.5);
	uchar LSB = intval & 0x7f;
	uchar MSB = (intval >>  7) & 0x7f;
	out << LSB << MSB;
	return 1;
}



///////////////////////////////////////////////////////////////////////////
//
// Ordered byte writing functions --
//

//////////////////////////////
//
// Binasc::writeLittleEndianUShort --
//

std::ostream& Binasc::writeLittleEndianUShort(std::ostream& out, ushort value) {
	union { char bytes[2]; ushort us; } data;
	data.us = value;
	out << data.bytes[0];
	out << data.bytes[1];
	return out;
}



//////////////////////////////
//
// Binasc::writeBigEndianUShort --
//

std::ostream& Binasc::writeBigEndianUShort(std::ostream& out, ushort value) {
	union { char bytes[2]; ushort us; } data;
	data.us = value;
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// Binasc::writeLittleEndianShort --
//

std::ostream& Binasc::writeLittleEndianShort(std::ostream& out, short value) {
	union { char bytes[2]; short s; } data;
	data.s = value;
	out << data.bytes[0];
	out << data.bytes[1];
	return out;
}



//////////////////////////////
//
// writeBigEndianShort --
//

std::ostream& Binasc::writeBigEndianShort(std::ostream& out, short value) {
	union { char bytes[2]; short s; } data;
	data.s = value;
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// Binasc::writeLittleEndianULong --
//

std::ostream& Binasc::writeLittleEndianULong(std::ostream& out, ulong value) {
	union { char bytes[4]; ulong ul; } data;
	data.ul = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	return out;
}



//////////////////////////////
//
// Binasc::writeBigEndianULong --
//

std::ostream& Binasc::writeBigEndianULong(std::ostream& out, ulong value) {
	union { char bytes[4]; long ul; } data;
	data.ul = value;
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// Binasc::writeLittleEndianLong --
//

std::ostream& Binasc::writeLittleEndianLong(std::ostream& out, long value) {
	union { char bytes[4]; long l; } data;
	data.l = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	return out;
}



//////////////////////////////
//
// Binasc::writeBigEndianLong --
//

std::ostream& Binasc::writeBigEndianLong(std::ostream& out, long value) {
	union { char bytes[4]; long l; } data;
	data.l = value;
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;

}



//////////////////////////////
//
// Binasc::writeBigEndianFloat --
//

std::ostream& Binasc::writeBigEndianFloat(std::ostream& out, float value) {
	union { char bytes[4]; float f; } data;
	data.f = value;
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// Binasc::writeLittleEndianFloat --
//

std::ostream& Binasc::writeLittleEndianFloat(std::ostream& out, float value) {
	union { char bytes[4]; float f; } data;
	data.f = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	return out;
}



//////////////////////////////
//
// Binasc::writeBigEndianDouble --
//

std::ostream& Binasc::writeBigEndianDouble(std::ostream& out, double value) {
	union { char bytes[8]; double d; } data;
	data.d = value;
	out << data.bytes[7];
	out << data.bytes[6];
	out << data.bytes[5];
	out << data.bytes[4];
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// Binasc::writeLittleEndianDouble --
//

std::ostream& Binasc::writeLittleEndianDouble(std::ostream& out, double value) {
	union { char bytes[8]; double d; } data;
	data.d = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	out << data.bytes[4];
	out << data.bytes[5];
	out << data.bytes[6];
	out << data.bytes[7];
	return out;
}


//////////////////////////////
//
// MidiMessage::MidiMessage -- Constructor.
//

MidiMessage::MidiMessage(void) : vector<uchar>() {
	// do nothing
}


MidiMessage::MidiMessage(int command) : vector<uchar>(1, (uchar)command) {
	// do nothing
}


MidiMessage::MidiMessage(int command, int p1) : vector<uchar>(2) {
	(*this)[0] = (uchar)command;
	(*this)[1] = (uchar)p1;
}


MidiMessage::MidiMessage(int command, int p1, int p2) : vector<uchar>(3) {
	(*this)[0] = (uchar)command;
	(*this)[1] = (uchar)p1;
	(*this)[2] = (uchar)p2;
}


MidiMessage::MidiMessage(const MidiMessage& message) : vector<uchar>() {
	(*this) = message;
}


MidiMessage::MidiMessage(const std::vector<uchar>& message) : vector<uchar>() {
	setMessage(message);
}


MidiMessage::MidiMessage(const std::vector<char>& message) : vector<uchar>() {
	setMessage(message);
}


MidiMessage::MidiMessage(const std::vector<int>& message) : vector<uchar>() {
	setMessage(message);
}



//////////////////////////////
//
// MidiMessage::~MidiMessage -- Deconstructor.
//

MidiMessage::~MidiMessage() {
	resize(0);
}



//////////////////////////////
//
// MidiMessage::operator= --
//

MidiMessage& MidiMessage::operator=(const MidiMessage& message) {
	if (this == &message) {
		return *this;
	}
	(*this) = message;
	return *this;
}


MidiMessage& MidiMessage::operator=(const std::vector<uchar>& bytes) {
	if (this == &bytes) {
		return *this;
	}
	setMessage(bytes);
	return *this;
}


MidiMessage& MidiMessage::operator=(const std::vector<char>& bytes) {
	setMessage(bytes);
	return *this;
}


MidiMessage& MidiMessage::operator=(const std::vector<int>& bytes) {
	setMessage(bytes);
	return *this;
}



//////////////////////////////
//
// MidiMessage::setSize -- Change the size of the message byte list.
//     If the size is increased, then the new bytes are not initialized
//     to any specific values.
//

void MidiMessage::setSize(int asize) {
	this->resize(asize);
}



//////////////////////////////
//
// MidiMessage::getSize -- Return the size of the MIDI message bytes.
//

int MidiMessage::getSize(void) const {
	return (int)this->size();
}



//////////////////////////////
//
// MidiMessage::setSizeToCommand -- Set the number of parameters if the
//   command byte is set in the range from 0x80 to 0xef.  Any newly
//   added parameter bytes will be set to 0. Commands in the range
//   of 0xF) should not use this function, and they will ignore
//   modification by this command.
//

int MidiMessage::setSizeToCommand(void) {
	int osize = (int)this->size();
	if (osize < 1) {
		return 0;
	}
	int command = getCommandNibble();
	if (command < 0) {
		return 0;
	}
	int bytecount = 1;
	switch (command) {
		case 0x80: bytecount = 2; break;  // Note Off
		case 0x90: bytecount = 2; break;  // Note On
		case 0xA0: bytecount = 2; break;  // Aftertouch
		case 0xB0: bytecount = 2; break;  // Continuous Controller
		case 0xC0: bytecount = 1; break;  // Patch Change
		case 0xD0: bytecount = 1; break;  // Channel Pressure
		case 0xE0: bytecount = 2; break;  // Pitch Bend
		case 0xF0:
		default:
			return (int)size();
	}
	if (bytecount + 1 < osize) {
		resize(bytecount+1);
		for (int i=osize; i<bytecount+1; i++) {
			(*this)[i] = 0;
		}
	}

	return (int)size();
}


int MidiMessage::resizeToCommand(void) {
	return setSizeToCommand();
}



//////////////////////////////
//
// MidiMessage::getTempoMicro -- Returns the number of microseconds per
//      quarter note if the MidiMessage is a tempo meta message.
//      Returns -1 if the MIDI message is not a tempo meta message.
//

int MidiMessage::getTempoMicro(void) const {
	if (!isTempo()) {
		return -1;
	} else {
		return ((*this)[3] << 16) + ((*this)[4] << 8) + (*this)[5];
	}
}


int MidiMessage::getTempoMicroseconds(void) const {
	return getTempoMicro();
}



//////////////////////////////
//
// MidiMessage::getTempoSeconds -- Returns the number of seconds per
//      quarter note.  Returns -1.0 if the MIDI message is not a
//      tempo meta message.
//

double MidiMessage::getTempoSeconds(void) const {
	int microseconds = getTempoMicroseconds();
	if (microseconds < 0) {
		return -1.0;
	} else {
		return (double)microseconds / 1000000.0;
	}
}



//////////////////////////////
//
// MidiMessage::getTempoBPM -- Returns the tempo in terms of beats per minute.
//   Returns -1 if the MidiMessage is note a tempo meta message.
//

double MidiMessage::getTempoBPM(void) const {
	int microseconds = getTempoMicroseconds();
	if (microseconds < 0) {
		return -1.0;
	}
	return 60000000.0 / (double)microseconds;
}



//////////////////////////////
//
// MidiMessage::getTempoTPS -- Returns the tempo in terms of ticks per seconds.
//

double MidiMessage::getTempoTPS(int tpq) const {
	int microseconds = getTempoMicroseconds();
	if (microseconds < 0) {
		return -1.0;
	} else {
		return tpq * 1000000.0 / (double)microseconds;
	}
}



//////////////////////////////
//
// MidiMessage::getTempoSPT -- Returns the tempo in terms of seconds per tick.
//

double MidiMessage::getTempoSPT(int tpq) const {
	int microseconds = getTempoMicroseconds();
	if (microseconds < 0) {
		return -1.0;
	} else {
		return (double)microseconds / 1000000.0 / tpq;
	}
}



//////////////////////////////
//
// MidiMessage::isMeta -- Returns true if message is a Meta message
//      (when the command byte is 0xff).
//

bool MidiMessage::isMeta(void) const {
	if (size() == 0) {
		return false;
	} else if ((*this)[0] != 0xff) {
		return false;
	} else if (size() < 3) {
		// meta message is ill-formed.
		// meta messages must have at least three bytes:
		//    0: 0xff == meta message marker
		//    1: meta message type
		//    2: meta message data bytes to follow
		return false;
	} else {
		return true;
	}
}


bool MidiMessage::isMetaMessage(void) const {
	return isMeta();
}



//////////////////////////////
//
// MidiMessage::isNoteOff -- Returns true if the command nibble is 0x80
//     or if the command nibble is 0x90 with p2=0 velocity.
//

bool MidiMessage::isNoteOff(void) const {
	if (size() != 3) {
		return false;
	} else if (((*this)[0] & 0xf0) == 0x80) {
		return true;
	} else if ((((*this)[0] & 0xf0) == 0x90) && ((*this)[2] == 0)) {
		return true;
	} else {
		return false;
	}
}



//////////////////////////////
//
// MidiMessage::isNoteOn -- Returns true if the command byte is in the 0x90
//    range and the velocity is non-zero
//

bool MidiMessage::isNoteOn(void) const {
	if (size() != 3) {
		return false;
	} else if (((*this)[0] & 0xf0) != 0x90) {
		return false;
	} else if ((*this)[2] == 0) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isNote -- Returns true if either a note-on or a note-off
//     message.
//

bool MidiMessage::isNote(void) const {
	return isNoteOn() || isNoteOff();
}



//////////////////////////////
//
// MidiMessage::isAftertouch -- Returns true if the command byte is in the 0xA0
//    range.
//

bool MidiMessage::isAftertouch(void) const {
	if (size() != 3) {
		return false;
	} else if (((*this)[0] & 0xf0) != 0xA0) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isController -- Returns true if the command byte is in the 0xB0
//    range and there are two additional data bytes.
//

bool MidiMessage::isController(void) const {
	if (size() != 3) {
		return false;
	} else if (((*this)[0] & 0xf0) != 0xB0) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isTimbre -- Returns true of a patch change message
//    (command nibble 0xc0).
//

bool MidiMessage::isTimbre(void) const {
	if (((*this)[0] & 0xf0) != 0xc0) {
		return false;
	} else if (size() != 2) {
		return false;
	} else {
		return true;
	}
}


bool MidiMessage::isPatchChange(void) const {
	return isTimbre();
}



//////////////////////////////
//
// MidiMessage::isPressure -- Returns true of a channel pressure message
//    (command nibble 0xd0).
//

bool MidiMessage::isPressure(void) const {
	if (((*this)[0] & 0xf0) != 0xd0) {
		return false;
	} else if (size() != 2) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isPitchbend -- Returns true of a pitch-bend message
//    (command nibble 0xe0).
//

bool MidiMessage::isPitchbend(void) const {
	if (((*this)[0] & 0xf0) != 0xe0) {
		return false;
	} else if (size() != 3) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isEmpty -- Returns true if size of data array is zero.
//

bool MidiMessage::isEmpty(void) const {
	return empty();
}



///////////////////////////////
//
// MidiMessage::getMetaType -- returns the meta-message type for the
//     MidiMessage.  If the message is not a meta message, then returns
//     -1.
//

int MidiMessage::getMetaType(void) const {
	if (!isMetaMessage()) {
		return -1;
	} else {
		return (int)(*this)[1];
	}
}



//////////////////////////////
//
// MidiMessage::isText -- Returns true if message is a meta
//      message describing some text (meta message type 0x01).
//

bool MidiMessage::isText(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x01) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isCopyright -- Returns true if message is a meta
//      message describing a copyright notice (meta message type 0x02).
//      Copyright messages should be at absolute tick position 0
//      (and be the first event in the track chunk as well), but this
//      function does not check for those requirements.
//

bool MidiMessage::isCopyright(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x02) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isTrackName -- Returns true if message is a meta
//      message describing a track name (meta message type 0x03).
//

bool MidiMessage::isTrackName(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x03) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isInstrumentName -- Returns true if message is a
//      meta message describing an instrument name (for the track)
//      (meta message type 0x04).
//

bool MidiMessage::isInstrumentName(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x04) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isLyricText -- Returns true if message is a meta message
//      describing some lyric text (for karakoke MIDI files)
//      (meta message type 0x05).
//

bool MidiMessage::isLyricText(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x05) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isMarkerText -- Returns true if message is a meta message
//      describing a marker text (meta message type 0x06).
//

bool MidiMessage::isMarkerText(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x06) {
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isTempo -- Returns true if message is a meta message
//      describing tempo (meta message type 0x51).
//

bool MidiMessage::isTempo(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x51) {
		return false;
	} else if (size() != 6) {
		// Meta tempo message can only be 6 bytes long.
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isTimeSignature -- Returns true if message is
//      a meta message describing a time signature (meta message
//      type 0x58).
//

bool MidiMessage::isTimeSignature(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x58) {
		return false;
	} else if (size() != 7) {
		// Meta time signature message can only be 7 bytes long:
		// FF 58 <size> <top> <bot-log-2> <clocks-per-beat> <32nds>
		return false;
	} else {
		return true;
	}
}



//////////////////////////////
//
// MidiMessage::isKeySignature -- Returns true if message is
//      a meta message describing a key signature (meta message
//      type 0x59).
//

bool MidiMessage::isKeySignature(void) const {
	if (!isMetaMessage()) {
		return false;
	} else if ((*this)[1] != 0x59) {
		return false;
	} else if (size() != 5) {
		// Meta key signature message can only be 5 bytes long:
		// FF 59 <size> <accid> <mode>
		return false;
	} else {
		return true;
	}
}


//////////////////////////////
//
// MidiMessage::isEndOfTrack -- Returns true if message is a meta message
//      for end-of-track (meta message type 0x2f).
//

bool MidiMessage::isEndOfTrack(void) const {
	return getMetaType() == 0x2f ? 1 : 0;
}



//////////////////////////////
//
// MidiMessage::getP0 -- Return index 1 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP0(void) const {
	return size() < 1 ? -1 : (*this)[0];
}



//////////////////////////////
//
// MidiMessage::getP1 -- Return index 1 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP1(void) const {
	return size() < 2 ? -1 : (*this)[1];
}



//////////////////////////////
//
// MidiMessage::getP2 -- Return index 2 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP2(void) const {
	return size() < 3 ? -1 : (*this)[2];
}



//////////////////////////////
//
// MidiMessage::getP3 -- Return index 3 byte, or -1 if it doesn't exist.
//

int MidiMessage::getP3(void) const {
	return size() < 4 ? -1 : (*this)[3];
}



//////////////////////////////
//
// MidiMessage::getKeyNumber -- Return the key number (such as 60 for
//    middle C).  If the message does not have a note parameter, then
//    return -1;  if the key is invalid (above 127 in value), then
//    limit to the range 0 to 127.
//

int MidiMessage::getKeyNumber(void) const {
	if (isNote() || isAftertouch()) {
		int output = getP1();
		if (output < 0) {
			return output;
		} else {
			return 0xff & output;
		}
	} else {
		return -1;
	}
}



//////////////////////////////
//
// MidiMessage::getVelocity -- Return the key veolocity.  If the message
//   is not a note-on or a note-off, then return -1.  If the value is
//   out of the range 0-127, then chop off the high-bits.
//

int MidiMessage::getVelocity(void) const {
	if (isNote()) {
		int output = getP2();
		if (output < 0) {
			return output;
		} else {
			return 0xff & output;
		}
	} else {
		return -1;
	}
}



//////////////////////////////
//
// MidiMessage::getControllerNumber -- Return the controller number (such as 1
//   for modulation wheel).  If the message does not have a controller number
//   parameter, then return -1.  If the controller number is invalid (above 127
//   in value), then limit the range to to 0-127.
//

int MidiMessage::getControllerNumber(void) const {
  if (isController()) {
		int output = getP1();
		if (output < 0) {
			// -1 means no P1, although isController() is false in such a case.
			return output;
		} else {
			return 0x7f & output;
		}
	} else {
		return -1;
	}
}



//////////////////////////////
//
// MidiMessage::getControllerValue -- Return the controller value.  If the
//   message is not a control change message, then return -1.  If the value is
//   out of the range 0-127, then chop off the high-bits.
//

int MidiMessage::getControllerValue(void) const {
	if (isController()) {
		int output = getP2();
		if (output < 0) {
			// -1 means no P2, although isController() is false in such a case.
			return output;
		} else {
			return 0x7f & output;
		}
	} else {
		return -1;
	}
}



//////////////////////////////
//
// MidiMessage::setP0 -- Set the command byte.
//   If the MidiMessage is too short, add extra spaces to
//   allow for P0.  The value should be in the range from
//   128 to 255, but this function will not babysit you.
//

void MidiMessage::setP0(int value) {
	if (getSize() < 1) {
		resize(1);
	}
	(*this)[0] = value;
}



//////////////////////////////
//
// MidiMessage::setP1 -- Set the first parameter value.
//   If the MidiMessage is too short, add extra spaces to
//   allow for P1.  The command byte will be undefined if
//   it was added.  The value should be in the range from
//   0 to 127, but this function will not babysit you.
//

void MidiMessage::setP1(int value) {
	if (getSize() < 2) {
		resize(2);
	}
	(*this)[1] = value;
}



//////////////////////////////
//
// MidiMessage::setP2 -- Set the second paramter value.
//     If the MidiMessage is too short, add extra spaces
//     to allow for P2.  The command byte and/or the P1 value
//     will be undefined if extra space needs to be added and
//     those slots are created.  The value should be in the range
//     from 0 to 127, but this function will not babysit you.
//

void MidiMessage::setP2(int value) {
	if (getSize() < 3) {
		resize(3);
	}
	(*this)[2] = value;
}



//////////////////////////////
//
// MidiMessage::setP3 -- Set the third paramter value.
//     If the MidiMessage is too short, add extra spaces
//     to allow for P3.  The command byte and/or the P1/P2 values
//     will be undefined if extra space needs to be added and
//     those slots are created.  The value should be in the range
//     from 0 to 127, but this function will not babysit you.
//

void MidiMessage::setP3(int value) {
	if (getSize() < 4) {
		resize(4);
	}
	(*this)[3] = value;
}



//////////////////////////////
//
// MidiMessage::setKeyNumber -- Set the note on/off key number (or
//    aftertouch key).  Ignore if not note or aftertouch message.
//    Limits the input value to the range from 0 to 127.
//

void MidiMessage::setKeyNumber(int value) {
	if (isNote() || isAftertouch()) {
		setP1(value & 0xff);
	} else {
		// don't do anything since this is not a note-related message.
	}
}



//////////////////////////////
//
// MidiMessage::setVelocity -- Set the note on/off velocity; ignore
//   if note a note message.  Limits the input value to the range
//   from 0 to 127.
//

void MidiMessage::setVelocity(int value) {
	if (isNote()) {
		setP2(value & 0xff);
	} else {
		// don't do anything since this is not a note-related message.
	}
}



//////////////////////////////
//
// MidiMessage::getCommandNibble -- Returns the top 4 bits of the (*this)[0]
//    entry, or -1 if there is not (*this)[0].
//

int MidiMessage::getCommandNibble(void) const {
	if (size() < 1) {
		return -1;
	} else {
		return (*this)[0] & 0xf0;
	}
}



//////////////////////////////
//
// MidiMessage::getCommandByte -- Return the command byte or -1 if not
//    allocated.
//

int MidiMessage::getCommandByte(void) const {
	if (size() < 1) {
		return -1;
	} else {
		return (*this)[0];
	}
}



//////////////////////////////
//
// MidiMessage::getChannelNibble -- Returns the bottom 4 bites of the
//      (*this)[0] entry, or -1 if there is not (*this)[0].  Should be refined
//      to return -1 if the top nibble is 0xf0, since those commands are
//      not channel specific.
//

int MidiMessage::getChannelNibble(void) const {
	if (size() < 1) {
		return -1;
	} else {
		return (*this)[0] & 0x0f;
	}
}


int MidiMessage::getChannel(void) const {
	return getChannelNibble();
}



//////////////////////////////
//
// MidiMessage::setCommandByte --
//

void MidiMessage::setCommandByte(int value) {
	if (size() < 1) {
		resize(1);
	} else {
		(*this)[0] = (uchar)(value & 0xff);
	}
}

void MidiMessage::setCommand(int value) {
	setCommandByte(value);
}



//////////////////////////////
//
// MidiMessage::setCommand -- Set the command byte and parameter bytes
//   for a MidiMessage.  The size of the message will be adjusted to
//   the number of input parameters.
//

void MidiMessage::setCommand(int value, int p1) {
	this->resize(2);
	(*this)[0] = (uchar)value;
	(*this)[1] = (uchar)p1;
}


void MidiMessage::setCommand(int value, int p1, int p2) {
	this->resize(3);
	(*this)[0] = (uchar)value;
	(*this)[1] = (uchar)p1;
	(*this)[2] = (uchar)p2;
}



//////////////////////////////
//
// MidiMessage::setCommandNibble --
//

void MidiMessage::setCommandNibble(int value) {
	if (this->size() < 1) {
		this->resize(1);
	}
	if (value <= 0x0f) {
		(*this)[0] = ((*this)[0] & 0x0f) | ((uchar)((value << 4) & 0xf0));
	} else {
		(*this)[0] = ((*this)[0] & 0x0f) | ((uchar)(value & 0xf0));
	}
}




//////////////////////////////
//
// MidiMessage::setChannelNibble --
//

void MidiMessage::setChannelNibble(int value) {
	if (this->size() < 1) {
		this->resize(1);
	}
	(*this)[0] = ((*this)[0] & 0xf0) | ((uchar)(value & 0x0f));
}


void MidiMessage::setChannel(int value) {
	setChannelNibble(value);
}



//////////////////////////////
//
// MidiMessage::setParameters -- Set the second and optionally the
//     third MIDI byte of a MIDI message.  The command byte will not
//     be altered, and will be set to 0 if it currently does not exist.
//

void MidiMessage::setParameters(int p1) {
	int oldsize = (int)size();
	resize(2);
	(*this)[1] = (uchar)p1;
	if (oldsize < 1) {
		(*this)[0] = 0;
	}
}


void MidiMessage::setParameters(int p1, int p2) {
	int oldsize = (int)size();
	resize(3);
	(*this)[1] = (uchar)p1;
	(*this)[2] = (uchar)p2;
	if (oldsize < 1) {
		(*this)[0] = 0;
	}
}


//////////////////////////////
//
// MidiMessage::setMessage --  Set the contents of MIDI bytes to the
//   input list of bytes.
//

void MidiMessage::setMessage(const std::vector<uchar>& message) {
	this->resize(message.size());
	for (int i=0; i<(int)this->size(); i++) {
		(*this)[i] = message[i];
	}
}


void MidiMessage::setMessage(const std::vector<char>& message) {
	resize(message.size());
	for (int i=0; i<(int)size(); i++) {
		(*this)[i] = (uchar)message[i];
	}
}


void MidiMessage::setMessage(const std::vector<int>& message) {
	resize(message.size());
	for (int i=0; i<(int)size(); i++) {
		(*this)[i] = (uchar)message[i];
	}
}



//////////////////////////////
//
// MidiMessage::setSpelling -- Encode a MidiPlus accidental state for a note.
//    For example, if a note's key number is 60, the enharmonic pitch name
//    could be any of these possibilities:
//        C, B-sharp, D-double-flat
//    MIDI note 60 is ambiguous as to which of these names are intended,
//    so MIDIPlus allows these mappings to be preserved for later recovery.
//    See Chapter 5 (pp. 99-104) of Beyond MIDI (1997).
//
//    The first parameter is the diatonic pitch number (or pitch class
//    if the octave is set to 0):
//       octave * 7 + 0 = C pitches
//       octave * 7 + 1 = D pitches
//       octave * 7 + 2 = E pitches
//       octave * 7 + 3 = F pitches
//       octave * 7 + 4 = G pitches
//       octave * 7 + 5 = A pitches
//       octave * 7 + 6 = B pitches
//
//    The second parameter is the semitone alteration (accidental).
//    0 = natural state, 1 = sharp, 2 = double sharp, -1 = flat,
//    -2 = double flat.
//
//    Only note-on messages can be processed (other messages will be
//    silently ignored).
//

void MidiMessage::setSpelling(int base7, int accidental) {
	if (!isNoteOn()) {
		return;
	}
	// The bottom two bits of the attack velocity are used for the
	// spelling, so need to make sure the velocity will not accidentally
	// be set to zero (and make the note-on a note-off).
	if (getVelocity() < 4) {
		setVelocity(4);
	}
	int dpc = base7 % 7;
	uchar spelling = 0;

	// Table 5.1, page 101 in Beyond MIDI (1997)
	// http://beyondmidi.ccarh.org/beyondmidi-600dpi.pdf
	switch (dpc) {

		case 0:
			switch (accidental) {
				case -2: spelling = 1; break; // Cbb
				case -1: spelling = 1; break; // Cb
				case  0: spelling = 2; break; // C
				case +1: spelling = 2; break; // C#
				case +2: spelling = 3; break; // C##
			}
			break;

		case 1:
			switch (accidental) {
				case -2: spelling = 1; break; // Dbb
				case -1: spelling = 1; break; // Db
				case  0: spelling = 2; break; // D
				case +1: spelling = 3; break; // D#
				case +2: spelling = 3; break; // D##
			}
			break;

		case 2:
			switch (accidental) {
				case -2: spelling = 1; break; // Ebb
				case -1: spelling = 2; break; // Eb
				case  0: spelling = 2; break; // E
				case +1: spelling = 3; break; // E#
				case +2: spelling = 3; break; // E##
			}
			break;

		case 3:
			switch (accidental) {
				case -2: spelling = 1; break; // Fbb
				case -1: spelling = 1; break; // Fb
				case  0: spelling = 2; break; // F
				case +1: spelling = 2; break; // F#
				case +2: spelling = 3; break; // F##
				case +3: spelling = 3; break; // F###
			}
			break;

		case 4:
			switch (accidental) {
				case -2: spelling = 1; break; // Gbb
				case -1: spelling = 1; break; // Gb
				case  0: spelling = 2; break; // G
				case +1: spelling = 2; break; // G#
				case +2: spelling = 3; break; // G##
			}
			break;

		case 5:
			switch (accidental) {
				case -2: spelling = 1; break; // Abb
				case -1: spelling = 1; break; // Ab
				case  0: spelling = 2; break; // A
				case +1: spelling = 3; break; // A#
				case +2: spelling = 3; break; // A##
			}
			break;

		case 6:
			switch (accidental) {
				case -2: spelling = 1; break; // Bbb
				case -1: spelling = 2; break; // Bb
				case  0: spelling = 2; break; // B
				case +1: spelling = 3; break; // B#
				case +2: spelling = 3; break; // B##
			}
			break;

	}

	uchar vel = getVelocity();
	// suppress any previous content in the first two bits:
	vel = vel & 0xFC;
	// insert the spelling code:
	vel = vel | spelling;
	setVelocity(vel);
}



//////////////////////////////
//
// MidiMessage::getSpelling -- Return the diatonic pitch class and accidental
//    for a note-on's key number.  The MIDI file must be encoded with MIDIPlus
//    pitch spelling codes for this function to return valid data; otherwise,
//    it will return a neutral fixed spelling for each MIDI key.
//
//    The first parameter will be filled in with the base-7 diatonic pitch:
//        pc + octave * 7
//     where pc is the numbers 0 through 6 representing the pitch classes
//     C through B, the octave is MIDI octave (not the scientific pitch
//     octave which is one less than the MIDI ocatave, such as C4 = middle C).
//     The second number is the accidental for the base-7 pitch.
//

void MidiMessage::getSpelling(int& base7, int& accidental) {
	if (!isNoteOn()) {
		return;
	}
	base7 = -123456;
	accidental = 123456;
	int base12   = getKeyNumber();
	int octave   = base12 / 12;
	int base12pc = base12 - octave * 12;
	int base7pc  = 0;
	int spelling = 0x03 & getVelocity();

	// Table 5.1, page 101 in Beyond MIDI (1997)
	// http://beyondmidi.ccarh.org/beyondmidi-600dpi.pdf
	switch (base12pc) {

		case 0:
			switch (spelling) {
				        case 1: base7pc = 1; accidental = -2; break;  // Dbb
				case 0: case 2: base7pc = 0; accidental =  0; break;  // C
				        case 3: base7pc = 6; accidental = +1; octave--; break;  // B#
			}
			break;

		case 1:
			switch (spelling) {
				        case 1: base7pc = 1; accidental = -1; break;  // Db
				case 0: case 2: base7pc = 0; accidental = +1; break;  // C#
				        case 3: base7pc = 6; accidental = +2; octave--; break;  // B##
			}
			break;

		case 2:
			switch (spelling) {
				        case 1: base7pc = 2; accidental = -2; break;  // Ebb
				case 0: case 2: base7pc = 1; accidental =  0; break;  // D
				        case 3: base7pc = 0; accidental = +2; break;  // C##
			}
			break;

		case 3:
			switch (spelling) {
				        case 1: base7pc = 3; accidental = -2; break;  // Fbb
				case 0: case 2: base7pc = 2; accidental = -1; break;  // Eb
				        case 3: base7pc = 1; accidental = +1; break;  // D#
			}
			break;

		case 4:
			switch (spelling) {
				        case 1: base7pc = 3; accidental = -1; break;  // Fb
				case 0: case 2: base7pc = 2; accidental =  0; break;  // E
				        case 3: base7pc = 1; accidental = +2; break;  // D##
			}
			break;

		case 5:
			switch (spelling) {
				        case 1: base7pc = 4; accidental = -2; break;  // Gbb
				case 0: case 2: base7pc = 3; accidental =  0; break;  // F
				        case 3: base7pc = 2; accidental = +1; break;  // E#
			}
			break;

		case 6:
			switch (spelling) {
				        case 1: base7pc = 4; accidental = -1; break;  // Gb
				case 0: case 2: base7pc = 3; accidental = +1; break;  // F#
				        case 3: base7pc = 2; accidental = +2; break;  // E##
			}
			break;

		case 7:
			switch (spelling) {
				        case 1: base7pc = 5; accidental = -2; break;  // Abb
				case 0: case 2: base7pc = 4; accidental =  0; break;  // G
				        case 3: base7pc = 3; accidental = +2; break;  // F##
			}
			break;

		case 8:
			switch (spelling) {
				        case 1: base7pc = 5; accidental = -1; break;  // Ab
				case 0: case 2: base7pc = 4; accidental = +1; break;  // G#
				        case 3: base7pc = 3; accidental = +3; break;  // F###
			}
			break;

		case 9:
			switch (spelling) {
				        case 1: base7pc = 6; accidental = -2; break;  // Bbb
				case 0: case 2: base7pc = 5; accidental =  0; break;  // A
				        case 3: base7pc = 4; accidental = +2; break;  // G##
			}
			break;

		case 10:
			switch (spelling) {
				        case 1: base7pc = 0; accidental = -2; octave++; break;  // Cbb
				case 0: case 2: base7pc = 6; accidental = -1; break;  // Bb
				        case 3: base7pc = 5; accidental = +1; break;  // A#
			}
			break;

		case 11:
			switch (spelling) {
				        case 1: base7pc = 0; accidental = -1; octave++; break;  // Cb
				case 0: case 2: base7pc = 6; accidental =  0; break;  // B
				        case 3: base7pc = 5; accidental = +2; break;  // A##
			}
			break;

	}

	base7 = base7pc + 7 * octave;
}



//////////////////////////////
//
// MidiMessage::getMetaContent -- Returns the bytes of the meta
//   message after the length (which is a variable-length-value).
//

std::string MidiMessage::getMetaContent(void) {
	std::string output;
	if (!isMetaMessage()) {
		return output;
	}
	int start = 3;
	if (operator[](2) > 0x7f) {
		start++;
		if (operator[](3) > 0x7f) {
			start++;
			if (operator[](4) > 0x7f) {
				start++;
				if (operator[](5) > 0x7f) {
					start++;
					// maximum of 5 bytes in VLV, so last must be < 0x80
				}
			}
		}
	}
	output.reserve(this->size());
	for (int i=start; i<(int)this->size(); i++) {
		output.push_back(operator[](i));
	}
	return output;
}



//////////////////////////////
//
// MidiMessage::setMetaContent - Set the content of a meta-message.  This
//    function handles the size of the message starting at byte 3 in the
//    message, and it does not alter the meta message type.  The message
//    must be a meta-message before calling this function and be assigne
//    a meta-message type.
//

void MidiMessage::setMetaContent(const std::string& content) {
	if (this->size() < 2) {
		// invalid message, so ignore request
		return;
	}
	if (operator[](0) != 0xFF) {
		// not a meta message, so ignore request
		return;
	}
	this->resize(2);
	
	// add the size of the meta message data (VLV)
	int dsize = (int)content.size();
	if (dsize < 128) {
		push_back((uchar)dsize);
	} else {
		// calculate VLV bytes and insert into message
		uchar byte1 = dsize & 0x7f;
		uchar byte2 = (dsize >>  7) & 0x7f;
		uchar byte3 = (dsize >> 14) & 0x7f;
		uchar byte4 = (dsize >> 21) & 0x7f;
		uchar byte5 = (dsize >> 28) & 0x7f;
		if (byte5) {
			byte4 |= 0x80;
		}
		if (byte4) {
			byte4 |= 0x80;
			byte3 |= 0x80;
		}
		if (byte3) {
			byte3 |= 0x80;
			byte2 |= 0x80;
		}
		if (byte2) {
			byte2 |= 0x80;
		}
		if (byte5) { push_back(byte5); }
		if (byte4) { push_back(byte4); }
		if (byte3) { push_back(byte3); }
		if (byte2) { push_back(byte2); }
		push_back(byte1);
	}
	std::copy(content.begin(), content.end(), std::back_inserter(*this));
}



//////////////////////////////
//
// MidiMessage::setMetaTempo -- Input tempo is in quarter notes per minute
//   (meta message #0x51).
//

void MidiMessage::setMetaTempo(double tempo) {
	int microseconds = (int)(60.0 / tempo * 1000000.0 + 0.5);
	setTempoMicroseconds(microseconds);
}



//////////////////////////////
//
// MidiMessage::setTempo -- Alias for MidiMessage::setMetaTempo().
//

void MidiMessage::setTempo(double tempo) {
	setMetaTempo(tempo);
}



//////////////////////////////
//
// MidiMessage::setTempoMicroseconds -- Set the tempo in terms
//   of microseconds per quarter note.
//

void MidiMessage::setTempoMicroseconds(int microseconds) {
	resize(6);
	(*this)[0] = 0xff;
	(*this)[1] = 0x51;
	(*this)[2] = 3;
	(*this)[3] = (microseconds >> 16) & 0xff;
	(*this)[4] = (microseconds >>  8) & 0xff;
	(*this)[5] = (microseconds >>  0) & 0xff;
}



//////////////////////////////
//
// MidiMessage::makeTimeSignature -- create a time signature meta message
//      (meta #0x58).  The "bottom" parameter should be a power of two;
//      otherwise, it will be forced to be the next highest power of two,
//      as MIDI time signatures must have a power of two in the denominator.
//
// Default values:
//     clocksPerClick     == 24 (quarter note)
//     num32ndsPerQuarter ==  8 (8 32nds per quarter note)
//
// Time signature of 4/4 would be:
//    top    = 4
//    bottom = 4 (converted to 2 in the MIDI file for 2nd power of 2).
//    clocksPerClick = 24 (2 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//
// Time signature of 6/8 would be:
//    top    = 6
//    bottom = 8 (converted to 3 in the MIDI file for 3rd power of 2).
//    clocksPerClick = 36 (3 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//

void MidiMessage::makeTimeSignature(int top, int bottom, int clocksPerClick,
		int num32ndsPerQuarter) {
	int base2 = 0;
	while (bottom >>= 1) base2++;
	resize(7);
	(*this)[0] = 0xff;
	(*this)[1] = 0x58;
	(*this)[2] = 4;
	(*this)[3] = 0xff & top;
	(*this)[4] = 0xff & base2;
	(*this)[5] = 0xff & clocksPerClick;
	(*this)[6] = 0xff & num32ndsPerQuarter;
}



///////////////////////////////////////////////////////////////////////////
//
// make functions to create various MIDI message --
//


//////////////////////////////
//
// MidiMessage::makeNoteOn -- create a note-on message.
//
// default value: channel = 0
//
// Note: The channel parameter used to be last, but makes more sense to
//   have it first...
//

void MidiMessage::makeNoteOn(int channel, int key, int velocity) {
	resize(3);
	(*this)[0] = 0x90 | (0x0f & channel);
	(*this)[1] = key & 0x7f;
	(*this)[2] = velocity & 0x7f;
}



//////////////////////////////
//
// MidiMessage::makeNoteOff -- create a note-off message.   If no
//   parameters are given, the current contents is presumed to be a
//   note-on message, which will be converted into a note-off message.
//
// default value: channel = 0
//
// Note: The channel parameter used to be last, but makes more sense to
//   have it first...
//


void MidiMessage::makeNoteOff(int channel, int key, int velocity) {
	resize(3);
	(*this)[0] = 0x80 | (0x0f & channel);
	(*this)[1] = key & 0x7f;
	(*this)[2] = velocity & 0x7f;
}


void MidiMessage::makeNoteOff(int channel, int key) {
	resize(3);
	(*this)[0] = 0x90 | (0x0f & channel);
	(*this)[1] = key & 0x7f;
	(*this)[2] = 0x00;
}

//
// MidiMessage::makeNoteOff(void) -- create a 0x90 note message with
//      The key and velocity set to 0.
//

void MidiMessage::makeNoteOff(void) {
	if (!isNoteOn()) {
		resize(3);
		(*this)[0] = 0x90;
		(*this)[1] = 0;
		(*this)[2] = 0;
	} else {
		(*this)[2] = 0;
	}
}



/////////////////////////////
//
// MidiMessage::makePatchChange -- Create a patch-change message.
//

void MidiMessage::makePatchChange(int channel, int patchnum) {
	resize(0);
	push_back(0xc0 | (0x0f & channel));
	push_back(0x7f & patchnum);
}

//
// MidiMessage::makeTimbre -- alias for MidiMessage::makePatchChange().
//

void MidiMessage::makeTimbre(int channel, int patchnum) {
	makePatchChange(channel, patchnum);
}


/////////////////////////////
//
// MidiMessage::makeController -- Create a controller message.
//

void MidiMessage::makeController(int channel, int num, int value) {
	resize(0);
	push_back(0xb0 | (0x0f & channel));
	push_back(0x7f & num);
	push_back(0x7f & value);
}



/////////////////////////////
//
// MidiMessage::makeSustain -- Create a sustain pedal message.
//   Value in 0-63 range is a sustain off.  Value in the
//   64-127 value is a sustain on.
//

void MidiMessage::makeSustain(int channel, int value) {
	makeController(channel, 64, value);
}

//
// MidiMessage::makeSustain -- Alias for MidiMessage::makeSustain().
//

void MidiMessage::makeSustainPedal(int channel, int value) {
	makeSustain(channel, value);
}



/////////////////////////////
//
// MidiMessage::makeSustainOn -- Create sustain-on controller message.
//

void MidiMessage::makeSustainOn(int channel) {
	makeController(channel, 64, 127);
}

//
// MidiMessage::makeSustainPedalOn -- Alias for MidiMessage::makeSustainOn().
//

void MidiMessage::makeSustainPedalOn(int channel) {
	makeSustainOn(channel);
}



/////////////////////////////
//
// MidiMessage::makeSustainOff -- Create a sustain-off controller message.
//

void MidiMessage::makeSustainOff(int channel) {
	makeController(channel, 64, 0);
}

//
// MidiMessage::makeSustainPedalOff -- Alias for MidiMessage::makeSustainOff().
//

void MidiMessage::makeSustainPedalOff(int channel) {
	makeSustainOff(channel);
}



//////////////////////////////
//
// MidiMessage::makeMetaMessage -- Create a Meta event with the
//   given text string as the parameter.  The length of the string should
//   is a VLV.  If the length is larger than 127 byte, then the length
//   will contain more than one byte.
//

void MidiMessage::makeMetaMessage(int mnum, const std::string& data) {
	resize(0);
	push_back(0xff);
	push_back(mnum & 0x7f); // max meta-message number is 0x7f.
	setMetaContent(data);
}



//////////////////////////////
//
// MidiMessage::makeText -- Create a metaevent text message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeText(const std::string& text) {
	makeMetaMessage(0x01, text);
}



//////////////////////////////
//
// MidiMessage::makeCopyright -- Create a metaevent copyright message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeCopyright(const std::string& text) {
	makeMetaMessage(0x02, text);
}



//////////////////////////////
//
// MidiMessage::makeTrackName -- Create a metaevent track name message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeTrackName(const std::string& name) {
	makeMetaMessage(0x03, name);
}



//////////////////////////////
//
// MidiMessage::makeTrackName -- Create a metaevent instrument name message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeInstrumentName(const std::string& name) {
	makeMetaMessage(0x04, name);
}



//////////////////////////////
//
// MidiMessage::makeLyric -- Create a metaevent lyrics/text message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeLyric(const std::string& text) {
	makeMetaMessage(0x05, text);
}



//////////////////////////////
//
// MidiMessage::makeMarker -- Create a metaevent marker message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeMarker(const std::string& text) {
	makeMetaMessage(0x06, text);
}



//////////////////////////////
//
// MidiMessage::makeCue -- Create a metaevent cue-point message.
//    This is not a real MIDI message, but rather a pretend message for use
//    within Standard MIDI Files.
//

void MidiMessage::makeCue(const std::string& text) {
	makeMetaMessage(0x07, text);
}


//////////////////////////////
//
// MidiEvent::MidiEvent -- Constructor classes
//

MidiEvent::MidiEvent(void) : MidiMessage() {
	clearVariables();
}


MidiEvent::MidiEvent(int command) : MidiMessage(command)  {
	clearVariables();
}


MidiEvent::MidiEvent(int command, int p1) : MidiMessage(command, p1) {
	clearVariables();
}


MidiEvent::MidiEvent(int command, int p1, int p2)
		: MidiMessage(command, p1, p2) {
	clearVariables();
}


MidiEvent::MidiEvent(int aTime, int aTrack, vector<uchar>& message)
		: MidiMessage(message) {
	track       = aTrack;
	tick        = aTime;
	seconds     = 0.0;
	seq         = 0;
	m_eventlink = NULL;
}


MidiEvent::MidiEvent(const MidiEvent& mfevent) : MidiMessage() {
	track   = mfevent.track;
	tick    = mfevent.tick;
	seconds = mfevent.seconds;
	seq     = mfevent.seq;
	m_eventlink = NULL;

	this->resize(mfevent.size());
	for (int i=0; i<(int)this->size(); i++) {
		(*this)[i] = mfevent[i];
	}
}



//////////////////////////////
//
// MidiEvent::~MidiEvent -- MidiFile Event destructor
//

MidiEvent::~MidiEvent() {
	track   = -1;
	tick    = -1;
	seconds = -1.0;
	seq     = -1;
	this->resize(0);
	m_eventlink = NULL;
}


//////////////////////////////
//
// MidiEvent::clearVariables --  Clear everything except MidiMessage data.
//

void MidiEvent::clearVariables(void) {
	track     = 0;
	tick      = 0;
	seconds   = 0.0;
	seq       = 0;
	m_eventlink = NULL;
}


//////////////////////////////
//
// MidiEvent::operator= -- Copy the contents of another MidiEvent.
//

MidiEvent& MidiEvent::operator=(const MidiEvent& mfevent) {
	if (this == &mfevent) {
		return *this;
	}
	tick    = mfevent.tick;
	track   = mfevent.track;
	seconds = mfevent.seconds;
	seq     = mfevent.seq;
	m_eventlink = NULL;
	this->resize(mfevent.size());
	for (int i=0; i<(int)this->size(); i++) {
		(*this)[i] = mfevent[i];
	}
	return *this;
}


MidiEvent& MidiEvent::operator=(const MidiMessage& message) {
	if (this == &message) {
		return *this;
	}
	clearVariables();
	this->resize(message.size());
	for (int i=0; i<(int)this->size(); i++) {
		(*this)[i] = message[i];
	}
	return *this;
}


MidiEvent& MidiEvent::operator=(const vector<uchar>& bytes) {
	clearVariables();
	this->resize(bytes.size());
	for (int i=0; i<(int)this->size(); i++) {
		(*this)[i] = bytes[i];
	}
	return *this;
}


MidiEvent& MidiEvent::operator=(const vector<char>& bytes) {
	clearVariables();
	setMessage(bytes);
	return *this;
}


MidiEvent& MidiEvent::operator=(const vector<int>& bytes) {
	clearVariables();
	setMessage(bytes);
	return *this;
}



//////////////////////////////
//
// MidiEvent::unlinkEvent -- Disassociate this event with another.
//   Also tell the other event to disassociate from this event.
//

void MidiEvent::unlinkEvent(void) {
	if (m_eventlink == NULL) {
		return;
	}
	MidiEvent* mev = m_eventlink;
	m_eventlink = NULL;
	mev->unlinkEvent();
}



//////////////////////////////
//
// MidiEvent::linkEvent -- Make a link between two messages.
//   Unlinking
//

void MidiEvent::linkEvent(MidiEvent* mev) {
	if (mev->m_eventlink != NULL) {
		// unlink other event if it is linked to something else;
		mev->unlinkEvent();
	}
	// if this is already linked to something else, then unlink:
	if (m_eventlink != NULL) {
		m_eventlink->unlinkEvent();
	}
	unlinkEvent();

	mev->m_eventlink = this;
	m_eventlink = mev;
}


void MidiEvent::linkEvent(MidiEvent& mev) {
	linkEvent(&mev);
}



//////////////////////////////
//
// MidiEvent::getLinkedEvent -- Returns a linked event.  Usually
//   this is the note-off message for a note-on message and vice-versa.
//   Returns null if there are no links.
//

MidiEvent* MidiEvent::getLinkedEvent(void) {
	return m_eventlink;
}


const MidiEvent* MidiEvent::getLinkedEvent(void) const {
	return m_eventlink;
}



//////////////////////////////
//
// MidiEvent::isLinked -- Returns true if there is an event which is not
//   NULL.  This function is similar to getLinkedEvent().
//

int MidiEvent::isLinked(void) const {
	return m_eventlink == NULL ? 0 : 1;
}



//////////////////////////////
//
// MidiEvent::getTickDuration --  For linked events (note-ons and note-offs),
//    return the absolute tick time difference between the two events.
//    The tick values are presumed to be in absolute tick mode rather than
//    delta tick mode.  Returns 0 if not linked.
//

int MidiEvent::getTickDuration(void) const {
	const MidiEvent* mev = getLinkedEvent();
	if (mev == NULL) {
		return 0;
	}
	int tick2 = mev->tick;
	if (tick2 > tick) {
		return tick2 - tick;
	} else {
		return tick - tick2;
	}
}



//////////////////////////////
//
// MidiEvent::getDurationInSeconds -- For linked events (note-ons and
//     note-offs), return the duration of the note in seconds.  The
//     seconds analysis must be done first; otherwise the duration will be
//     reported as zero.
//

double MidiEvent::getDurationInSeconds(void) const {
	const MidiEvent* mev = getLinkedEvent();
	if (mev == NULL) {
		return 0;
	}
	double seconds2 = mev->seconds;
	if (seconds2 > seconds) {
		return seconds2 - seconds;
	} else {
		return seconds - seconds2;
	}
}


//////////////////////////////
//
// MidiEventList::MidiEventList -- Constructor.
//

MidiEventList::MidiEventList(void) {
	reserve(1000);
}



//////////////////////////////
//
// MidiEventList::MidiEventList(MidiEventList&) -- Copy constructor.
//

MidiEventList::MidiEventList(const MidiEventList& other) {
	list.reserve(other.list.size());
	auto it = other.list.begin();
	std::generate_n(std::back_inserter(list), other.list.size(), [&]() -> MidiEvent* {
		return new MidiEvent(**it++);
	});
}



//////////////////////////////
//
// MidiEventList::MidiEventList(MidiEventList&&) -- Move constructor.
//

MidiEventList::MidiEventList(MidiEventList&& other) {
   list = std::move(other.list);
   other.list.clear();
}



//////////////////////////////
//
// MidiEventList::~MidiEventList -- Deconstructor.  Deallocate all stored
//   data.
//

MidiEventList::~MidiEventList() {
	clear();
}



//////////////////////////////
//
// MidiEventList::operator[] --
//

MidiEvent&  MidiEventList::operator[](int index) {
	return *list[index];
}


const MidiEvent&  MidiEventList::operator[](int index) const {
	return *list[index];
}



//////////////////////////////
//
// MidiEventList::back -- Return the last element in the list.
//

MidiEvent& MidiEventList::back(void) {
	return *list.back();
}


const MidiEvent& MidiEventList::back(void) const {
	return *list.back();
}

//
// MidiEventList::last -- Alias for MidiEventList::back().
//

MidiEvent& MidiEventList::last(void) {
	return back();
}


const MidiEvent& MidiEventList::last(void) const {
	return back();
}



//////////////////////////////
//
// MidiEventList::getEvent -- The same thing as operator[], for
//      internal use when operator[] would look more messy.
//

MidiEvent& MidiEventList::getEvent(int index) {
   return *list[index];
}


const MidiEvent& MidiEventList::getEvent(int index) const {
   return *list[index];
}



//////////////////////////////
//
// MidiEventList::clear -- De-allocate any MidiEvents present in the list
//    and set the size of the list to 0.
//

void MidiEventList::clear(void) {
	for (int i=0; i<(int)list.size(); i++) {
		if (list[i] != NULL) {
			delete list[i];
			list[i] = NULL;
		}
	}
	list.resize(0);
}



//////////////////////////////
//
// MidiEventList::data -- Return the low-level array of MidiMessage
//     pointers.  This is useful for applying your own sorting
//     function to the list.
//

MidiEvent** MidiEventList::data(void) {
	return list.data();
}



//////////////////////////////
//
// MidiEventList::reserve --  Pre-allocate space in the list for storing
//     elements.
//

void MidiEventList::reserve(int rsize) {
	if (rsize > (int)list.size()) {
		list.reserve(rsize);
	}
}


//////////////////////////////
//
// MidiEventList::getSize -- Return the number of MidiEvents stored
//     in the list.
//

int MidiEventList::getSize(void) const {
	return (int)list.size();
}

//
// MidiEventList::size -- Alias for MidiEventList::getSize().
//

int MidiEventList::size(void) const {
	return getSize();
}

//
// MidiEventList::getEventCount -- Alias for MidiEventList::getSize().
//

int MidiEventList::getEventCount(void) const {
	return getSize();
}



//////////////////////////////
//
// MidiEventList::append -- add a MidiEvent at the end of the list.  Returns
//     the index of the appended event.
//

int MidiEventList::append(MidiEvent& event) {
	MidiEvent* ptr = new MidiEvent(event);
	list.push_back(ptr);
	return (int)list.size()-1;
}

//
// MidiEventList::push -- Alias for MidiEventList::append().
//

int MidiEventList::push(MidiEvent& event) {
	return append(event);
}

//
// MidiEventList::push_back -- Alias for MidiEventList::append().
//

int MidiEventList::push_back(MidiEvent& event) {
	return append(event);
}



//////////////////////////////
//
// MidiEventList::removeEmpties -- Remove any MIDI message which contain no
//    bytes.  This function first deallocates any empty MIDI events, and then
//    removes them from the list of events.
//

void MidiEventList::removeEmpties(void) {
	int count = 0;
	for (int i=0; i<(int)list.size(); i++) {
		if (list[i]->empty()) {
			delete list[i];
			list[i] = NULL;
			count++;
		}
	}
	if (count == 0) {
		return;
	}
	std::vector<MidiEvent*> newlist;
	newlist.reserve(list.size() - count);
	for (int i=0; i<(int)list.size(); i++) {
		if (list[i]) {
			newlist.push_back(list[i]);
		}
	}
	list.swap(newlist);
}



//////////////////////////////
//
// MidiEventList::linkNotePairs -- Match note-ones and note-offs together
//   There are two models that can be done if two notes are overlapping
//   on the same pitch: the first note-off affects the last note-on,
//   or the first note-off affects the first note-on.  Currently  the
//   first note-off affects the last note-on, but both methods could
//   be implemented with user selectability.  The current state of the
//   track is assumed to be in time-sorted order.  Returns the number
//   of linked notes (note-on/note-off pairs).
//

int MidiEventList::linkEventPairs(void) {
	return linkNotePairs();
}


int MidiEventList::linkNotePairs(void) {

	// Note-on states:
	// dimension 1: MIDI channel (0-15)
	// dimension 2: MIDI key     (0-127)  (but 0 not used for note-ons)
	// dimension 3: List of active note-ons or note-offs.
	std::vector<std::vector<std::vector<MidiEvent*>>> noteons;
	noteons.resize(16);
	int i;
	for (i=0; i<(int)noteons.size(); i++) {
		noteons[i].resize(128);
	}

	// Controller linking: The following General MIDI controller numbers are
	// also monitored for linking within the track (but not between tracks).
	// hex dec  name                                    range
	// 40  64   Hold pedal (Sustain) on/off             0..63=off  64..127=on
	// 41  65   Portamento on/off                       0..63=off  64..127=on
	// 42  66   Sustenuto Pedal on/off                  0..63=off  64..127=on
	// 43  67   Soft Pedal on/off                       0..63=off  64..127=on
	// 44  68   Legato Pedal on/off                     0..63=off  64..127=on
	// 45  69   Hold Pedal 2 on/off                     0..63=off  64..127=on
	// 50  80   General Purpose Button                  0..63=off  64..127=on
	// 51  81   General Purpose Button                  0..63=off  64..127=on
	// 52  82   General Purpose Button                  0..63=off  64..127=on
	// 53  83   General Purpose Button                  0..63=off  64..127=on
	// 54  84   Undefined on/off                        0..63=off  64..127=on
	// 55  85   Undefined on/off                        0..63=off  64..127=on
	// 56  86   Undefined on/off                        0..63=off  64..127=on
	// 57  87   Undefined on/off                        0..63=off  64..127=on
	// 58  88   Undefined on/off                        0..63=off  64..127=on
	// 59  89   Undefined on/off                        0..63=off  64..127=on
	// 5A  90   Undefined on/off                        0..63=off  64..127=on
	// 7A 122   Local Keyboard On/Off                   0..63=off  64..127=on

	// first keep track of whether the controller is an on/off switch:
	std::vector<std::pair<int, int>> contmap;
	contmap.resize(128);
	std::pair<int, int> zero(0, 0);
	std::fill(contmap.begin(), contmap.end(), zero);
	contmap[64].first  = 1;   contmap[64].second = 0;
	contmap[65].first  = 1;   contmap[65].second = 1;
	contmap[66].first  = 1;   contmap[66].second = 2;
	contmap[67].first  = 1;   contmap[67].second = 3;
	contmap[68].first  = 1;   contmap[68].second = 4;
	contmap[69].first  = 1;   contmap[69].second = 5;
	contmap[80].first  = 1;   contmap[80].second = 6;
	contmap[81].first  = 1;   contmap[81].second = 7;
	contmap[82].first  = 1;   contmap[82].second = 8;
	contmap[83].first  = 1;   contmap[83].second = 9;
	contmap[84].first  = 1;   contmap[84].second = 10;
	contmap[85].first  = 1;   contmap[85].second = 11;
	contmap[86].first  = 1;   contmap[86].second = 12;
	contmap[87].first  = 1;   contmap[87].second = 13;
	contmap[88].first  = 1;   contmap[88].second = 14;
	contmap[89].first  = 1;   contmap[89].second = 15;
	contmap[90].first  = 1;   contmap[90].second = 16;
	contmap[122].first = 1;   contmap[122].second = 17;

	// dimensions:
	// 1: mapped controller (0 to 17)
	// 2: channel (0 to 15)
	std::vector<std::vector<MidiEvent*>> contevents;
	contevents.resize(18);
	std::vector<std::vector<int>> oldstates;
	oldstates.resize(18);
	for (int i=0; i<18; i++) {
		contevents[i].resize(16);
		std::fill(contevents[i].begin(), contevents[i].end(), nullptr);
		oldstates[i].resize(16);
		std::fill(oldstates[i].begin(), oldstates[i].end(), -1);
	}

	// Now iterate through the MidiEventList keeping track of note and
	// select controller states and linking notes/controllers as needed.
	int channel;
	int key;
	int contnum;
	int contval;
	int conti;
	int contstate;
	int counter = 0;
	MidiEvent* mev;
	MidiEvent* noteon;
	for (i=0; i<getSize(); i++) {
		mev = &getEvent(i);
		mev->unlinkEvent();
		if (mev->isNoteOn()) {
			// store the note-on to pair later with a note-off message.
			key = mev->getKeyNumber();
			channel = mev->getChannel();
			noteons[channel][key].push_back(mev);
		} else if (mev->isNoteOff()) {
			key = mev->getKeyNumber();
			channel = mev->getChannel();
			if (noteons[channel][key].size() > 0) {
				noteon = noteons[channel][key].back();
				noteons[channel][key].pop_back();
				noteon->linkEvent(mev);
				counter++;
			}
		} else if (mev->isController()) {
			contnum = mev->getP1();
			if (contmap[contnum].first) {
				conti     = contmap[contnum].second;
				channel   = mev->getChannel();
				contval   = mev->getP2();
				contstate = contval < 64 ? 0 : 1;
				if ((oldstates[conti][channel] == -1) && contstate) {
					// a newly initialized onstate was detected, so store for
					// later linking to an off state.
					contevents[conti][channel] = mev;
					oldstates[conti][channel] = contstate;
				} else if (oldstates[conti][channel] == contstate) {
					// the controller state is redundant and will be ignored.
				} else if ((oldstates[conti][channel] == 0) && contstate) {
					// controller is currently off, so store on-state for next link
					contevents[conti][channel] = mev;
					oldstates[conti][channel] = contstate;
				} else if ((oldstates[conti][channel] == 1) && (contstate == 0)) {
					// controller has just been turned off, so link to
					// stored on-message.
					contevents[conti][channel]->linkEvent(mev);
					oldstates[conti][channel] = contstate;
					// not necessary, but maybe use for something later:
					contevents[conti][channel] = mev;
				}
			}
		}
	}
	return counter;
}



//////////////////////////////
//
// MidiEventList::clearLinks -- remove all note-on/note-off links.
//

void MidiEventList::clearLinks(void) {
	for (int i=0; i<(int)getSize(); i++) {
		getEvent(i).unlinkEvent();
	}
}



//////////////////////////////
//
// MidiEventList::clearSequence -- Remove any seqence serial numbers from
//   MidiEvents in the list.  This will cause the default ordering by
//   sortTracks() to be used, in which case the ordering of MidiEvents
//   occurring at the same tick may switch their ordering.
//

void MidiEventList::clearSequence(void) {
	for (int i=0; i<getEventCount(); i++) {
		getEvent(i).seq = 0;
	}
}



//////////////////////////////
//
// MidiEventList::markSequence -- Assign a sequence serial number to
//   every MidiEvent in the event list.  This is useful if you want
//   to preseve the order of MIDI messages in a track when they occur
//   at the same tick time.  Particularly for use with joinTracks()
//   or sortTracks().  markSequence will be done automatically when
//   a MIDI file is read, in case the ordering of events occuring at
//   the same time is important.  Use clearSequence() to use the
//   default sorting behavior of sortTracks() when events occur at the
//   same time.  Returns the next serial number that has not yet been
//   used.
//   default value: sequence = 1.
//

int MidiEventList::markSequence(int sequence) {
	for (int i=0; i<getEventCount(); i++) {
		getEvent(i).seq = sequence++;
	}
	return sequence;
}


///////////////////////////////////////////////////////////////////////////
//
// protected functions --
//


//////////////////////////////
//
// MidiEventList::detach -- De-allocate any MidiEvents present in the list
//    and set the size of the list to 0.
//


void MidiEventList::detach(void) {
	list.resize(0);
}



//////////////////////////////
//
// MidiEventList::push_back_no_copy -- add a MidiEvent at the end of
//     the list.  The event is not copied, but memory from the
//     remote location is used.  Returns the index of the appended event.
//

int MidiEventList::push_back_no_copy(MidiEvent* event) {
	list.push_back(event);
	return (int)list.size()-1;
}



//////////////////////////////
//
// MidiEventList::operator=(MidiEventList) -- Assignment.
//

MidiEventList& MidiEventList::operator=(MidiEventList& other) {
	list.swap(other.list);
	return *this;
}


///////////////////////////////////////////////////////////////////////////
//
// private functions
//

//////////////////////////////
//
// MidiEventList::sort -- Private because the MidiFile class keeps
//    track of delta versus absolute tick states of the MidiEventList,
//    and sorting is only allowed in absolute tick state (The MidiEventList
//    does not know about delta/absolute tick states of its contents).
//

void MidiEventList::sort(void) {
	qsort(data(), getEventCount(), sizeof(MidiEvent*), eventcompare);
}



///////////////////////////////////////////////////////////////////////////
//
// external functions
//

//////////////////////////////
//
// eventcompare -- Event comparison function for sorting tracks.
//
// Sorting rules:
//    (1) sort by (absolute) tick value; otherwise, if tick values are the same:
//    (2) end-of-track meta message is always last.
//    (3) other meta-messages come before regular MIDI messages.
//    (4) note-offs come after all other regular MIDI messages except note-ons.
//    (5) note-ons come after all other regular MIDI messages.
//

int eventcompare(const void* a, const void* b) {
	MidiEvent& aevent = **((MidiEvent**)a);
	MidiEvent& bevent = **((MidiEvent**)b);

	if (aevent.tick > bevent.tick) {
		// aevent occurs after bevent
		return +1;
	} else if (aevent.tick < bevent.tick) {
		// aevent occurs before bevent
		return -1;
	} else if ((aevent.seq != 0) && (bevent.seq != 0) && (aevent.seq > bevent.seq)) {
		// aevent sequencing state occurs after bevent
		// see MidiEventList::markSequence()
		return +1;
	} else if ((aevent.seq != 0) && (bevent.seq != 0) && (aevent.seq < bevent.seq)) {
		// aevent sequencing state occurs before bevent
		// see MidiEventList::markSequence()
		return -1;
	} else if (aevent.getP0() == 0xff && aevent.getP1() == 0x2f) {
		// end-of-track meta-message should always be last (but won't really
		// matter since the writing function ignores all end-of-track messages
		// and writes its own.
		return +1;
	} else if (bevent.getP0() == 0xff && bevent.getP1() == 0x2f) {
		// end-of-track meta-message should always be last (but won't really
		// matter since the writing function ignores all end-of-track messages
		// and writes its own.
		return -1;
	} else if (aevent.getP0() == 0xff && bevent.getP0() != 0xff) {
		// other meta-messages are placed before real MIDI messages
		return -1;
	} else if (aevent.getP0() != 0xff && bevent.getP0() == 0xff) {
		// other meta-messages are placed before real MIDI messages
		return +1;
	} else if (((aevent.getP0() & 0xf0) == 0x90) && (aevent.getP2() != 0)) {
		// note-ons come after all other types of MIDI messages
		return +1;
	} else if (((bevent.getP0() & 0xf0) == 0x90) && (bevent.getP2() != 0)) {
		// note-ons come after all other types of MIDI messages
		return -1;
	} else if (((aevent.getP0() & 0xf0) == 0x90) || ((aevent.getP0() & 0xf0) == 0x80)) {
		// note-offs come after all other MIDI messages (except note-ons)
		return +1;
	} else if (((bevent.getP0() & 0xf0) == 0x90) || ((bevent.getP0() & 0xf0) == 0x80)) {
		// note-offs come after all other MIDI messages (except note-ons)
		return -1;
	} else if (((aevent.getP0() & 0xf0) == 0xb0) && ((bevent.getP0() & 0xf0) == 0xb0)) {
		// both events are continuous controllers.  Sort them by controller number
		if (aevent.getP1() > bevent.getP1()) {
			return +1;
		} if (aevent.getP1() < bevent.getP1()) {
			return -1;
		} else {
			// same controller number, so sort by data value
			if (aevent.getP2() > bevent.getP2()) {
				return +1;
			} if (aevent.getP2() < bevent.getP2()) {
				return -1;
			} else {
				return 0;
			}
		}
	} else {
		return 0;
	}
}


//////////////////////////////
//
// MidiFile::MidiFile -- Constuctor.
//

MidiFile::MidiFile(void) {
	m_events.resize(m_trackCount);
	for (int i=0; i<m_trackCount; i++) {
		m_events[i] = new MidiEventList;
	}
}


MidiFile::MidiFile(const std::string& filename) {
	m_events.resize(m_trackCount);
	for (int i=0; i<m_trackCount; i++) {
		m_events[i] = new MidiEventList;
	}
	read(filename);
}


MidiFile::MidiFile(std::istream& input) {
	m_events.resize(m_trackCount);
	for (int i=0; i<m_trackCount; i++) {
		m_events[i] = new MidiEventList;
	}
	read(input);
}



MidiFile::MidiFile(const MidiFile& other) {
	*this = other;
}



MidiFile::MidiFile(MidiFile&& other) {
	*this = std::move(other);
}



//////////////////////////////
//
// MidiFile::~MidiFile -- Deconstructor.
//

MidiFile::~MidiFile() {
	m_readFileName.clear();
	clear();
	if (m_events[0] != NULL) {
		delete m_events[0];
		m_events[0] = NULL;
	}
	m_events.resize(0);
	m_rwstatus = false;
	m_timemap.clear();
	m_timemapvalid = 0;
}



//////////////////////////////
//
// MidiFile::operator= -- Copying another
//

MidiFile& MidiFile::operator=(const MidiFile& other) {
	if (this == &other) {
		return *this;
	}
	m_events.reserve(other.m_events.size());
	auto it = other.m_events.begin();
	std::generate_n(std::back_inserter(m_events), other.m_events.size(),
		[&]()->MidiEventList* {
			return new MidiEventList(**it++);
		}
	);
	m_ticksPerQuarterNote = other.m_ticksPerQuarterNote;
	m_trackCount          = other.m_trackCount;
	m_theTrackState       = other.m_theTrackState;
	m_theTimeState        = other.m_theTimeState;
	m_readFileName        = other.m_readFileName;
	m_timemapvalid        = other.m_timemapvalid;
	m_timemap             = other.m_timemap;
	m_rwstatus            = other.m_rwstatus;
	if (other.m_linkedEventsQ) {
		linkEventPairs();
	}
	return *this;
}


MidiFile& MidiFile::operator=(MidiFile&& other) {
	m_events = std::move(other.m_events);
	m_linkedEventsQ = other.m_linkedEventsQ;
	other.m_linkedEventsQ = false;
	other.m_events.clear();
	other.m_events.emplace_back(new MidiEventList);
	m_ticksPerQuarterNote = other.m_ticksPerQuarterNote;
	m_trackCount          = other.m_trackCount;
	m_theTrackState       = other.m_theTrackState;
	m_theTimeState        = other.m_theTimeState;
	m_readFileName        = other.m_readFileName;
	m_timemapvalid        = other.m_timemapvalid;
	m_timemap             = other.m_timemap;
	m_rwstatus            = other.m_rwstatus;
	return *this;
}


///////////////////////////////////////////////////////////////////////////
//
// reading/writing functions --
//

//////////////////////////////
//
// MidiFile::read -- Parse a Standard MIDI File and store its contents
//      in the object.
//

bool MidiFile::read(const std::string& filename) {
	m_timemapvalid = 0;
	setFilename(filename);
	m_rwstatus = true;

	std::fstream input;
	input.open(filename.c_str(), std::ios::binary | std::ios::in);

	if (!input.is_open()) {
		m_rwstatus = false;
		return m_rwstatus;
	}

	m_rwstatus = read(input);
	return m_rwstatus;
}

//
// istream version of read().
//

bool MidiFile::read(std::istream& input) {
	m_rwstatus = true;
	if (input.peek() != 'M') {
		// If the first byte in the input stream is not 'M', then presume that
		// the MIDI file is in the binasc format which is an ASCII representation
		// of the MIDI file.  Convert the binasc content into binary content and
		// then continue reading with this function.
		std::stringstream binarydata;
		Binasc binasc;
		binasc.writeToBinary(binarydata, input);
		binarydata.seekg(0, std::ios_base::beg);
		if (binarydata.peek() != 'M') {
			std::cerr << "Bad MIDI data input" << std::endl;
			m_rwstatus = false;
			return m_rwstatus;
		} else {
			m_rwstatus = read(binarydata);
			return m_rwstatus;
		}
	}

	std::string filename = getFilename();

	int    character;
	// uchar  buffer[123456] = {0};
	ulong  longdata;
	ushort shortdata;


	// Read the MIDI header (4 bytes of ID, 4 byte data size,
	// anticipated 6 bytes of data.

	character = input.get();
	if (character == EOF) {
		std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
		std::cerr << "Expecting 'M' at first byte, but found nothing." << std::endl;
		m_rwstatus = false; return m_rwstatus;
	} else if (character != 'M') {
		std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
		std::cerr << "Expecting 'M' at first byte but got '"
		     << (char)character << "'" << std::endl;
		m_rwstatus = false; return m_rwstatus;
	}

	character = input.get();
	if (character == EOF) {
		std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
		std::cerr << "Expecting 'T' at second byte, but found nothing." << std::endl;
		m_rwstatus = false; return m_rwstatus;
	} else if (character != 'T') {
		std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
		std::cerr << "Expecting 'T' at second byte but got '"
		     << (char)character << "'" << std::endl;
		m_rwstatus = false; return m_rwstatus;
	}

	character = input.get();
	if (character == EOF) {
		std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
		std::cerr << "Expecting 'h' at third byte, but found nothing." << std::endl;
		m_rwstatus = false; return m_rwstatus;
	} else if (character != 'h') {
		std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
		std::cerr << "Expecting 'h' at third byte but got '"
		     << (char)character << "'" << std::endl;
		m_rwstatus = false; return m_rwstatus;
	}

	character = input.get();
	if (character == EOF) {
		std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
		std::cerr << "Expecting 'd' at fourth byte, but found nothing." << std::endl;
		m_rwstatus = false; return m_rwstatus;
	} else if (character != 'd') {
		std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
		std::cerr << "Expecting 'd' at fourth byte but got '"
		     << (char)character << "'" << std::endl;
		m_rwstatus = false; return m_rwstatus;
	}

	// read header size (allow larger header size?)
	longdata = readLittleEndian4Bytes(input);
	if (longdata != 6) {
		std::cerr << "File " << filename
		     << " is not a MIDI 1.0 Standard MIDI file." << std::endl;
		std::cerr << "The header size is " << longdata << " bytes." << std::endl;
		m_rwstatus = false; return m_rwstatus;
	}

	// Header parameter #1: format type
	int type;
	shortdata = readLittleEndian2Bytes(input);
	switch (shortdata) {
		case 0:
			type = 0;
			break;
		case 1:
			type = 1;
			break;
		case 2:
			// Type-2 MIDI files should probably be allowed as well,
			// but I have never seen one in the wild to test with.
		default:
			std::cerr << "Error: cannot handle a type-" << shortdata
			     << " MIDI file" << std::endl;
			m_rwstatus = false; return m_rwstatus;
	}

	// Header parameter #2: track count
	int tracks;
	shortdata = readLittleEndian2Bytes(input);
	if (type == 0 && shortdata != 1) {
		std::cerr << "Error: Type 0 MIDI file can only contain one track" << std::endl;
		std::cerr << "Instead track count is: " << shortdata << std::endl;
		m_rwstatus = false; return m_rwstatus;
	} else {
		tracks = shortdata;
	}
	clear();
	if (m_events[0] != NULL) {
		delete m_events[0];
	}
	m_events.resize(tracks);
	for (int z=0; z<tracks; z++) {
		m_events[z] = new MidiEventList;
		m_events[z]->reserve(10000);   // Initialize with 10,000 event storage.
		m_events[z]->clear();
	}

	// Header parameter #3: Ticks per quarter note
	shortdata = readLittleEndian2Bytes(input);
	if (shortdata >= 0x8000) {
		int framespersecond = 255 - ((shortdata >> 8) & 0x00ff) + 1;
		int subframes       = shortdata & 0x00ff;
		switch (framespersecond) {
			case 25:  framespersecond = 25; break;
			case 24:  framespersecond = 24; break;
			case 29:  framespersecond = 29; break;  // really 29.97 for color television
			case 30:  framespersecond = 30; break;
			default:
					std::cerr << "Warning: unknown FPS: " << framespersecond << std::endl;
					std::cerr << "Using non-standard FPS: " << framespersecond << std::endl;
		}
		m_ticksPerQuarterNote = framespersecond * subframes;

		// std::cerr << "SMPTE ticks: " << m_ticksPerQuarterNote << " ticks/sec" << std::endl;
		// std::cerr << "SMPTE frames per second: " << framespersecond << std::endl;
		// std::cerr << "SMPTE subframes per frame: " << subframes << std::endl;
	}  else {
		m_ticksPerQuarterNote = shortdata;
	}


	//////////////////////////////////////////////////
	//
	// now read individual tracks:
	//

	uchar runningCommand;
	MidiEvent event;
	std::vector<uchar> bytes;
	int xstatus;
	// int barline;

	for (int i=0; i<tracks; i++) {
		runningCommand = 0;

		// std::cout << "\nReading Track: " << i + 1 << flush;

		// read track header...

		character = input.get();
		if (character == EOF) {
			std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
			std::cerr << "Expecting 'M' at first byte in track, but found nothing."
			     << std::endl;
			m_rwstatus = false; return m_rwstatus;
		} else if (character != 'M') {
			std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
			std::cerr << "Expecting 'M' at first byte in track but got '"
			     << (char)character << "'" << std::endl;
			m_rwstatus = false; return m_rwstatus;
		}

		character = input.get();
		if (character == EOF) {
			std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
			std::cerr << "Expecting 'T' at second byte in track, but found nothing."
			     << std::endl;
			m_rwstatus = false; return m_rwstatus;
		} else if (character != 'T') {
			std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
			std::cerr << "Expecting 'T' at second byte in track but got '"
			     << (char)character << "'" << std::endl;
			m_rwstatus = false; return m_rwstatus;
		}

		character = input.get();
		if (character == EOF) {
			std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
			std::cerr << "Expecting 'r' at third byte in track, but found nothing."
			     << std::endl;
			m_rwstatus = false; return m_rwstatus;
		} else if (character != 'r') {
			std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
			std::cerr << "Expecting 'r' at third byte in track but got '"
			     << (char)character << "'" << std::endl;
			m_rwstatus = false; return m_rwstatus;
		}

		character = input.get();
		if (character == EOF) {
			std::cerr << "In file " << filename << ": unexpected end of file." << std::endl;
			std::cerr << "Expecting 'k' at fourth byte in track, but found nothing."
			     << std::endl;
			m_rwstatus = false; return m_rwstatus;
		} else if (character != 'k') {
			std::cerr << "File " << filename << " is not a MIDI file" << std::endl;
			std::cerr << "Expecting 'k' at fourth byte in track but got '"
			     << (char)character << "'" << std::endl;
			m_rwstatus = false; return m_rwstatus;
		}

		// Now read track chunk size and throw it away because it is
		// not really necessary since the track MUST end with an
		// end of track meta event, and many MIDI files found in the wild
		// do not correctly give the track size.
		longdata = readLittleEndian4Bytes(input);

		// set the size of the track allocation so that it might
		// approximately fit the data.
		m_events[i]->reserve((int)longdata/2);
		m_events[i]->clear();

		// process the track
		int absticks = 0;
		// barline = 1;
		while (!input.eof()) {
			longdata = readVLValue(input);
			//std::cout << "ticks = " << longdata << std::endl;
			absticks += longdata;
			xstatus = extractMidiData(input, bytes, runningCommand);
			if (xstatus == 0) {
				m_rwstatus = false;  return m_rwstatus;
			}
			event.setMessage(bytes);
			//std::cout << "command = " << std::hex << (int)event.data[0] << std::dec << std::endl;
			if (bytes[0] == 0xff && (bytes[1] == 1 ||
					bytes[1] == 2 || bytes[1] == 3 || bytes[1] == 4)) {
				// mididata.push_back('\0');
				// std::cout << '\t';
				// for (int m=0; m<event.data[2]; m++) {
				//    std::cout << event.data[m+3];
				// }
				// std::cout.flush();
			} else if (bytes[0] == 0xff && bytes[1] == 0x2f) {
				// end of track message
				// uncomment out the following three lines if you don't want
				// to see the end of track message (which is always required,
				// and added automatically when a MIDI is written.
				event.tick = absticks;
				event.track = i;
				m_events[i]->push_back(event);
				break;
			}

			if (bytes[0] != 0xff && bytes[0] != 0xf0) {
				event.tick = absticks;
				event.track = i;
				m_events[i]->push_back(event);
			} else {
				event.tick = absticks;
				event.track = i;
				m_events[i]->push_back(event);
			}

		}

	}

	m_theTimeState = TIME_STATE_ABSOLUTE;
	markSequence();
	return m_rwstatus;
}



//////////////////////////////
//
// MidiFile::write -- write a standard MIDI file to a file or an output
//    stream.
//

bool MidiFile::write(const std::string& filename) {
	std::fstream output(filename.c_str(), std::ios::binary | std::ios::out);

	if (!output.is_open()) {
		std::cerr << "Error: could not write: " << filename << std::endl;
		return false;
	}
	m_rwstatus = write(output);
	output.close();
	return m_rwstatus;
}

//
// ostream version of MidiFile::write().
//

bool MidiFile::write(std::ostream& out) {
	int oldTimeState = getTickState();
	if (oldTimeState == TIME_STATE_ABSOLUTE) {
		makeDeltaTicks();
	}

	// write the header of the Standard MIDI File
	char ch;
	// 1. The characters "MThd"
	ch = 'M'; out << ch;
	ch = 'T'; out << ch;
	ch = 'h'; out << ch;
	ch = 'd'; out << ch;

	// 2. write the size of the header (always a "6" stored in unsigned long
	//    (4 bytes).
	ulong longdata = 6;
	writeBigEndianULong(out, longdata);

	// 3. MIDI file format, type 0, 1, or 2
	ushort shortdata;
	shortdata = (getNumTracks() == 1) ? 0 : 1;
	writeBigEndianUShort(out,shortdata);

	// 4. write out the number of tracks.
	shortdata = getNumTracks();
	writeBigEndianUShort(out, shortdata);

	// 5. write out the number of ticks per quarternote. (avoiding SMTPE for now)
	shortdata = getTicksPerQuarterNote();
	writeBigEndianUShort(out, shortdata);

	// now write each track.
	std::vector<uchar> trackdata;
	uchar endoftrack[4] = {0, 0xff, 0x2f, 0x00};
	int i, j, k;
	int size;
	for (i=0; i<getNumTracks(); i++) {
		trackdata.reserve(123456);   // make the track data larger than
		                             // expected data input
		trackdata.clear();
		for (j=0; j<(int)m_events[i]->size(); j++) {
			if ((*m_events[i])[j].empty()) {
				// Don't write empty m_events (probably a delete message).
				continue;
			}
			if ((*m_events[i])[j].isEndOfTrack()) {
				// Suppress end-of-track meta messages (one will be added
				// automatically after all track data has been written).
				continue;
			}
			writeVLValue((*m_events[i])[j].tick, trackdata);
			if (((*m_events[i])[j].getCommandByte() == 0xf0) ||
					((*m_events[i])[j].getCommandByte() == 0xf7)) {
				// 0xf0 == Complete sysex message (0xf0 is part of the raw MIDI).
				// 0xf7 == Raw byte message (0xf7 not part of the raw MIDI).
				// Print the first byte of the message (0xf0 or 0xf7), then
				// print a VLV length for the rest of the bytes in the message.
				// In other words, when creating a 0xf0 or 0xf7 MIDI message,
				// do not insert the VLV byte length yourself, as this code will
				// do it for you automatically.
				trackdata.push_back((*m_events[i])[j][0]); // 0xf0 or 0xf7;
				writeVLValue(((int)(*m_events[i])[j].size())-1, trackdata);
				for (k=1; k<(int)(*m_events[i])[j].size(); k++) {
					trackdata.push_back((*m_events[i])[j][k]);
				}
			} else {
				// non-sysex type of message, so just output the
				// bytes of the message:
				for (k=0; k<(int)(*m_events[i])[j].size(); k++) {
					trackdata.push_back((*m_events[i])[j][k]);
				}
			}
		}
		size = (int)trackdata.size();
		if ((size < 3) || !((trackdata[size-3] == 0xff)
				&& (trackdata[size-2] == 0x2f))) {
			trackdata.push_back(endoftrack[0]);
			trackdata.push_back(endoftrack[1]);
			trackdata.push_back(endoftrack[2]);
			trackdata.push_back(endoftrack[3]);
		}

		// now ready to write to MIDI file.

		// first write the track ID marker "MTrk":
		ch = 'M'; out << ch;
		ch = 'T'; out << ch;
		ch = 'r'; out << ch;
		ch = 'k'; out << ch;

		// A. write the size of the MIDI data to follow:
		longdata = (int)trackdata.size();
		writeBigEndianULong(out, longdata);

		// B. write the actual data
		out.write((char*)trackdata.data(), trackdata.size());
	}

	if (oldTimeState == TIME_STATE_ABSOLUTE) {
		makeAbsoluteTicks();
	}

	return true;
}



//////////////////////////////
//
// MidiFile::writeHex -- print the Standard MIDI file as a list of
//    ASCII Hex bytes, formatted 25 to a line by default, and
//    two digits for each hex byte code.  If the input width is 0,
//    then don't wrap lines.
//
//  default value: width=25
//

bool MidiFile::writeHex(const std::string& filename, int width) {
	std::fstream output(filename.c_str(), std::ios::out);
	if (!output.is_open()) {
		std::cerr << "Error: could not write: " << filename << std::endl;
		return false;
	}
	m_rwstatus = writeHex(output, width);
	output.close();
	return m_rwstatus;
}

//
// ostream version of MidiFile::writeHex().
//

bool MidiFile::writeHex(std::ostream& out, int width) {
	std::stringstream tempstream;
	MidiFile::write(tempstream);
	int len = (int)tempstream.str().length();
	int wordcount = 1;
	int linewidth = width >= 0 ? width : 25;
	for (int i=0; i<len; i++) {
		int value = (unsigned char)tempstream.str()[i];
		out << std::hex << std::setw(2) << std::setfill('0') << value;
		if (linewidth) {
			if (i < len - 1) {
				out << ((wordcount % linewidth) ? ' ' : '\n');
			}
			wordcount++;
		} else {
			// print with no line breaks
			if (i < len - 1) {
				out << ' ';
			}
		}
	}
	if (linewidth) {
		out << '\n';
	}
	return true;
}



//////////////////////////////
//
// MidiFile::writeBinasc -- write a standard MIDI file from data into
//    the binasc format (ASCII version of the MIDI file).
//

bool MidiFile::writeBinasc(const std::string& filename) {
	std::fstream output(filename.c_str(), std::ios::out);

	if (!output.is_open()) {
		std::cerr << "Error: could not write: " << filename << std::endl;
		return false;
	}
	m_rwstatus = writeBinasc(output);
	output.close();
	return m_rwstatus;
}

//
// ostream version of MidiFile::writeBinasc().
//

bool MidiFile::writeBinasc(std::ostream& output) {
	std::stringstream binarydata;
	m_rwstatus = write(binarydata);
	if (m_rwstatus == false) {
		return false;
	}

	Binasc binasc;
	binasc.setMidiOn();
	binarydata.seekg(0, std::ios_base::beg);
	binasc.readFromBinary(output, binarydata);
	return true;
}



//////////////////////////////
//
// MidiFile::writeBinascWithComents -- write a standard MIDI
//    file from data into the binasc format (ASCII version
//    of the MIDI file), including commentary about the MIDI messages.
//

bool MidiFile::writeBinascWithComments(const std::string& filename) {
	std::fstream output(filename.c_str(), std::ios::out);

	if (!output.is_open()) {
		std::cerr << "Error: could not write: " << filename << std::endl;
		return 0;
	}
	m_rwstatus = writeBinascWithComments(output);
	output.close();
	return m_rwstatus;
}

//
// ostream version of MidiFile::writeBinascWithComments().
//

bool MidiFile::writeBinascWithComments(std::ostream& output) {
	std::stringstream binarydata;
	m_rwstatus = write(binarydata);
	if (m_rwstatus == false) {
		return false;
	}

	Binasc binasc;
	binasc.setMidiOn();
	binasc.setCommentsOn();
	binarydata.seekg(0, std::ios_base::beg);
	binasc.readFromBinary(output, binarydata);
	return true;
}



//////////////////////////////
//
// MidiFile::status -- return the success flag from the last read or
//    write (writeHex, writeBinasc).
//

bool MidiFile::status(void) const {
	return m_rwstatus;
}


///////////////////////////////////////////////////////////////////////////
//
// track-related functions --
//

//////////////////////////////
//
// MidiFile::operator[] -- return the event list for the specified track.
//

MidiEventList& MidiFile::operator[](int aTrack) {
	return *m_events[aTrack];
}

const MidiEventList& MidiFile::operator[](int aTrack) const {
	return *m_events[aTrack];
}


//////////////////////////////
//
// MidiFile::getTrackCount -- return the number of tracks in
//   the Midi File.
//

int MidiFile::getTrackCount(void) const {
	return (int)m_events.size();
}

//
// Alias for getTrackCount()
//

int MidiFile::getNumTracks(void) const {
	return getTrackCount();
}

//
// Alias for getTrackCount()
//

int MidiFile::size(void) const {
	return getTrackCount();
}



//////////////////////////////
//
// MidiFile::removeEmpties -- Remove any MIDI message that
//     contains no bytes.
//

void MidiFile::removeEmpties(void) {
	for (int i=0; i<(int)m_events.size(); i++) {
		m_events[i]->removeEmpties();
	}
}



//////////////////////////////
//
// MidiFile::markSequence -- Assign a sequence serial number to
//   every MidiEvent in every track in the MIDI file.  This is
//   useful if you want to preseve the order of MIDI messages in
//   a track when they occur at the same tick time.  Particularly
//   for use with joinTracks() or sortTracks().  markSequence will
//   be done automatically when a MIDI file is read, in case the
//   ordering of m_events occuring at the same time is important.
//   Use clearSequence() to use the default sorting behavior of
//   sortTracks().
//

void MidiFile::markSequence(void) {
	int sequence = 1;
	for (int i=0; i<getTrackCount(); i++) {
		sequence = operator[](i).markSequence(sequence);
	}
}

//
// MidiFile::markSequence -- default value: sequence = 1.
//

void MidiFile::markSequence(int track, int sequence) {
	if ((track >= 0) && (track < getTrackCount())) {
		operator[](track).markSequence(sequence);
	} else {
		std::cerr << "Warning: track " << track << " does not exist." << std::endl;
	}
}



//////////////////////////////
//
// MidiFile::clearSequence -- Remove any seqence serial numbers from
//   MidiEvents in the MidiFile.  This will cause the default ordering by
//   sortTracks() to be used, in which case the ordering of MidiEvents
//   occurring at the same tick may switch their ordering.
//

void MidiFile::clearSequence(void) {
	for (int i=0; i<getTrackCount(); i++) {
		operator[](i).clearSequence();
	}
}


void MidiFile::clearSequence(int track) {
	if ((track >= 0) && (track < getTrackCount())) {
		operator[](track).clearSequence();
	} else {
		std::cerr << "Warning: track " << track << " does not exist." << std::endl;
	}
}



//////////////////////////////
//
// MidiFile::joinTracks -- Interleave the data from all tracks,
//   but keeping the identity of the tracks unique so that
//   the function splitTracks can be called to split the
//   tracks into separate units again.  The style of the
//   MidiFile when read from a file is with tracks split.
//   The original track index is stored in the MidiEvent::track
//   variable.
//

void MidiFile::joinTracks(void) {
	if (getTrackState() == TRACK_STATE_JOINED) {
		return;
	}
	if (getNumTracks() == 1) {
		m_theTrackState = TRACK_STATE_JOINED;
		return;
	}

	MidiEventList* joinedTrack;
	joinedTrack = new MidiEventList;

	int messagesum = 0;
	int length = getNumTracks();
	int i, j;
	for (i=0; i<length; i++) {
		messagesum += (*m_events[i]).size();
	}
	joinedTrack->reserve((int)(messagesum + 32 + messagesum * 0.1));

	int oldTimeState = getTickState();
	if (oldTimeState == TIME_STATE_DELTA) {
		makeAbsoluteTicks();
	}
	for (i=0; i<length; i++) {
		for (j=0; j<(int)m_events[i]->size(); j++) {
			joinedTrack->push_back_no_copy(&(*m_events[i])[j]);
		}
	}

	clear_no_deallocate();

	delete m_events[0];
	m_events.resize(0);
	m_events.push_back(joinedTrack);
	sortTracks();
	if (oldTimeState == TIME_STATE_DELTA) {
		makeDeltaTicks();
	}

	m_theTrackState = TRACK_STATE_JOINED;
}



//////////////////////////////
//
// MidiFile::splitTracks -- Take the joined tracks and split them
//   back into their separate track identities.
//

void MidiFile::splitTracks(void) {
	if (getTrackState() == TRACK_STATE_SPLIT) {
		return;
	}
	int oldTimeState = getTickState();
	if (oldTimeState == TIME_STATE_DELTA) {
		makeAbsoluteTicks();
	}

	int maxTrack = 0;
	int i;
	int length = m_events[0]->size();
	for (i=0; i<length; i++) {
		if ((*m_events[0])[i].track > maxTrack) {
			maxTrack = (*m_events[0])[i].track;
		}
	}
	int m_trackCount = maxTrack + 1;

	if (m_trackCount <= 1) {
		return;
	}

	MidiEventList* olddata = m_events[0];
	m_events[0] = NULL;
	m_events.resize(m_trackCount);
	for (i=0; i<m_trackCount; i++) {
		m_events[i] = new MidiEventList;
	}

	for (i=0; i<length; i++) {
		int trackValue = (*olddata)[i].track;
		m_events[trackValue]->push_back_no_copy(&(*olddata)[i]);
	}

	olddata->detach();
	delete olddata;

	if (oldTimeState == TIME_STATE_DELTA) {
		makeDeltaTicks();
	}

	m_theTrackState = TRACK_STATE_SPLIT;
}



//////////////////////////////
//
// MidiFile::splitTracksByChannel -- Take the joined tracks and split them
//   back into their separate track identities.
//

void MidiFile::splitTracksByChannel(void) {
	joinTracks();
	if (getTrackState() == TRACK_STATE_SPLIT) {
		return;
	}

	int oldTimeState = getTickState();
	if (oldTimeState == TIME_STATE_DELTA) {
		makeAbsoluteTicks();
	}

	int maxTrack = 0;
	int i;
	MidiEventList& eventlist = *m_events[0];
	MidiEventList* olddata = &eventlist;
	int length = eventlist.size();
	for (i=0; i<length; i++) {
		if (eventlist[i].size() == 0) {
			continue;
		}
		if ((eventlist[i][0] & 0xf0) == 0xf0) {
			// ignore system and meta messages.
			continue;
		}
		if (maxTrack < (eventlist[i][0] & 0x0f)) {
			maxTrack = eventlist[i][0] & 0x0f;
		}
	}
	int m_trackCount = maxTrack + 2; // + 1 for expression track

	if (m_trackCount <= 1) {
		// only one channel, so don't do anything (leave as Type-0 file).
		return;
	}

	m_events[0] = NULL;
	m_events.resize(m_trackCount);
	for (i=0; i<m_trackCount; i++) {
		m_events[i] = new MidiEventList;
	}

	for (i=0; i<length; i++) {
		int trackValue = 0;
		if ((eventlist[i][0] & 0xf0) == 0xf0) {
			trackValue = 0;
		} else if (eventlist[i].size() > 0) {
			trackValue = (eventlist[i][0] & 0x0f) + 1;
		}
		m_events[trackValue]->push_back_no_copy(&eventlist[i]);
	}

	olddata->detach();
	delete olddata;

	if (oldTimeState == TIME_STATE_DELTA) {
		makeDeltaTicks();
	}

	m_theTrackState = TRACK_STATE_SPLIT;
}



//////////////////////////////
//
// MidiFile::getTrackState -- returns what type of track method
//     is being used: either TRACK_STATE_JOINED or TRACK_STATE_SPLIT.
//

int MidiFile::getTrackState(void) const {
	return m_theTrackState;
}



//////////////////////////////
//
// MidiFile::hasJoinedTracks -- Returns true if the MidiFile tracks
//    are in a joined state.
//

int MidiFile::hasJoinedTracks(void) const {
	return m_theTrackState == TRACK_STATE_JOINED;
}



//////////////////////////////
//
// MidiFile::hasSplitTracks -- Returns true if the MidiFile tracks
//     are in a split state.
//

int MidiFile::hasSplitTracks(void) const {
	return m_theTrackState == TRACK_STATE_SPLIT;
}



//////////////////////////////
//
// MidiFile::getSplitTrack --  Return the track index when the MidiFile
//   is in the split state.  This function returns the original track
//   when the MidiFile is in the joined state.  The MidiEvent::track
//   variable is used to store the original track index when the
//   MidiFile is converted to the joined-track state.
//

int MidiFile::getSplitTrack(int track, int index) const {
	if (hasSplitTracks()) {
		return track;
	} else {
		return getEvent(track, index).track;
	}
}

//
// When the parameter is void, assume track 0:
//

int MidiFile::getSplitTrack(int index) const {
	if (hasSplitTracks()) {
		return 0;
	} else {
		return getEvent(0, index).track;
	}
}



///////////////////////////////////////////////////////////////////////////
//
// tick-related functions --
//

//////////////////////////////
//
// MidiFile::makeDeltaTicks -- convert the time data to
//     delta time, which means that the time field
//     in the MidiEvent struct represents the time
//     since the last event was played. When a MIDI file
//     is read from a file, this is the default setting.
//

void MidiFile::makeDeltaTicks(void) {
	if (getTickState() == TIME_STATE_DELTA) {
		return;
	}
	int i, j;
	int temp;
	int length = getNumTracks();
	int *timedata = new int[length];
	for (i=0; i<length; i++) {
		timedata[i] = 0;
		if (m_events[i]->size() > 0) {
			timedata[i] = (*m_events[i])[0].tick;
		} else {
			continue;
		}
		for (j=1; j<(int)m_events[i]->size(); j++) {
			temp = (*m_events[i])[j].tick;
			int deltatick = temp - timedata[i];
			if (deltatick < 0) {
				std::cerr << "Error: negative delta tick value: " << deltatick << std::endl
				     << "Timestamps must be sorted first"
				     << " (use MidiFile::sortTracks() before writing)." << std::endl;
			}
			(*m_events[i])[j].tick = deltatick;
			timedata[i] = temp;
		}
	}
	m_theTimeState = TIME_STATE_DELTA;
	delete [] timedata;
}

//
// MidiFile::deltaTicks -- Alias for MidiFile::makeDeltaTicks().
//

void MidiFile::deltaTicks(void) {
	makeDeltaTicks();
}



//////////////////////////////
//
// MidiFile::makeAbsoluteTicks -- convert the time data to
//    absolute time, which means that the time field
//    in the MidiEvent struct represents the exact tick
//    time to play the event rather than the time since
//    the last event to wait untill playing the current
//    event.
//

void MidiFile::makeAbsoluteTicks(void) {
	if (getTickState() == TIME_STATE_ABSOLUTE) {
		return;
	}
	int i, j;
	int length = getNumTracks();
	int* timedata = new int[length];
	for (i=0; i<length; i++) {
		timedata[i] = 0;
		if (m_events[i]->size() > 0) {
			timedata[i] = (*m_events[i])[0].tick;
		} else {
			continue;
		}
		for (j=1; j<(int)m_events[i]->size(); j++) {
			timedata[i] += (*m_events[i])[j].tick;
			(*m_events[i])[j].tick = timedata[i];
		}
	}
	m_theTimeState = TIME_STATE_ABSOLUTE;
	delete [] timedata;
}

//
// MidiFile::absoluteTicks -- Alias for MidiFile::makeAbsoluteTicks().
//

void MidiFile::absoluteTicks(void) {
	makeAbsoluteTicks();
}



//////////////////////////////
//
// MidiFile::getTickState -- returns what type of time method is
//   being used: either TIME_STATE_ABSOLUTE or TIME_STATE_DELTA.
//

int MidiFile::getTickState(void) const {
	return m_theTimeState;
}



//////////////////////////////
//
// MidiFile::isDeltaTicks -- Returns true if MidiEvent .tick
//    variables are in delta time mode.
//

bool MidiFile::isDeltaTicks(void) const {
	return m_theTimeState == TIME_STATE_DELTA ? true : false;
}



//////////////////////////////
//
// MidiFile::isAbsoluteTicks -- Returns true if MidiEvent .tick
//    variables are in absolute time mode.
//

bool MidiFile::isAbsoluteTicks(void) const {
	return m_theTimeState == TIME_STATE_ABSOLUTE ? true : false;
}



//////////////////////////////
//
// MidiFile::getFileDurationInTicks -- Returns the largest
//    tick value in any track.  The tracks must be sorted
//    before calling this function, since this function
//    assumes that the last MidiEvent in the track has the
//    highest tick timestamp.  The file state can be in delta
//    ticks since this function will temporarily go to absolute
//    tick mode for the calculation of the max tick.
//

int MidiFile::getFileDurationInTicks(void) {
	bool revertToDelta = false;
	if (isDeltaTicks()) {
		makeAbsoluteTicks();
		revertToDelta = true;
	}
	const MidiFile& mf = *this;
	int output = 0;
	for (int i=0; i<mf.getTrackCount(); i++) {
		if (mf[i].back().tick > output) {
			output = mf[i].back().tick;
		}
	}
	if (revertToDelta) {
		deltaTicks();
	}
	return output;
}



///////////////////////////////
//
// MidiFile::getFileDurationInQuarters -- Returns the Duration of the MidiFile
//    in units of quarter notes.  If the MidiFile is in delta tick mode,
//    then temporarily got into absolute tick mode to do the calculations.
//    Note that this is expensive, so you should normally call this function
//    while in aboslute tick (default) mode.
//

double MidiFile::getFileDurationInQuarters(void) {
	return (double)getFileDurationInTicks() / (double)getTicksPerQuarterNote();
}



//////////////////////////////
//
// MidiFile::getFileDurationInSeconds -- returns the duration of the
//    logest track in the file.  The tracks must be sorted before
//    calling this function, since this function assumes that the
//    last MidiEvent in the track has the highest timestamp.
//    The file state can be in delta ticks since this function
//    will temporarily go to absolute tick mode for the calculation
//    of the max time.

double MidiFile::getFileDurationInSeconds(void) {
	if (m_timemapvalid == 0) {
		buildTimeMap();
		if (m_timemapvalid == 0) {
			return -1.0;    // something went wrong
		}
	}
	bool revertToDelta = false;
	if (isDeltaTicks()) {
		makeAbsoluteTicks();
		revertToDelta = true;
	}
	const MidiFile& mf = *this;
	double output = 0.0;
	for (int i=0; i<mf.getTrackCount(); i++) {
		if (mf[i].back().tick > output) {
			output = mf[i].back().seconds;
		}
	}
	if (revertToDelta) {
		deltaTicks();
	}
	return output;
}


///////////////////////////////////////////////////////////////////////////
//
// physical-time analysis functions --
//

//////////////////////////////
//
// MidiFile::doTimeAnalysis -- Identify the real-time position of
//    all events by monitoring the tempo in relations to the tick
//    times in the file.
//

void MidiFile::doTimeAnalysis(void) {
	buildTimeMap();
}



//////////////////////////////
//
// MidiFile::getTimeInSeconds -- return the time in seconds for
//     the current message.
//

double MidiFile::getTimeInSeconds(int aTrack, int anIndex) {
	return getTimeInSeconds(getEvent(aTrack, anIndex).tick);
}


double MidiFile::getTimeInSeconds(int tickvalue) {
	if (m_timemapvalid == 0) {
		buildTimeMap();
		if (m_timemapvalid == 0) {
			return -1.0;    // something went wrong
		}
	}

	_TickTime key;
	key.tick    = tickvalue;
	key.seconds = -1;

	void* ptr = bsearch(&key, m_timemap.data(), m_timemap.size(),
			sizeof(_TickTime), ticksearch);

	if (ptr == NULL) {
		// The specific tick value was not found, so do a linear
		// search for the two tick values which occur before and
		// after the tick value, and do a linear interpolation of
		// the time in seconds values to figure out the final
		// time in seconds.
		// Since the code is not yet written, kill the program at this point:
		return linearSecondInterpolationAtTick(tickvalue);
	} else {
		return ((_TickTime*)ptr)->seconds;
	}
}



//////////////////////////////
//
// MidiFile::getAbsoluteTickTime -- return the tick value represented
//    by the input time in seconds.  If there is not tick entry at
//    the given time in seconds, then interpolate between two values.
//

double MidiFile::getAbsoluteTickTime(double starttime) {
	if (m_timemapvalid == 0) {
		buildTimeMap();
		if (m_timemapvalid == 0) {
			if (m_timemapvalid == 0) {
				return -1.0;    // something went wrong
			}
		}
	}

	_TickTime key;
	key.tick    = -1;
	key.seconds = starttime;

	void* ptr = bsearch(&key, m_timemap.data(), m_timemap.size(),
			sizeof(_TickTime), secondsearch);

	if (ptr == NULL) {
		// The specific seconds value was not found, so do a linear
		// search for the two time values which occur before and
		// after the given time value, and do a linear interpolation of
		// the time in tick values to figure out the final time in ticks.
		return linearTickInterpolationAtSecond(starttime);
	} else {
		return ((_TickTime*)ptr)->tick;
	}

}



///////////////////////////////////////////////////////////////////////////
//
// note-analysis functions --
//

//////////////////////////////
//
// MidiFile::linkNotePairs --  Link note-ons to note-offs separately
//     for each track.  Returns the total number of note message pairs
//     that were linked.
//

int MidiFile::linkNotePairs(void) {
	int i;
	int sum = 0;
	for (i=0; i<getTrackCount(); i++) {
		if (m_events[i] == NULL) {
			continue;
		}
		sum += m_events[i]->linkNotePairs();
	}
	m_linkedEventsQ = true;
	return sum;
}

//
// MidiFile::linkEventPairs -- Alias for MidiFile::linkNotePairs().
//

int MidiFile::linkEventPairs(void) {
	return linkNotePairs();
}


///////////////////////////////////////////////////////////////////////////
//
// filename functions --
//

//////////////////////////////
//
// MidiFile::setFilename -- sets the filename of the MIDI file.
//      Currently removed any directory path.
//

void MidiFile::setFilename(const std::string& aname) {
	auto loc = aname.rfind('/');
	if (loc != std::string::npos) {
		m_readFileName = aname.substr(loc+1);
	} else {
		m_readFileName = aname;
	}
}



//////////////////////////////
//
// MidiFile::getFilename -- returns the name of the file read into the
//    structure (if the data was read from a file).
//

const char* MidiFile::getFilename(void) const {
	return m_readFileName.c_str();
}



//////////////////////////////
//
// MidiFile::addEvent --
//

MidiEvent* MidiFile::addEvent(int aTrack, int aTick,
		std::vector<uchar>& midiData) {
	m_timemapvalid = 0;
	MidiEvent* me = new MidiEvent;
	me->tick = aTick;
	me->track = aTrack;
	me->setMessage(midiData);
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addEvent -- Some bug here when joinedTracks(), but track==1...
//

MidiEvent* MidiFile::addEvent(MidiEvent& mfevent) {
	if (getTrackState() == TRACK_STATE_JOINED) {
		m_events[0]->push_back(mfevent);
		return &m_events[0]->back();
	} else {
		m_events.at(mfevent.track)->push_back(mfevent);
		return &m_events.at(mfevent.track)->back();
	}
}

//
// Variant where the track is an input parameter:
//

MidiEvent* MidiFile::addEvent(int aTrack, MidiEvent& mfevent) {
	if (getTrackState() == TRACK_STATE_JOINED) {
		m_events[0]->push_back(mfevent);
      m_events[0]->back().track = aTrack;
		return &m_events[0]->back();
	} else {
		m_events.at(aTrack)->push_back(mfevent);
		m_events.at(aTrack)->back().track = aTrack;
		return &m_events.at(aTrack)->back();
	}
}



///////////////////////////////
//
// MidiFile::addMetaEvent --
//

MidiEvent* MidiFile::addMetaEvent(int aTrack, int aTick, int aType,
		std::vector<uchar>& metaData) {
	m_timemapvalid = 0;
	int i;
	int length = (int)metaData.size();
	std::vector<uchar> fulldata;
	uchar size[23] = {0};
	int lengthsize = makeVLV(size, length);

	fulldata.resize(2+lengthsize+length);
	fulldata[0] = 0xff;
	fulldata[1] = aType & 0x7F;
	for (i=0; i<lengthsize; i++) {
		fulldata[2+i] = size[i];
	}
	for (i=0; i<length; i++) {
		fulldata[2+lengthsize+i] = metaData[i];
	}

	return addEvent(aTrack, aTick, fulldata);
}


MidiEvent* MidiFile::addMetaEvent(int aTrack, int aTick, int aType,
		const std::string& metaData) {
	int length = (int)metaData.size();
	std::vector<uchar> buffer;
	buffer.resize(length);
	int i;
	for (i=0; i<length; i++) {
		buffer[i] = (uchar)metaData[i];
	}
	return addMetaEvent(aTrack, aTick, aType, buffer);
}



//////////////////////////////
//
// MidiFile::addText --  Add a text meta-message (#1).
//

MidiEvent* MidiFile::addText(int aTrack, int aTick, const std::string& text) {
	MidiEvent* me = new MidiEvent;
	me->makeText(text);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addCopyright --  Add a copyright notice meta-message (#2).
//

MidiEvent* MidiFile::addCopyright(int aTrack, int aTick, const std::string& text) {
	MidiEvent* me = new MidiEvent;
	me->makeCopyright(text);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addTrackName --  Add an track name meta-message (#3).
//

MidiEvent* MidiFile::addTrackName(int aTrack, int aTick, const std::string& name) {
	MidiEvent* me = new MidiEvent;
	me->makeTrackName(name);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addInstrumentName --  Add an instrument name meta-message (#4).
//

MidiEvent* MidiFile::addInstrumentName(int aTrack, int aTick,
		const std::string& name) {
	MidiEvent* me = new MidiEvent;
	me->makeInstrumentName(name);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addLyric -- Add a lyric meta-message (meta #5).
//

MidiEvent* MidiFile::addLyric(int aTrack, int aTick, const std::string& text) {
	MidiEvent* me = new MidiEvent;
	me->makeLyric(text);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addMarker -- Add a marker meta-message (meta #6).
//

MidiEvent* MidiFile::addMarker(int aTrack, int aTick, const std::string& text) {
	MidiEvent* me = new MidiEvent;
	me->makeMarker(text);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addCue -- Add a cue-point meta-message (meta #7).
//

MidiEvent* MidiFile::addCue(int aTrack, int aTick, const std::string& text) {
	MidiEvent* me = new MidiEvent;
	me->makeCue(text);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addTempo -- Add a tempo meta message (meta #0x51).
//

MidiEvent* MidiFile::addTempo(int aTrack, int aTick, double aTempo) {
	MidiEvent* me = new MidiEvent;
	me->makeTempo(aTempo);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addTimeSignature -- Add a time signature meta message
//      (meta #0x58).  The "bottom" parameter must be a power of two;
//      otherwise, it will be set to the next highest power of two.
//
// Default values:
//     clocksPerClick     == 24 (quarter note)
//     num32ndsPerQuarter ==  8 (8 32nds per quarter note)
//
// Time signature of 4/4 would be:
//    top    = 4
//    bottom = 4 (converted to 2 in the MIDI file for 2nd power of 2).
//    clocksPerClick = 24 (2 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//
// Time signature of 6/8 would be:
//    top    = 6
//    bottom = 8 (converted to 3 in the MIDI file for 3rd power of 2).
//    clocksPerClick = 36 (3 eighth notes based on num32ndsPerQuarter)
//    num32ndsPerQuarter = 8
//

MidiEvent* MidiFile::addTimeSignature(int aTrack, int aTick, int top, int bottom,
		int clocksPerClick, int num32ndsPerQuarter) {
	MidiEvent* me = new MidiEvent;
	me->makeTimeSignature(top, bottom, clocksPerClick, num32ndsPerQuarter);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addCompoundTimeSignature -- Add a time signature meta message
//      (meta #0x58), where the clocksPerClick parameter is set to three
//      eighth notes for compount meters such as 6/8 which represents
//      two beats per measure.
//
// Default values:
//     clocksPerClick     == 36 (quarter note)
//     num32ndsPerQuarter ==  8 (8 32nds per quarter note)
//

MidiEvent* MidiFile::addCompoundTimeSignature(int aTrack, int aTick, int top,
		int bottom, int clocksPerClick, int num32ndsPerQuarter) {
	return addTimeSignature(aTrack, aTick, top, bottom, clocksPerClick,
		num32ndsPerQuarter);
}



//////////////////////////////
//
// MidiFile::makeVLV --  This function is used to create
//   size byte(s) for meta-messages.  If the size of the data
//   in the meta-message is greater than 127, then the size
//   should (?) be specified as a VLV.
//

int MidiFile::makeVLV(uchar *buffer, int number) {

	unsigned long value = (unsigned long)number;

	if (value >= (1 << 28)) {
		std::cerr << "Error: Meta-message size too large to handle" << std::endl;
		buffer[0] = 0;
		buffer[1] = 0;
		buffer[2] = 0;
		buffer[3] = 0;
		return 1;
	}

	buffer[0] = (value >> 21) & 0x7f;
	buffer[1] = (value >> 14) & 0x7f;
	buffer[2] = (value >>  7) & 0x7f;
	buffer[3] = (value >>  0) & 0x7f;

	int i;
	int flag = 0;
	int length = -1;
	for (i=0; i<3; i++) {
		if (buffer[i] != 0) {
			flag = 1;
		}
		if (flag) {
			buffer[i] |= 0x80;
		}
		if (length == -1 && buffer[i] >= 0x80) {
			length = 4-i;
		}
	}

	if (length == -1) {
		length = 1;
	}

	if (length < 4) {
		for (i=0; i<length; i++) {
			buffer[i] = buffer[4-length+i];
		}
	}

	return length;
}



//////////////////////////////
//
// MidiFile::addNoteOn -- Add a note-on message to the given track at the
//    given time in the given channel.
//

MidiEvent* MidiFile::addNoteOn(int aTrack, int aTick, int aChannel, int key, int vel) {
	MidiEvent* me = new MidiEvent;
	me->makeNoteOn(aChannel, key, vel);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addNoteOff -- Add a note-off message (using 0x80 messages).
//

MidiEvent* MidiFile::addNoteOff(int aTrack, int aTick, int aChannel, int key,
		int vel) {
	MidiEvent* me = new MidiEvent;
	me->makeNoteOff(aChannel, key, vel);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addNoteOff -- Add a note-off message (using 0x90 messages with
//   zero attack velocity).
//

MidiEvent* MidiFile::addNoteOff(int aTrack, int aTick, int aChannel, int key) {
	MidiEvent* me = new MidiEvent;
	me->makeNoteOff(aChannel, key);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addController -- Add a controller message in the given
//    track at the given tick time in the given channel.
//

MidiEvent* MidiFile::addController(int aTrack, int aTick, int aChannel,
		int num, int value) {
	MidiEvent* me = new MidiEvent;
	me->makeController(aChannel, num, value);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addPatchChange -- Add a patch-change message in the given
//    track at the given tick time in the given channel.
//

MidiEvent* MidiFile::addPatchChange(int aTrack, int aTick, int aChannel,
		int patchnum) {
	MidiEvent* me = new MidiEvent;
	me->makePatchChange(aChannel, patchnum);
	me->tick = aTick;
	m_events[aTrack]->push_back_no_copy(me);
	return me;
}



//////////////////////////////
//
// MidiFile::addTimbre -- Add a patch-change message in the given
//    track at the given tick time in the given channel.  Alias for
//    MidiFile::addPatchChange().
//

MidiEvent* MidiFile::addTimbre(int aTrack, int aTick, int aChannel, int patchnum) {
	return addPatchChange(aTrack, aTick, aChannel, patchnum);
}



//////////////////////////////
//
// MidiFile::addPitchBend -- convert  number in the range from -1 to +1
//     into two 7-bit numbers (smallest piece first)
//
//   -1.0 maps to 0 (0x0000)
//    0.0 maps to 8192 (0x2000 --> 0x40 0x00)
//   +1.0 maps to 16383 (0x3FFF --> 0x7F 0x7F)
//

MidiEvent* MidiFile::addPitchBend(int aTrack, int aTick, int aChannel, double amount) {
	m_timemapvalid = 0;
	amount += 1.0;
	int value = int(amount * 8192 + 0.5);

	// prevent any wrap-around in case of round-off errors
	if (value > 0x3fff) {
		value = 0x3fff;
	}
	if (value < 0) {
		value = 0;
	}

	int lsbint = 0x7f & value;
	int msbint = 0x7f & (value  >> 7);

	std::vector<uchar> mididata;
	mididata.resize(3);
	if (aChannel < 0) {
		aChannel = 0;
	} else if (aChannel > 15) {
		aChannel = 15;
	}
	mididata[0] = uchar(0xe0 | aChannel);
	mididata[1] = uchar(lsbint);
	mididata[2] = uchar(msbint);

	return addEvent(aTrack, aTick, mididata);
}


///////////////////////////////////////////////////////////////////////////
//
// Controller message adding convenience functions:
//

//////////////////////////////
//
// MidiFile::addSustain -- Add a continuous controller message for the sustain pedal.
//

MidiEvent* MidiFile::addSustain(int aTrack, int aTick, int aChannel, int value) {
	return addController(aTrack, aTick, aChannel, 64, value);
}

//
// MidiFile::addSustainPedal -- Alias for MidiFile::addSustain().
//

MidiEvent* MidiFile::addSustainPedal(int aTrack, int aTick, int aChannel, int value) {
	return addSustain(aTrack, aTick, aChannel, value);
}



//////////////////////////////
//
// MidiFile::addSustainOn -- Add a continuous controller message for the sustain pedal on.
//

MidiEvent* MidiFile::addSustainOn(int aTrack, int aTick, int aChannel) {
	return addSustain(aTrack, aTick, aChannel, 127);
}

//
// MidiFile::addSustainPedalOn -- Alias for MidiFile::addSustainOn().
//

MidiEvent* MidiFile::addSustainPedalOn(int aTrack, int aTick, int aChannel) {
	return addSustainOn(aTrack, aTick, aChannel);
}



//////////////////////////////
//
// MidiFile::addSustainOff -- Add a continuous controller message for the sustain pedal off.
//

MidiEvent* MidiFile::addSustainOff(int aTrack, int aTick, int aChannel) {
	return addSustain(aTrack, aTick, aChannel, 0);
}

//
// MidiFile::addSustainPedalOff -- Alias for MidiFile::addSustainOff().
//

MidiEvent* MidiFile::addSustainPedalOff(int aTrack, int aTick, int aChannel) {
	return addSustainOff(aTrack, aTick, aChannel);
}



//////////////////////////////
//
// MidiFile::addTrack -- adds a blank track at end of the
//    track list.  Returns the track number of the added
//    track.
//

int MidiFile::addTrack(void) {
	int length = getNumTracks();
	m_events.resize(length+1);
	m_events[length] = new MidiEventList;
	m_events[length]->reserve(10000);
	m_events[length]->clear();
	return length;
}

int MidiFile::addTrack(int count) {
	int length = getNumTracks();
	m_events.resize(length+count);
	int i;
	for (i=0; i<count; i++) {
		m_events[length + i] = new MidiEventList;
		m_events[length + i]->reserve(10000);
		m_events[length + i]->clear();
	}
	return length + count - 1;
}

//
// MidiFile::addTracks -- Alias for MidiFile::addTrack().
//

int MidiFile::addTracks(int count) {
	return addTrack(count);
}




//////////////////////////////
//
// MidiFile::allocateEvents --
//

void MidiFile::allocateEvents(int track, int aSize) {
	int oldsize = m_events[track]->size();
	if (oldsize < aSize) {
		m_events[track]->reserve(aSize);
	}
}



//////////////////////////////
//
// MidiFile::deleteTrack -- remove a track from the MidiFile.
//   Tracks are numbered starting at track 0.
//

void MidiFile::deleteTrack(int aTrack) {
	int length = getNumTracks();
	if (aTrack < 0 || aTrack >= length) {
		return;
	}
	if (length == 1) {
		return;
	}
	delete m_events[aTrack];
	for (int i=aTrack; i<length-1; i++) {
		m_events[i] = m_events[i+1];
	}

	m_events[length] = NULL;
	m_events.resize(length-1);
}



//////////////////////////////
//
// MidiFile::clear -- make the MIDI file empty with one
//     track with no data in it.
//

void MidiFile::clear(void) {
	int length = getNumTracks();
	for (int i=0; i<length; i++) {
		delete m_events[i];
		m_events[i] = NULL;
	}
	m_events.resize(1);
	m_events[0] = new MidiEventList;
	m_timemapvalid=0;
	m_timemap.clear();
	m_theTrackState = TRACK_STATE_SPLIT;
	m_theTimeState = TIME_STATE_ABSOLUTE;
}


void MidiFile::erase(void) {
	clear();
}



//////////////////////////////
//
// MidiFile::getEvent -- return the event at the given index in the
//    specified track.
//

MidiEvent& MidiFile::getEvent(int aTrack, int anIndex) {
	return (*m_events[aTrack])[anIndex];
}


const MidiEvent& MidiFile::getEvent(int aTrack, int anIndex) const {
	return (*m_events[aTrack])[anIndex];
}



//////////////////////////////
//
// MidiFile::getTicksPerQuarterNote -- returns the number of
//   time units that are supposed to occur during a quarternote.
//

int MidiFile::getTicksPerQuarterNote(void) const {
	if (m_ticksPerQuarterNote == 0xE728) {
		// this is a special case which is the SMPTE time code
		// setting for 25 frames a second with 40 subframes
		// which means one tick per millisecond.  When SMPTE is
		// being used, there is no real concept of the quarter note,
		// so presume 60 bpm as a simiplification here.
		// return 1000;
	}
	return m_ticksPerQuarterNote;
}

//
// Alias for getTicksPerQuarterNote:
//

int MidiFile::getTPQ(void) const {
	return getTicksPerQuarterNote();
}



//////////////////////////////
//
// MidiFile::getEventCount -- returns the number of events
//   in a given track.
//

int MidiFile::getEventCount(int aTrack) const {
	return m_events[aTrack]->size();
}


int MidiFile::getNumEvents(int aTrack) const {
	return m_events[aTrack]->size();
}



//////////////////////////////
//
// MidiFile::mergeTracks -- combine the data from two
//   tracks into one.  Placing the data in the first
//   track location listed, and Moving the other tracks
//   in the file around to fill in the spot where Track2
//   used to be.  The results of this function call cannot
//   be reversed.
//

void MidiFile::mergeTracks(int aTrack1, int aTrack2) {
	MidiEventList* mergedTrack;
	mergedTrack = new MidiEventList;
	int oldTimeState = getTickState();
	if (oldTimeState == TIME_STATE_DELTA) {
		makeAbsoluteTicks();
	}
	int length = getNumTracks();
	for (int i=0; i<(int)m_events[aTrack1]->size(); i++) {
		mergedTrack->push_back((*m_events[aTrack1])[i]);
	}
	for (int j=0; j<(int)m_events[aTrack2]->size(); j++) {
		(*m_events[aTrack2])[j].track = aTrack1;
		mergedTrack->push_back((*m_events[aTrack2])[j]);
	}

	mergedTrack->sort();

	delete m_events[aTrack1];

	m_events[aTrack1] = mergedTrack;

	for (int i=aTrack2; i<length-1; i++) {
		m_events[i] = m_events[i+1];
		for (int j=0; j<(int)m_events[i]->size(); j++) {
			(*m_events[i])[j].track = i;
		}
	}

	m_events[length-1] = NULL;
	m_events.resize(length-1);

	if (oldTimeState == TIME_STATE_DELTA) {
		deltaTicks();
	}
}



//////////////////////////////
//
// MidiFile::setTicksPerQuarterNote --
//

void MidiFile::setTicksPerQuarterNote(int ticks) {
	m_ticksPerQuarterNote = ticks;
}

//
// Alias for setTicksPerQuarterNote:
//

void MidiFile::setTPQ(int ticks) {
	setTicksPerQuarterNote(ticks);
}


//////////////////////////////
//
// MidiFile::setMillisecondTicks -- set the ticks per quarter note
//   value to milliseconds.  The format for this specification is
//   highest 8-bits: SMPTE Frame rate (as a negative 2's compliment value).
//   lowest 8-bits: divisions per frame (as a positive number).
//   for millisecond resolution, the SMPTE value is -25, and the
//   frame rate is 40 frame per division.  In hexadecimal, these
//   values are: -25 = 1110,0111 = 0xE7 and 40 = 0010,1000 = 0x28
//   So setting the ticks per quarter note value to 0xE728 will cause
//   delta times in the MIDI file to represent milliseconds.  Calling
//   this function will not change any exiting timestamps, it will
//   only change the meaning of the timestamps.
//

void MidiFile::setMillisecondTicks(void) {
	m_ticksPerQuarterNote = 0xE728;
}



//////////////////////////////
//
// MidiFile::sortTrack -- Sort the specified track in tick order.
//    If the MidiEvent::seq variables have been filled in with
//    a sequence value, this will preserve the order of the
//    events that occur at the same tick time before the sort
//    was done.
//

void MidiFile::sortTrack(int track) {
	if ((track >= 0) && (track < getTrackCount())) {
		m_events.at(track)->sort();
	} else {
		std::cerr << "Warning: track " << track << " does not exist." << std::endl;
	}
}



//////////////////////////////
//
// MidiFile::sortTracks -- sort all tracks in the MidiFile.
//

void MidiFile::sortTracks(void) {
	if (m_theTimeState == TIME_STATE_ABSOLUTE) {
		for (int i=0; i<getTrackCount(); i++) {
			m_events.at(i)->sort();
		}
	} else {
		std::cerr << "Warning: Sorting only allowed in absolute tick mode.";
	}
}



//////////////////////////////
//
// MidiFile::getTrackCountAsType1 --  Return the number of tracks in the
//    MIDI file.  Returns the size of the events if not in joined state.
//    If in joined state, reads track 0 to find the maximum track
//    value from the original unjoined tracks.
//

int MidiFile::getTrackCountAsType1(void) {
	if (getTrackState() == TRACK_STATE_JOINED) {
		int output = 0;
		int i;
		for (i=0; i<(int)m_events[0]->size(); i++) {
			if (getEvent(0,i).track > output) {
				output = getEvent(0,i).track;
			}
		}
		return output+1;  // I think the track values are 0 offset...
	} else {
		return (int)m_events.size();
	}
}



//////////////////////////////
//
// MidiFile::clearLinks --
//

void MidiFile::clearLinks(void) {
	for (int i=0; i<getTrackCount(); i++) {
		if (m_events[i] == NULL) {
			continue;
		}
		m_events[i]->clearLinks();
	}
	m_linkedEventsQ = false;
}



///////////////////////////////////////////////////////////////////////////
//
// private functions
//

//////////////////////////////
//
// MidiFile::linearTickInterpolationAtSecond -- return the tick value at the
//    given input time.
//

double MidiFile::linearTickInterpolationAtSecond(double seconds) {
	if (m_timemapvalid == 0) {
		buildTimeMap();
		if (m_timemapvalid == 0) {
			return -1.0;    // something went wrong
		}
	}

	int i;
	double lasttime = m_timemap[m_timemap.size()-1].seconds;
	// give an error value of -1 if time is out of range of data.
	if (seconds < 0.0) {
		return -1.0;
	}
	if (seconds > m_timemap[m_timemap.size()-1].seconds) {
		return -1.0;
	}

	// Guess which side of the list is closest to target:
	// Could do a more efficient algorithm since time values are sorted,
	// but good enough for now...
	int startindex = -1;
	if (seconds < lasttime / 2) {
		for (i=0; i<(int)m_timemap.size(); i++) {
			if (m_timemap[i].seconds > seconds) {
				startindex = i-1;
				break;
			} else if (m_timemap[i].seconds == seconds) {
				startindex = i;
				break;
			}
		}
	} else {
		for (i=(int)m_timemap.size()-1; i>0; i--) {
			if (m_timemap[i].seconds < seconds) {
				startindex = i+1;
				break;
			} else if (m_timemap[i].seconds == seconds) {
				startindex = i;
				break;
			}
		}
	}

	if (startindex < 0) {
		return -1.0;
	}
	if (startindex >= (int)m_timemap.size()-1) {
		return -1.0;
	}

	double x1 = m_timemap[startindex].seconds;
	double x2 = m_timemap[startindex+1].seconds;
	double y1 = m_timemap[startindex].tick;
	double y2 = m_timemap[startindex+1].tick;
	double xi = seconds;

	return (xi-x1) * ((y2-y1)/(x2-x1)) + y1;
}



//////////////////////////////
//
// MidiFile::linearSecondInterpolationAtTick -- return the time in seconds
//    value at the given input tick time. (Ticks input could be made double).
//

double MidiFile::linearSecondInterpolationAtTick(int ticktime) {
	if (m_timemapvalid == 0) {
		buildTimeMap();
		if (m_timemapvalid == 0) {
			return -1.0;    // something went wrong
		}
	}

	int i;
	double lasttick = m_timemap[m_timemap.size()-1].tick;
	// give an error value of -1 if time is out of range of data.
	if (ticktime < 0.0) {
		return -1;
	}
	if (ticktime > m_timemap.back().tick) {
		return -1;  // don't try to extrapolate
	}

	// Guess which side of the list is closest to target:
	// Could do a more efficient algorithm since time values are sorted,
	// but good enough for now...
	int startindex = -1;
	if (ticktime < lasttick / 2) {
		for (i=0; i<(int)m_timemap.size(); i++) {
			if (m_timemap[i].tick > ticktime) {
				startindex = i-1;
				break;
			} else if (m_timemap[i].tick == ticktime) {
				startindex = i;
				break;
			}
		}
	} else {
		for (i=(int)m_timemap.size()-1; i>0; i--) {
			if (m_timemap[i].tick < ticktime) {
				startindex = i;
				break;
			} else if (m_timemap[i].tick == ticktime) {
				startindex = i;
				break;
			}
		}
	}

	if (startindex < 0) {
		return -1;
	}
	if (startindex >= (int)m_timemap.size()-1) {
		return -1;
	}

	if (m_timemap[startindex].tick == ticktime) {
		return m_timemap[startindex].seconds;
	}

	double x1 = m_timemap[startindex].tick;
	double x2 = m_timemap[startindex+1].tick;
	double y1 = m_timemap[startindex].seconds;
	double y2 = m_timemap[startindex+1].seconds;
	double xi = ticktime;

	return (xi-x1) * ((y2-y1)/(x2-x1)) + y1;
}



//////////////////////////////
//
// MidiFile::buildTimeMap -- build an index of the absolute tick values
//      found in a MIDI file, and their corresponding time values in
//      seconds, taking into consideration tempo change messages.  If no
//      tempo messages are given (or untill they are given, then the
//      tempo is set to 120 beats per minute).  If SMPTE time code is
//      used, then ticks are actually time values.  So don't build
//      a time map for SMPTE ticks, and just calculate the time in
//      seconds from the tick value (1000 ticks per second SMPTE
//      is the only mode tested (25 frames per second and 40 subframes
//      per frame).
//

void MidiFile::buildTimeMap(void) {

	// convert the MIDI file to absolute time representation
	// in single track mode (and undo if the MIDI file was not
	// in that state when this function was called.
	//
	int trackstate = getTrackState();
	int timestate  = getTickState();

	makeAbsoluteTicks();
	joinTracks();

	int allocsize = getNumEvents(0);
	m_timemap.reserve(allocsize+10);
	m_timemap.clear();

	_TickTime value;

	int lasttick = 0;
	int tickinit = 0;

	int i;
	int tpq = getTicksPerQuarterNote();
	double defaultTempo = 120.0;
	double secondsPerTick = 60.0 / (defaultTempo * tpq);

	double lastsec = 0.0;
	double cursec = 0.0;

	for (i=0; i<getNumEvents(0); i++) {
		int curtick = getEvent(0, i).tick;
		getEvent(0, i).seconds = cursec;
		if ((curtick > lasttick) || !tickinit) {
			tickinit = 1;

			// calculate the current time in seconds:
			cursec = lastsec + (curtick - lasttick) * secondsPerTick;
			getEvent(0, i).seconds = cursec;

			// store the new tick to second mapping
			value.tick = curtick;
			value.seconds = cursec;
			m_timemap.push_back(value);
			lasttick   = curtick;
			lastsec    = cursec;
		}

		// update the tempo if needed:
		if (getEvent(0,i).isTempo()) {
			secondsPerTick = getEvent(0,i).getTempoSPT(getTicksPerQuarterNote());
		}
	}

	// reset the states of the tracks or time values if necessary here:
	if (timestate == TIME_STATE_DELTA) {
		deltaTicks();
	}
	if (trackstate == TRACK_STATE_SPLIT) {
		splitTracks();
	}

	m_timemapvalid = 1;

}



//////////////////////////////
//
// MidiFile::extractMidiData -- Extract MIDI data from input
//    stream.  Return value is 0 if failure; otherwise, returns 1.
//

int MidiFile::extractMidiData(std::istream& input, std::vector<uchar>& array,
	uchar& runningCommand) {

	int character;
	uchar byte;
	array.clear();
	int runningQ;

	character = input.get();
	if (character == EOF) {
		std::cerr << "Error: unexpected end of file." << std::endl;
		return 0;
	} else {
		byte = (uchar)character;
	}

	if (byte < 0x80) {
		runningQ = 1;
		if (runningCommand == 0) {
			std::cerr << "Error: running command with no previous command" << std::endl;
			return 0;
		}
		if (runningCommand >= 0xf0) {
			std::cerr << "Error: running status not permitted with meta and sysex"
			     << " event." << std::endl;
			std::cerr << "Byte is 0x" << std::hex << (int)byte << std::dec << std::endl;
			return 0;
		}
	} else {
		runningCommand = byte;
		runningQ = 0;
	}

	array.push_back(runningCommand);
	if (runningQ) {
		array.push_back(byte);
	}

	switch (runningCommand & 0xf0) {
		case 0x80:        // note off (2 more bytes)
		case 0x90:        // note on (2 more bytes)
		case 0xA0:        // aftertouch (2 more bytes)
		case 0xB0:        // cont. controller (2 more bytes)
		case 0xE0:        // pitch wheel (2 more bytes)
			byte = readByte(input);
			if (!status()) { return m_rwstatus; }
			if (byte > 0x7f) {
				std::cerr << "MIDI data byte too large: " << (int)byte << std::endl;
				m_rwstatus = false; return m_rwstatus;
			}
			array.push_back(byte);
			if (!runningQ) {
				byte = readByte(input);
				if (!status()) { return m_rwstatus; }
				if (byte > 0x7f) {
					std::cerr << "MIDI data byte too large: " << (int)byte << std::endl;
					m_rwstatus = false; return m_rwstatus;
				}
				array.push_back(byte);
			}
			break;
		case 0xC0:        // patch change (1 more byte)
		case 0xD0:        // channel pressure (1 more byte)
			if (!runningQ) {
				byte = readByte(input);
				if (!status()) { return m_rwstatus; }
				if (byte > 0x7f) {
					std::cerr << "MIDI data byte too large: " << (int)byte << std::endl;
					m_rwstatus = false; return m_rwstatus;
				}
				array.push_back(byte);
			}
			break;
		case 0xF0:
			switch (runningCommand) {
				case 0xff:                 // meta event
					{
					if (!runningQ) {
						byte = readByte(input); // meta type
						if (!status()) { return m_rwstatus; }
						array.push_back(byte);
					}
					ulong length = 0;
					uchar byte1 = 0;
					uchar byte2 = 0;
					uchar byte3 = 0;
					uchar byte4 = 0;
					byte1 = readByte(input);
					if (!status()) { return m_rwstatus; }
					array.push_back(byte1);
					if (byte1 >= 0x80) {
						byte2 = readByte(input);
						if (!status()) { return m_rwstatus; }
						array.push_back(byte2);
						if (byte2 > 0x80) {
							byte3 = readByte(input);
							if (!status()) { return m_rwstatus; }
							array.push_back(byte3);
							if (byte3 >= 0x80) {
								byte4 = readByte(input);
								if (!status()) { return m_rwstatus; }
								array.push_back(byte4);
								if (byte4 >= 0x80) {
									std::cerr << "Error: cannot handle large VLVs" << std::endl;
									m_rwstatus = false; return m_rwstatus;
								} else {
									length = unpackVLV(byte1, byte2, byte3, byte4);
									if (!m_rwstatus) { return m_rwstatus; }
								}
							} else {
								length = unpackVLV(byte1, byte2, byte3);
								if (!m_rwstatus) { return m_rwstatus; }
							}
						} else {
							length = unpackVLV(byte1, byte2);
							if (!m_rwstatus) { return m_rwstatus; }
						}
					} else {
						length = byte1;
					}
					for (int j=0; j<(int)length; j++) {
						byte = readByte(input); // meta type
						if (!status()) { return m_rwstatus; }
						array.push_back(byte);
					}
					}
					break;

				// The 0xf0 and 0xf7 meta commands deal with system-exclusive
				// messages. 0xf0 is used to either start a message or to store
				// a complete message.  The 0xf0 is part of the outgoing MIDI
				// bytes.  The 0xf7 message is used to send arbitrary bytes,
				// typically the middle or ends of system exclusive messages.  The
				// 0xf7 byte at the start of the message is not part of the
				// outgoing raw MIDI bytes, but is kept in the MidiFile message
				// to indicate a raw MIDI byte message (typically a partial
				// system exclusive message).
				case 0xf7:   // Raw bytes. 0xf7 is not part of the raw
				             // bytes, but are included to indicate
				             // that this is a raw byte message.
				case 0xf0:   // System Exclusive message
					{         // (complete, or start of message).
					int length = (int)readVLValue(input);
					for (int i=0; i<length; i++) {
						byte = readByte(input);
						if (!status()) { return m_rwstatus; }
						array.push_back(byte);
					}
					}
					break;

				// other "F" MIDI commands are not expected, but can be
				// handled here if they exist.
			}
			break;
		default:
			std::cout << "Error reading midifile" << std::endl;
			std::cout << "Command byte was " << (int)runningCommand << std::endl;
			return 0;
	}
	return 1;
}



//////////////////////////////
//
// MidiFile::readVLValue -- The VLV value is expected to be unpacked into
//   a 4-byte integer no greater than 0x0fffFFFF, so a VLV value up to
//   4-bytes in size (FF FF FF 7F) will only be considered.  Longer
//   VLV values are not allowed in standard MIDI files, so the extract
//   delta time would be truncated and the extra byte(s) will be parsed
//   incorrectly as a MIDI command.
//

ulong MidiFile::readVLValue(std::istream& input) {
	uchar b[5] = {0};

	for (int i=0; i<5; i++) {
		b[i] = readByte(input);
		if (!status()) { return m_rwstatus; }
		if (b[i] < 0x80) {
			break;
		}
	}

	return unpackVLV(b[0], b[1], b[2], b[3], b[4]);
}



//////////////////////////////
//
// MidiFile::unpackVLV -- converts a VLV value to an unsigned long value.
//     The bytes a, b, c, d, e are in big-endian order (the order they would
//     be read out of the MIDI file).
// default values: a = b = c = d = 0;
//

ulong MidiFile::unpackVLV(uchar a, uchar b, uchar c, uchar d, uchar e) {
	uchar bytes[5] = {a, b, c, d, e};
	int count = 0;
	while ((count < 5) && (bytes[count] > 0x7f)) {
		count++;
	}
	count++;
	if (count >= 6) {
		std::cerr << "VLV number is too large" << std::endl;
		m_rwstatus = false;
		return 0;
	}

	ulong output = 0;
	for (int i=0; i<count; i++) {
		output = output << 7;
		output = output | (bytes[i] & 0x7f);
	}

	return output;
}



//////////////////////////////
//
// MidiFile::writeVLValue -- write a number to the midifile
//    as a variable length value which segments a file into 7-bit
//    values and adds a contination bit to each.  Maximum size of input
//    aValue is 0x0FFFffff.
//

void MidiFile::writeVLValue(long aValue, std::vector<uchar>& outdata) {
	uchar bytes[4] = {0};

	if ((unsigned long)aValue >= (1 << 28)) {
		std::cerr << "Error: number too large to convert to VLV" << std::endl;
		aValue = 0x0FFFffff;
	}

	bytes[0] = (uchar)(((ulong)aValue >> 21) & 0x7f);  // most significant 7 bits
	bytes[1] = (uchar)(((ulong)aValue >> 14) & 0x7f);
	bytes[2] = (uchar)(((ulong)aValue >> 7)  & 0x7f);
	bytes[3] = (uchar)(((ulong)aValue)       & 0x7f);  // least significant 7 bits

	int start = 0;
	while ((start<4) && (bytes[start] == 0))  start++;

	for (int i=start; i<3; i++) {
		bytes[i] = bytes[i] | 0x80;
		outdata.push_back(bytes[i]);
	}
	outdata.push_back(bytes[3]);
}



//////////////////////////////
//
// MidiFile::clear_no_deallocate -- Similar to clear() but does not
//   delete the Events in the lists.  This is primarily used internally
//   to the MidiFile class, so don't use unless you really know what you
//   are doing (otherwise you will end up with memory leaks or
//   segmentation faults).
//

void MidiFile::clear_no_deallocate(void) {
	for (int i=0; i<getTrackCount(); i++) {
		m_events[i]->detach();
		delete m_events[i];
		m_events[i] = NULL;
	}
	m_events.resize(1);
	m_events[0] = new MidiEventList;
	m_timemapvalid=0;
	m_timemap.clear();
	// m_events.resize(0);   // causes a memory leak [20150205 Jorden Thatcher]
}



//////////////////////////////
//
// MidiFile::ticksearch -- for finding a tick entry in the time map.
//

int MidiFile::ticksearch(const void* A, const void* B) {
	_TickTime& a = *((_TickTime*)A);
	_TickTime& b = *((_TickTime*)B);

	if (a.tick < b.tick) {
		return -1;
	} else if (a.tick > b.tick) {
		return 1;
	}
	return 0;
}



//////////////////////////////
//
// MidiFile::secondsearch -- for finding a second entry in the time map.
//

int MidiFile::secondsearch(const void* A, const void* B) {
	_TickTime& a = *((_TickTime*)A);
	_TickTime& b = *((_TickTime*)B);

	if (a.seconds < b.seconds) {
		return -1;
	} else if (a.seconds > b.seconds) {
		return 1;
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////
//
// Static functions:
//


//////////////////////////////
//
// MidiFile::readLittleEndian4Bytes -- Read four bytes which are in
//      little-endian order (smallest byte is first).  Then flip
//      the order of the bytes to create the return value.
//

ulong MidiFile::readLittleEndian4Bytes(std::istream& input) {
	uchar buffer[4] = {0};
	input.read((char*)buffer, 4);
	if (input.eof()) {
		std::cerr << "Error: unexpected end of file." << std::endl;
		return 0;
	}
	return buffer[3] | (buffer[2] << 8) | (buffer[1] << 16) | (buffer[0] << 24);
}



//////////////////////////////
//
// MidiFile::readLittleEndian2Bytes -- Read two bytes which are in
//       little-endian order (smallest byte is first).  Then flip
//       the order of the bytes to create the return value.
//

ushort MidiFile::readLittleEndian2Bytes(std::istream& input) {
	uchar buffer[2] = {0};
	input.read((char*)buffer, 2);
	if (input.eof()) {
		std::cerr << "Error: unexpected end of file." << std::endl;
		return 0;
	}
	return buffer[1] | (buffer[0] << 8);
}



//////////////////////////////
//
// MidiFile::readByte -- Read one byte from input stream.  Set
//     fail status error if there was a problem (calling function
//     has to check this status for an error after reading).
//

uchar MidiFile::readByte(std::istream& input) {
	uchar buffer[1] = {0};
	input.read((char*)buffer, 1);
	if (input.eof()) {
		std::cerr << "Error: unexpected end of file." << std::endl;
		m_rwstatus = false;
		return 0;
	}
	return buffer[0];
}



//////////////////////////////
//
// MidiFile::writeLittleEndianUShort --
//

std::ostream& MidiFile::writeLittleEndianUShort(std::ostream& out, ushort value) {
	union { char bytes[2]; ushort us; } data;
	data.us = value;
	out << data.bytes[0];
	out << data.bytes[1];
	return out;
}



//////////////////////////////
//
// MidiFile::writeBigEndianUShort --
//

std::ostream& MidiFile::writeBigEndianUShort(std::ostream& out, ushort value) {
	union { char bytes[2]; ushort us; } data;
	data.us = value;
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// MidiFile::writeLittleEndianShort --
//

std::ostream& MidiFile::writeLittleEndianShort(std::ostream& out, short value) {
	union { char bytes[2]; short s; } data;
	data.s = value;
	out << data.bytes[0];
	out << data.bytes[1];
	return out;
}



//////////////////////////////
//
// MidiFile::writeBigEndianShort --
//

std::ostream& MidiFile::writeBigEndianShort(std::ostream& out, short value) {
	union { char bytes[2]; short s; } data;
	data.s = value;
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// MidiFile::writeLittleEndianULong --
//

std::ostream& MidiFile::writeLittleEndianULong(std::ostream& out, ulong value) {
	union { char bytes[4]; ulong ul; } data;
	data.ul = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	return out;
}



//////////////////////////////
//
// MidiFile::writeBigEndianULong --
//

std::ostream& MidiFile::writeBigEndianULong(std::ostream& out, ulong value) {
	union { char bytes[4]; long ul; } data;
	data.ul = value;
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// MidiFile::writeLittleEndianLong --
//

std::ostream& MidiFile::writeLittleEndianLong(std::ostream& out, long value) {
	union { char bytes[4]; long l; } data;
	data.l = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	return out;
}



//////////////////////////////
//
// MidiFile::writeBigEndianLong --
//

std::ostream& MidiFile::writeBigEndianLong(std::ostream& out, long value) {
	union { char bytes[4]; long l; } data;
	data.l = value;
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;

}



//////////////////////////////
//
// MidiFile::writeBigEndianFloat --
//

std::ostream& MidiFile::writeBigEndianFloat(std::ostream& out, float value) {
	union { char bytes[4]; float f; } data;
	data.f = value;
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// MidiFile::writeLittleEndianFloat --
//

std::ostream& MidiFile::writeLittleEndianFloat(std::ostream& out, float value) {
	union { char bytes[4]; float f; } data;
	data.f = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	return out;
}



//////////////////////////////
//
// MidiFile::writeBigEndianDouble --
//

std::ostream& MidiFile::writeBigEndianDouble(std::ostream& out, double value) {
	union { char bytes[8]; double d; } data;
	data.d = value;
	out << data.bytes[7];
	out << data.bytes[6];
	out << data.bytes[5];
	out << data.bytes[4];
	out << data.bytes[3];
	out << data.bytes[2];
	out << data.bytes[1];
	out << data.bytes[0];
	return out;
}



//////////////////////////////
//
// MidiFile::writeLittleEndianDouble --
//

std::ostream& MidiFile::writeLittleEndianDouble(std::ostream& out, double value) {
	union { char bytes[8]; double d; } data;
	data.d = value;
	out << data.bytes[0];
	out << data.bytes[1];
	out << data.bytes[2];
	out << data.bytes[3];
	out << data.bytes[4];
	out << data.bytes[5];
	out << data.bytes[6];
	out << data.bytes[7];
	return out;
}


} // end namespace smf

///////////////////////////////////////////////////////////////////////////
//
// external functions
//

//////////////////////////////
//
// operator<< -- for printing an ASCII version of the MIDI file
//

std::ostream& operator<<(std::ostream& out, smf::MidiFile& aMidiFile) {
	aMidiFile.writeBinascWithComments(out);
	return out;
}
