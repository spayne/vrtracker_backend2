// rangesplay.cpp : Defines the entry point for the console application.
//


#include "vr_tracker.h"
#include "update_history_visitor.h"
#include "vr_system_wrapper.h"
#include "traverse_graph.h"
#include "openvr_broker.h"
#include "update_history_visitor.h"
#include "tbb/tick_count.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/task_group.h"
#include <iostream>

using namespace vr_result;

template <typename visitor_fn>
static void traverse_history_graph_threaded(visitor_fn &visitor,
	vr_tracker<slab_allocator<char>> *outer_state,
	openvr_broker::open_vr_interfaces &interfaces) 
{
	SystemWrapper			system_wrapper(interfaces.sysi);
	ApplicationsWrapper		application_wrapper(interfaces.appi);
	SettingsWrapper			settings_wrapper(interfaces.seti);
	ChaperoneWrapper		chaperone_wrapper(interfaces.chapi);
	ChaperoneSetupWrapper	chaperone_setup_wrapper(interfaces.chapsi);
	CompositorWrapper		compositor_wrapper(interfaces.compi);
	OverlayWrapper			overlay_wrapper(interfaces.ovi);
	RenderModelWrapper		rendermodel_wrapper(interfaces.remi);
	ExtendedDisplayWrapper	extended_display_wrapper(interfaces.exdi);
	TrackedCameraWrapper	tracked_camera_wrapper(interfaces.taci);
	ResourcesWrapper		resources_wrapper(interfaces.resi);

	std::vector<std::thread*> threads;
	vr_state *s = &outer_state->m_state;
	vr_keys *keys = &outer_state->keys;
	tbb::task_group g;

	g.run([&] {
		visit_system_node(visitor, &s->system_node, interfaces.sysi, system_wrapper, rendermodel_wrapper, keys, g);
	});

	g.run([&] {
		visit_applications_node(visitor, &s->applications_node, application_wrapper, keys, g);
	});

	g.run([&] {
		visit_settings_node(visitor, &s->settings_node, settings_wrapper, keys, g);
	});

	g.run([&] {
		visit_chaperone_node(visitor, &s->chaperone_node, chaperone_wrapper, keys);
	});

	g.run([&] {
		visit_chaperone_setup_node(visitor, &s->chaperone_setup_node, chaperone_setup_wrapper);
	});

	g.run([&] {
		visit_compositor_state(visitor, &s->compositor_node, compositor_wrapper, keys, g);
	});
	
	g.run([&] {
		visit_overlay_state(visitor, &s->overlay_node, overlay_wrapper, keys);
	});
	g.run([&] {
		visit_rendermodel_state(visitor, &s->rendermodels_node, rendermodel_wrapper);
	});
	g.run([&] {
		visit_extended_display_state(visitor, &s->extendeddisplay_node, extended_display_wrapper);
	});
	g.run([&] {
		visit_trackedcamera_state(visitor, &s->trackedcamera_node, tracked_camera_wrapper, keys);
	});
	g.run([&] {
		visit_resources_state(visitor, &s->resources_node, resources_wrapper, keys);
	});

	g.wait();
}

#include <intrin.h>
uint64_t rdtsc() {
	return __rdtsc();
}


class ExecuteImmediatelyTaskGroup
{
public:
	template <typename T>
	void run(T t)
	{
		t();
	}
};

template <typename visitor_fn>
static void traverse_history_graph_sequential(visitor_fn &visitor, 
	vr_tracker<slab_allocator<char>> *outer_state,
	openvr_broker::open_vr_interfaces &interfaces)
{
	SystemWrapper			system_wrapper(interfaces.sysi);
	ApplicationsWrapper		application_wrapper(interfaces.appi);
	SettingsWrapper			settings_wrapper(interfaces.seti);
	ChaperoneWrapper		chaperone_wrapper(interfaces.chapi);
	ChaperoneSetupWrapper	chaperone_setup_wrapper(interfaces.chapsi);
	CompositorWrapper		compositor_wrapper(interfaces.compi);
	OverlayWrapper			overlay_wrapper(interfaces.ovi);
	RenderModelWrapper		rendermodel_wrapper(interfaces.remi);
	ExtendedDisplayWrapper	extended_display_wrapper(interfaces.exdi);
	TrackedCameraWrapper	tracked_camera_wrapper(interfaces.taci);
	ResourcesWrapper		resources_wrapper(interfaces.resi);

	vr_state *s = &outer_state->m_state;
	vr_keys *keys = &outer_state->keys;
	ExecuteImmediatelyTaskGroup dummy;

	uint64_t clock_start = rdtsc();
	tbb::tick_count t0 = tbb::tick_count::now();
	visit_system_node(visitor, &s->system_node, interfaces.sysi, system_wrapper, rendermodel_wrapper, keys, dummy);

	tbb::tick_count t1 = tbb::tick_count::now();
	visit_applications_node(visitor, &s->applications_node, application_wrapper, 
		keys, dummy);

	tbb::tick_count t2 = tbb::tick_count::now();
	visit_settings_node(visitor, &s->settings_node, settings_wrapper, 
			keys, dummy);

	tbb::tick_count t3 = tbb::tick_count::now();
	visit_chaperone_node(visitor, &s->chaperone_node, chaperone_wrapper, keys);
	tbb::tick_count t4 = tbb::tick_count::now();
	visit_chaperone_setup_node(visitor, &s->chaperone_setup_node, chaperone_setup_wrapper);
	tbb::tick_count t5 = tbb::tick_count::now();
	
	visit_compositor_state(visitor, &s->compositor_node, compositor_wrapper, keys, dummy);

	tbb::tick_count t6 = tbb::tick_count::now();
	visit_overlay_state(visitor, &s->overlay_node, overlay_wrapper, keys);
	tbb::tick_count t7 = tbb::tick_count::now();
	visit_rendermodel_state(visitor, &s->rendermodels_node, rendermodel_wrapper);
	tbb::tick_count t8 = tbb::tick_count::now();
	visit_extended_display_state(visitor, &s->extendeddisplay_node, extended_display_wrapper);
	tbb::tick_count t9 = tbb::tick_count::now();
	visit_trackedcamera_state(visitor, &s->trackedcamera_node, tracked_camera_wrapper,
			keys);
	tbb::tick_count t10 = tbb::tick_count::now();
	visit_resources_state(visitor, &s->resources_node, resources_wrapper, keys);
	tbb::tick_count t11 = tbb::tick_count::now();
	uint64_t clock_end = rdtsc();
	std::cout << "sequential visitor cycles: "
		<< clock_end - clock_start
		<< "\n";
}

struct read_only_visitor
{
	constexpr read_only_visitor()
	{}

	constexpr bool visit_source_interfaces() const { return false; }
	constexpr bool expand_structure() const { return false; }
	constexpr bool reload_render_models() const { return false; }
	constexpr bool recheck_distortion() const { return false; }

	inline void start_group_node(const base::URL &url_name, int group_id_index)
	{}

	inline void end_group_node(const base::URL &group_id_name, int group_id_index)
	{}

	// this should give the history half and the openvr half
	// because if openvr resizes something, then the history visitor should have a chance to resize
	// as well
	template <typename T>
	inline void start_vector(const base::URL &vector_name, T &vec)
	{}

	template <typename T>
	inline void end_vector(const base::URL &vector_name, T &vec)
	{}

	template <typename HistoryVectorType, typename ResultType>
	void visit_node(HistoryVectorType &history, ResultType &latest_result)
	{
	}

	template <typename HistoryVectorType>
	void visit_node(HistoryVectorType &history_node)
	{
	}
};


void UPDATE_USE_CASE()
{
	using namespace vr;
	using namespace vr_result;
	using std::range;

	// 
	// Initialize base
	//
	slab s(1024 * 1024 * 32);	// todo - put textures onto their own slab (2048*2048*4 = 16MB)
	slab_allocator<char>::m_temp_slab = &s;
	tmp_vector_pool<VRTMPSize> tmp_pool;
	vr_tmp_vector_base::m_global_pool = &tmp_pool;

	//
	// Construct a tracker
	//
	slab_allocator<char> allocator;
	vr_tracker<slab_allocator<char>> tracker(&s);

	//
	// Baseline config
	// 
	TrackerConfig c;
	c.set_default();
	tracker.keys.Init(c);

	// 
	// Sequential visit 
	//
	update_history_visitor update_visitor(1);
	openvr_broker::open_vr_interfaces interfaces;
	char *error;
	openvr_broker::acquire_interfaces("raw", &interfaces, &error);
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		traverse_history_graph_sequential(update_visitor, &tracker, interfaces);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "first sequential visit took "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
			<< "ms.\n";
	}
	
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		traverse_history_graph_sequential(update_visitor, &tracker, interfaces);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "second sequential visit took "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
			<< "ms.\n";
	}

	//int n = tbb::task_scheduler_init::default_num_threads();
	//tbb::task_scheduler_init init(8);
	

	update_visitor.m_frame_number++;
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		traverse_history_graph_threaded(update_visitor, &tracker, interfaces);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "threaded visit took "
			<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
			<< "us.\n";
	}

	while (1)
	{
		update_visitor.m_frame_number++;
		{
			std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
			traverse_history_graph_threaded(update_visitor, &tracker, interfaces);
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

			std::cout << "threaded visit took "
				<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
				<< "us.\n";
		}
	}

	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		traverse_history_graph_sequential(update_visitor, &tracker, interfaces);
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::cout << "final sequential visit took "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
			<< "ms.\n";
	}



	//
	// test the read only visitor
	//
	int allocs_before = tmp_pool.get_num_alloc_one_calls();
	int frees_before = tmp_pool.get_num_free_one_calls();
	read_only_visitor read_visitor;
	traverse_history_graph_sequential(read_visitor, &tracker, interfaces);

	// see if the read only allocated any temporaries
	assert(allocs_before == tmp_pool.get_num_alloc_one_calls());
	assert(frees_before == tmp_pool.get_num_free_one_calls());

}

