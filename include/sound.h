#ifndef _SOUND_H_
#define _SOUND_H_

#include <spatialaudio/Ambisonics.h>
#include <string>
#include <fstream>

#define SAMPLERATE 48000
#define BLOCK_SIZE 1024
#define NORDER 3
#define NUM_CHANNELS (OrderToComponents(NORDER, true))
#define NUM_SRCS 16

namespace ILLIXR_AUDIO{
	class Sound{
	public:
		Sound();
		~Sound();

		Sound(std::string srcFile, unsigned nOrder, bool b3D);
		// set sound src position
		void setSrcPos(PolarPoint& pos);
		// set sound amplitude scale
		void setSrcAmp(float ampScale);
		// read sound samples from mono 16bit WAV file and encode into ambisonics format
		CBFormat* readInBFormat();		
	private:
		// corresponding sound src file
		std::fstream* srcFile;
		// sample buffer HARDCODE
		float sample[BLOCK_SIZE];
		// ambisonics format sound buffer
		CBFormat* BFormat;
		// ambisonics encoder, containing format info, position info, etc.
		CAmbisonicEncoderDist* BEncoder;
		// ambisonics position
		PolarPoint srcPos;
		// amplitude scale to avoid clipping
		float amp;
	};
}
#endif
