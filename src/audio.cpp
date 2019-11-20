#include <audio.h>

ABAudio::ABAudio(std::string outputFilePath){
	outputFile = new std::ofstream(outputFilePath, std::ios_base::out| std::ios_base::binary);
	soundSrcs = new std::vector<Sound*>;
	generateWAVHeader();

	decoder = new CAmbisonicBinauralizer();
	unsigned temp;
	bool ok = decoder->Configure(NORDER, true, SAMPLERATE, BLOCK_SIZE, temp);
	if (!ok){
		printf("Binauralizer Configuration failed!\n");
	}
	// Processor to rotate
	rotator = new CAmbisonicProcessor();
	rotator->Configure(NORDER, true, BLOCK_SIZE, 0);
	// Processor to zoom
	zoomer = new CAmbisonicZoomer();
	zoomer->Configure(NORDER, true, 0);
}

ABAudio::~ABAudio(){
	free(outputFile);
	for (unsigned int soundIdx = 0; soundIdx < soundSrcs->size(); ++soundIdx){
		free((*soundSrcs)[soundIdx]);
	}
	free(soundSrcs);
	free(decoder);
	free(rotator);
	free(zoomer);
}

void ABAudio::loadSource(){
	// Add a bunch of sources
	Sound* inSound;
	PolarPoint position;

    for (unsigned i = 0; i < NUM_SRCS; i++) {
        //inSound = new Sound("speech00.wav", NORDER, true);
        //position.fAzimuth = -1 * i;
        //position.fElevation = 1 * i;
        //position.fDistance = 2 * i;
        //inSound->setSrcPos(position);
        //soundSrcs->push_back(inSound);

        //inSound = new Sound("speech01.wav", NORDER, true);
        //position.fAzimuth = 1 * i;
        //position.fElevation = 1 * i;
        //position.fDistance = 1 * i;
        //inSound->setSrcPos(position);
        //soundSrcs->push_back(inSound);

#if 1
        inSound = new Sound("speech02.wav", NORDER, true);
        position.fAzimuth = -0.1 * i;
        position.fElevation = 3.14/2 * i;
        position.fDistance = 1 * i;
        inSound->setSrcPos(position);
        soundSrcs->push_back(inSound);
#else
        inSound = new Sound("paarchana.wav", NORDER, true);
        position.fAzimuth = -0.1 * i;
        position.fElevation = 3.14/2 * i;
        position.fDistance = 1 * i;
        inSound->setSrcPos(position);
        soundSrcs->push_back(inSound);
#endif
    }
}

short f2s(float input){
	return (short)(input/1.0 * 32767);
}


void ABAudio::processBlock(){
	float** resultSample = new float*[2];
	resultSample[0] = new float[BLOCK_SIZE];
	resultSample[1] = new float[BLOCK_SIZE];

	// temporary BFormat file to sum up ambisonics
	CBFormat sumBF;
	sumBF.Configure(NORDER, true, BLOCK_SIZE);

	readNEncode(sumBF);
	rotateNZoom(sumBF);
	decoder->Process(&sumBF, resultSample);
	writeFile(resultSample);
	
	delete[] resultSample[0];
	delete[] resultSample[1];
	delete[] resultSample;
}

void ABAudio::readNEncode(CBFormat& sumBF){
	CBFormat* tempBF;
	for (unsigned int soundIdx = 0; soundIdx < soundSrcs->size(); ++soundIdx){
		tempBF = (*soundSrcs)[soundIdx]->readInBFormat();
		if (soundIdx == 0)
			sumBF = *tempBF;
		else
			sumBF += *tempBF;
	}
}


void ABAudio::updateRotation(){
	static int frame = 0;
	frame++;
	Orientation head(0,0,1.0*frame/1500*3.14*2);
	rotator->SetOrientation(head);
	rotator->Refresh();
}

void ABAudio::updateZoom(){
	static int frame = 0;
	frame++;
	zoomer->SetZoom(sinf(frame/100));
	zoomer->Refresh();
}

void ABAudio::rotateNZoom(CBFormat& sumBF){
	updateRotation();
	rotator->Process(&sumBF, BLOCK_SIZE);
	updateZoom();
	zoomer->Process(&sumBF, BLOCK_SIZE);
}

void ABAudio::writeFile(float** resultSample){
	// Normalize, then write into file
	for(int sampleIdx = 0; sampleIdx < BLOCK_SIZE; ++sampleIdx){
		resultSample[0][sampleIdx] = std::max(std::min(resultSample[0][sampleIdx], +1.0f), -1.0f);
		resultSample[1][sampleIdx] = std::max(std::min(resultSample[1][sampleIdx], +1.0f), -1.0f);
		short tempSample0 = f2s(resultSample[0][sampleIdx]);
		short tempSample1 = f2s(resultSample[1][sampleIdx]);
		outputFile->write((char*)&tempSample0,sizeof(short));
		outputFile->write((char*)&tempSample1,sizeof(short));
	}

}

typedef struct __attribute__ ((packed)) WAVHeader_t
{
	unsigned int sGroupID = 0x46464952;
	unsigned int dwFileLength = 48000000;		// A large enough random number
	unsigned int sRiffType = 0x45564157;
	unsigned int subchunkID = 0x20746d66;
	unsigned int subchunksize = 16;
	unsigned short audioFormat = 1;
	unsigned short NumChannels = 2;
	unsigned int SampleRate = 48000;
	unsigned int byteRate = 48000*2*2;
	unsigned short BlockAlign = 2*2;
	unsigned short BitsPerSample = 16;
	unsigned int dataChunkID = 0x61746164;	
	unsigned int dataChunkSize = 48000000;		// A large enough random number
} WAVHeader;

void ABAudio::generateWAVHeader(){
	// insert at head
	WAVHeader wavh;
	outputFile->write((char*)&wavh, sizeof(WAVHeader));
}
