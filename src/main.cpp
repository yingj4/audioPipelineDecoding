#include <audio.h>
#include <iostream>
#include <chrono>
#include <fstream>

int main(int argc, char const *argv[])
{
    using namespace ILLIXR_AUDIO;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <number of size 1024 blocks to process> ";
        std::cout << "<optional: encode/decode>\n";
        std::cout << "Note: If you want to hear the output sound, limit the process sample blocks so that the output is not longer than input!\n";
        return 1;
    }

    const int numBlocks = atoi(argv[1]);
    ABAudio::ProcessType procType(ABAudio::ProcessType::FULL);
    if (argc > 2){
        if (!strcmp(argv[2], "encode"))
            procType = ABAudio::ProcessType::ENCODE;
        else
            procType = ABAudio::ProcessType::DECODE;
    }


    ABAudio::ProcessType procencode(ABAudio::ProcessType::ENCODE);
    ABAudio::ProcessType procdecode(ABAudio::ProcessType::DECODE);
    ABAudio encoder("", procencode);
    ABAudio decoder("", procdecode);
    //audio.loadSource();
    std::ofstream outputFile;
    outputFile.open("audio_timing_solo.txt");
    std::chrono::time_point<std::chrono::system_clock> blockStart, blockFinish;
    for (int i = 0; i < 7200; ++i){
        blockStart = std::chrono::high_resolution_clock::now();
        encoder.processBlock();
        decoder.processBlock();
        blockFinish = std::chrono::high_resolution_clock::now();
        outputFile << std::chrono::duration_cast<std::chrono::milliseconds>(blockFinish - blockStart).count() << std::endl;
    }
    return 0;
}
