#include <audio.h>
#include <iostream>

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <number of size 1024 blocks to process>\n";
        return 1;
    }

    const int numBlocks = atoi(argv[1]);
#if 1
    ABAudio audio("3rd_speech_RotNZoom_NoPOP.wav");
#else
    ABAudio audio("3rd_paarchana.wav");
#endif
    audio.loadSource();
    for (int i = 0; i < numBlocks; ++i){
        audio.processBlock();
    }
    return 0;
}
