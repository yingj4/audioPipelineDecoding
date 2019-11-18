#ifndef _AUDIO_H_
#define _AUDIO_H_

#include<sound.h>
#include <vector>
#include <string>

class ABAudio{
private:
	std::vector<Sound*>* soundSrcs;
	std::ofstream* outputFile;
	CAmbisonicBinauralizer* decoder;
	CAmbisonicProcessor* rotator;
	CAmbisonicZoomer* zoomer;
	void generateWAVHeader();
	void readNEncode(CBFormat& sumBF);
	void rotateNZoom(CBFormat& sumBF);
	void writeFile(float** resultSample);
	void updateRotation();
	void updateZoom();
public:
	ABAudio(std::string outputFilePath);
	~ABAudio();
	void processBlock();
	void loadSource();
};

#endif