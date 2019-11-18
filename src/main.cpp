#include <audio.h>

int main(int argc, char const *argv[])
{
	ABAudio audio("3rd_speech_RotNZoom_NoPOP.wav");
	audio.loadSource();
	for (int i = 0; i < 1500; ++i){
		audio.processBlock();
	}
	return 0;
}
