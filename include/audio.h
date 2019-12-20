#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <sound.h>
#include <vector>
#include <string>

namespace ILLIXR_AUDIO{
	class ABAudio{

	public:
		// Process types
		enum class ProcessType {
			FULL, 			// FULL for output wav file
			ENCODE, 		// For profiling, do file reading and encoding without file output
			DECODE			// For profiling, do ambisonics decoding without file output
		};
		ABAudio(std::string outputFilePath, ProcessType processType);
		~ABAudio();
		// Process a block (1024) samples of sound
		void processBlock();
		// Load sound source files (predefined)
		void loadSource();
	private:
		ProcessType processType;
		// a list of sound sources in this audio
		std::vector<Sound*>* soundSrcs;
		// target output file
		std::ofstream* outputFile;
		// decoder associated with this audio
		CAmbisonicBinauralizer* decoder;
		// ambisonics rotator associated with this audio
		CAmbisonicProcessor* rotator;
		// ambisonics zoomer associated with this audio
		CAmbisonicZoomer* zoomer;

		// Generate dummy WAV output file header
		void generateWAVHeader();
		// Read in data from WAV files and encode into ambisonics
		void readNEncode(CBFormat& sumBF);
		// Apply rotation and zoom effects to the ambisonics sound field
		void rotateNZoom(CBFormat& sumBF);
		// Write out a block of samples to the output file
		void writeFile(float** resultSample);

		void updateRotation();
		void updateZoom();
	};
}
#endif
