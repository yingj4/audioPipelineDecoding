#include <audio.h>
#include <common/threadloop.hh>
#include <common/phonebook.hh>
#include <common/switchboard.hh>

using namespace ILLIXR;

class audio_comp : threadloop {
public:
	audio_comp(phonebook* pb)
		: sb{pb->lookup_impl<switchboard>()}
		, pose{sb->subscribe_latest<pose_type>("slow_pose")}
		, now{std::chrono::system_clock::now()}
		, orig_pos{pose->get_latest_ro()->position}
		, orig_rot{pose->get_latest_ro()->orientation}
	{ }

protected:
	virtual void _p_one_iteration() {
		ABAudio::ProcessType procType {ABAudio::ProcessType::FULL};
		const int numBlocks = 1024;
		ABAudio audio {"output.wav", procType};
		audio.loadSource();
		for (int i = 0; i < numBlocks; ++i) {
			auto rot = pose->get_latest_ro()->rotation * orig_rot;
			auto euler_rot = rot.toRotationMatrix().eulerAngles(0, 1, 2);
			audio.updateRotation(Orientation{
				euler_rot[0], euler_rot[1], euler_rot[2]});
			audio.processBlock();
			reliable_sleep_until(now += std::chrono::milliseconds {43});
		}
	}

private:
	switchboard * const sb;
	std::unique_ptr<writer<pose_type>> pose;
	std::chrono::time_point<std::chrono::system_clock> now;
	Eigen::Vector3f orig_pos;
	Eigen::Quaternionf orig_rot;
};

PLUGIN_MAIN(audio_comp)
