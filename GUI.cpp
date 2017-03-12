// GUI usecase

#include "vr_tracker.h"
#include "openvr_string_std.h"


// how the gui will use these data structures

void GUI_USE_CASE()
{
	using namespace openvr_string;
	using namespace vr;
	using namespace vr_result;
	using std::range;

	slab s(1024 * 1024);
	slab_allocator<char> allocator;
	slab_allocator<char>::m_temp_slab = &s;
	vr_tracker<slab_allocator<char>> tracker(&s);

	auto &system = tracker.m_state.system_node;
	auto &controllers = tracker.m_state.system_node.controllers;
	controllers.emplace_back(URL());

	//
	// test starts here
	//

	// I0: case 0 - what if it's not present?
	Result<vr::TrackedDevicePose_t, NoReturnCode> pose;
	if (!controllers.empty() && 
		!controllers[0].raw_tracking_pose.empty())
	{
		pose = controllers[0].raw_tracking_pose();
	}

	// II: ask some meta data questions
	std::string name = controllers[0].raw_tracking_pose.get_name();
	std::string path = controllers[0].raw_tracking_pose.get_path();


	// III: return a time_indexed version
	time_indexed<DevicePose<>> a = controllers[0].raw_tracking_pose.latest();
	time_indexed<DevicePose<>> b = controllers[0].raw_tracking_pose.earliest();


	time_index_t t = a.get_time_index();	// returns an int
	time_stamp_t ts = tracker.get_time_stamp(a.get_time_index()); // returns a time

												  // IV: return a range
	auto pose_range = controllers[0].raw_tracking_pose.get_range();

	auto r2 = controllers[0].raw_tracking_pose.get_range(0, 42);


	time_stamp_t ta = 0;
	time_stamp_t tb = 0;
	auto range_from_times = controllers[0].raw_tracking_pose.get_range(
		tracker.get_closest_time_index(ta), tracker.get_closest_time_index(tb));	// inputs are times

																		// V: copy a range of time_indexed_values into a vector
	std::vector<decltype(pose_range)::value_type> a_copy(pose_range.begin(), pose_range.end());


	// VIII: given a vector of time_indexed values, create a vector that holds the corresponding values
	std::vector<time_stamp_t> vec;
	vec.resize(pose_range.size());
	std::transform(pose_range.begin(), pose_range.end(), vec.begin(),
		[&tracker](auto &c) { return tracker.get_time_stamp(c.get_time_index()); });

	// VII: convert to a string (using return value from I)
	to_string(controllers[0].raw_tracking_pose().val);
}
