#include <sound.h>
#include <algorithm>


ILLIXR_AUDIO::Sound::Sound(std::string srcFilename, unsigned nOrder, bool b3D){
    amp = 1.0;
    srcFile = new std::fstream(srcFilename, std::fstream::in);

    // NOTE: This is currently only accepts mono channel 16-bit depth WAV file
    // TODO: Change brutal read from wav file
    char temp[44];
    srcFile->read((char*)temp, 44);

    // BFormat file initialization
    BFormat = new CBFormat();
    bool ok = BFormat->Configure(nOrder, true, BLOCK_SIZE);
    BFormat->Refresh();

    // Encoder initialization
    BEncoder = new CAmbisonicEncoderDist();
    ok &= BEncoder->Configure(nOrder, true, SAMPLERATE);
    BEncoder->Refresh();
    srcPos.fAzimuth = 0;
    srcPos.fElevation = 0;
    srcPos.fDistance = 0;
    BEncoder->SetPosition(srcPos);
    BEncoder->Refresh();
}

void ILLIXR_AUDIO::Sound::setSrcPos(PolarPoint& pos){
    srcPos.fAzimuth = pos.fAzimuth;
    srcPos.fElevation = pos.fElevation;
    srcPos.fDistance = pos.fDistance;
    BEncoder->SetPosition(srcPos);
    BEncoder->Refresh();
}

void ILLIXR_AUDIO::Sound::setSrcAmp(float ampScale){
    amp = ampScale;
}

//TODO: Change brutal read from wav file
CBFormat* ILLIXR_AUDIO::Sound::readInBFormat(){
    short sampleTemp[BLOCK_SIZE];
    srcFile->read((char*)sampleTemp, BLOCK_SIZE * sizeof(short));
    // normalize samples to -1 to 1 float, with amplitude scale
    for (int i = 0; i < BLOCK_SIZE; ++i){
        sample[i] = amp * (sampleTemp[i] / 32767.0);
    }
    BEncoder->Process(sample, BLOCK_SIZE, BFormat);
    return BFormat;
}

ILLIXR_AUDIO::Sound::~Sound(){
    srcFile->close();
    delete srcFile;
    delete BFormat;
    delete BEncoder;
}