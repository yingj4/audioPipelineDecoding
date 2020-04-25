#include <chrono>
#include <future>
#include <fstream>
#include <thread>

#include "common/component.hh"
#include "common/switchboard.hh"
#include "common/data_format.hh"

#include <audio.h>

using namespace ILLIXR;

#define AUDIO_EPOCH (1024.0/48000.0)

class audio_component : public component
{
public:
	audio_component(std::unique_ptr<reader_latest<pose_type>>&& pose_getter)
		: _m_pose{std::move(pose_getter)}
	{
		ILLIXR_AUDIO::ABAudio::ProcessType processDecode(ILLIXR_AUDIO::ABAudio::ProcessType::DECODE);	
		decoder = new ILLIXR_AUDIO::ABAudio("", processDecode);
		ILLIXR_AUDIO::ABAudio::ProcessType processEncode(ILLIXR_AUDIO::ABAudio::ProcessType::ENCODE);	
		encoder = new ILLIXR_AUDIO::ABAudio("", processEncode);

		decoder->loadSource();
		encoder->loadSource();
		
		outputFile.open("audio_timing.txt");
		readTiming.open("pose_timing.txt");
	}
	~audio_component(){
		delete encoder;
		delete decoder;
		outputFile.close();
		readTiming.close();
	}

	void loop(){
		while (!_m_terminate.load()) {
			blockStart = std::chrono::high_resolution_clock::now();
			if (blockStart < sync) {
				std::this_thread::yield(); // ←_←
				continue;
			}
			// seconds in double
			double timespent = std::chrono::duration<double>(blockStart-sync).count();
			int num_epoch = ceil(timespent/AUDIO_EPOCH);
			sync += std::chrono::microseconds(num_epoch*((int)(AUDIO_EPOCH*1000000))); 

			blockStart = std::chrono::high_resolution_clock::now();
			auto most_recent_pose = _m_pose->get_latest_ro();
			blockPose = std::chrono::high_resolution_clock::now();
			encoder->processBlock();
			decoder->processBlock();
			blockFinish = std::chrono::high_resolution_clock::now();
			//outputFile << "At Sync: " 
			//	<< std::chrono::duration_cast<std::chrono::milliseconds>(sync - start).count() << "		Time spent: "
			outputFile	<< std::chrono::duration_cast<std::chrono::milliseconds>(blockFinish - blockStart).count() << std::endl;
			//readTiming << std::chrono::duration_cast<std::chrono::microseconds>(blockPose - blockStart).count() << std::endl;
			
		}
	}

	void _p_start() override {
		sync = std::chrono::high_resolution_clock::now();
		start = std::chrono::high_resolution_clock::now();
		_m_thread = std::thread{&audio_component::loop, this};
	}

	void _p_stop() override {
		_m_terminate.store(true);
		_m_thread.join();
	}

private:
	std::unique_ptr<reader_latest<pose_type>> _m_pose;
	ILLIXR_AUDIO::ABAudio* decoder, *encoder;
	std::chrono::time_point<std::chrono::system_clock> sync, start, blockStart, blockPose, blockFinish;
	std::ofstream outputFile, readTiming;
	std::thread _m_thread;
	std::atomic<bool> _m_terminate {false};
};

extern "C" component* create_component(switchboard* sb) {
	auto pose_ev = sb->subscribe_latest<pose_type>("fast_pose");
	return new audio_component{std::move(pose_ev)};
}