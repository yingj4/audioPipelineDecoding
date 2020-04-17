#include <chrono>
#include <thread>
#include "common/switchboard.hh"
#include "common/component.hh"
#include "common/data_format.hh"
#include "audio.h"

using namespace ILLIXR;
using namespace ILLIXR_AUDIO;

const size_t TIME_SAFETY_FACTOR = 100;

template< class Rep, class Period >
static void t_reliable_sleep(const std::chrono::duration<Rep, Period>& sleep_duration) {
	auto start = std::chrono::high_resolution_clock::now();
	while (std::chrono::high_resolution_clock::now() - start < sleep_duration) {
		std::this_thread::sleep_for(sleep_duration / TIME_SAFETY_FACTOR);
	}
}

class spindle_main : public component {
public:
	spindle_main(switchboard* sb_)
		: sb{sb_}
		, pose{sb->subscribe_latest<pose_type>("slow_pose")}
		, now{std::chrono::system_clock::now()}
		, orig_pos{pose->get_latest_ro()->position}
		, orig_rot{pose->get_latest_ro()->orientation}
	{ }

protected:
	virtual void _p_compute_one_iteration() override {
		ABAudio::ProcessType procType {ABAudio::ProcessType::FULL};
		const int numBlocks = 1024;
		ABAudio audio {"output.wav", procType};
		audio.loadSource();
		for (int i = 0; i < numBlocks; ++i) {
			auto rot = pose->get_latest_ro()->orientation * orig_rot;
			auto euler_rot = rot.toRotationMatrix().eulerAngles(0, 1, 2);
			audio.updateRotation(Orientation{static_cast<float>(euler_rot[0]),
											 static_cast<float>(euler_rot[1]),
											 static_cast<float>(euler_rot[2])});
			audio.processBlock();
			now += std::chrono::milliseconds {21};
			t_reliable_sleep(now - std::chrono::high_resolution_clock::now());
		}
	}

private:
	switchboard * const sb;
	std::unique_ptr<reader_latest<pose_type>> pose;
	std::chrono::time_point<std::chrono::system_clock> now;
	Eigen::Vector3f orig_pos;
	Eigen::Quaternionf orig_rot;
};

extern "C" component* create_component(switchboard* sb) {
	return new spindle_main {sb};
}
