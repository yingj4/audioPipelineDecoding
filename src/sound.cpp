#include<sound.h>
#include <algorithm>
//using namespace audioPipe;

Sound::Sound(std::string srcFilename, unsigned nOrder, bool b3D){
	amp = 1.0;
	srcFile = new std::fstream(srcFilename, std::fstream::in);

	// Discard header
	char temp[44];
	srcFile->read((char*)temp, 44);

	BFormat = new CBFormat();
	BEncoder = new CAmbisonicEncoderDist();
	bool ok = BFormat->Configure(nOrder, true, BLOCK_SIZE);
	BFormat->Refresh();
	ok &= BEncoder->Configure(nOrder, true, SAMPLERATE);
	BEncoder->Refresh();
	srcPos.fAzimuth = 0;
	srcPos.fElevation = 0;
	srcPos.fDistance = 0;
	BEncoder->SetPosition(srcPos);
	BEncoder->Refresh();
}

void Sound::setSrcPos(PolarPoint& pos){
	srcPos.fAzimuth = pos.fAzimuth;
	srcPos.fElevation = pos.fElevation;
	srcPos.fDistance = pos.fDistance;
	BEncoder->SetPosition(srcPos);
	BEncoder->Refresh();
}

void Sound::setSrcAmp(float ampScale){
	amp = ampScale;
}

//TODO: Change brutal read from wav file
CBFormat* Sound::readInBFormat(){
	short sampleTemp[BLOCK_SIZE];
	srcFile->read((char*)sampleTemp, BLOCK_SIZE * sizeof(short));
	// normalize samples to -1 to 1 float, with amplitude scale
	for (int i = 0; i < BLOCK_SIZE; ++i){
		sample[i] = amp * (sampleTemp[i] / 32767.0);
		//sample[i] = std::max(std::min(sample[i], (float)+1.0), (float)-1.0);
	}
	BEncoder->Process(sample, BLOCK_SIZE, BFormat);
	return BFormat;
}

Sound::~Sound(){
	srcFile->close();
	delete srcFile;
	delete BFormat;
	delete BEncoder;
}
