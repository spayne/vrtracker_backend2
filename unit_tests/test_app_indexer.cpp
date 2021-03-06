#include "vr_applications_indexer.h"
#include "capture_test_context.h"
#include <thread>
#include "vr_applications_wrapper.h"
std::atomic<int> app_reader_done;

static void app_reader(ApplicationsIndexer *ai)
{
	int application_sets_read = 0;
	while (application_sets_read < 1000)
	{
		if (ai->get_num_applications() > 0)
			application_sets_read++;

		for (int i = 0; i < ai->get_num_applications(); i++)
		{
			int invalid = ai->get_index_for_key("ha");
			assert(invalid == -1);

			const char *k = ai->get_key_for_index(i);
			int i2 = ai->get_index_for_key(k);
			assert(i == i2);
		}

		ai->read_lock_live_indexes();
		auto &v = ai->get_live_indexes();
		for (auto iter = v.begin(); iter != v.end(); iter++)
		{
			const char *k = ai->get_key_for_index(*iter);
			int i2 = ai->get_index_for_key(k);
			assert(*iter == i2);
		}
		ai->read_unlock_live_indexes();
	}
	app_reader_done = 1;
}

static void app_updater(ApplicationsIndexer *ai, vr_result::ApplicationsWrapper *wrap)
{
	while (!app_reader_done)
	{
		ai->update_presence_and_size(wrap);
	}
}

void app_test_multi_threading(ApplicationsIndexer *ai, vr_result::ApplicationsWrapper *wrap)
{
	auto a = std::thread(app_reader, ai);
	auto b = std::thread(app_updater, ai, wrap);

	a.join();
	b.join();
}

void app_lookup_perf_test(ApplicationsIndexer *ai, vr_result::ApplicationsWrapper *wrap)
{
	int counter = 0;
	ai->update_presence_and_size(wrap);
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	
	for (int i = 0; i < 5000; i++)
	{
		for (int i = 0; i < ai->get_num_applications(); i++)
		{
			const char *k = ai->get_key_for_index(i);
			int i2 = ai->get_index_for_key(k);
			counter += i2;
		}
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	log_printf("lookups took %lld ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	log_printf("%d\n", counter);
}

void test_app_indexer()
{
	capture_test_context context;	// utility object used by unit test programs to facilitate setup.
	vr_result::ApplicationsWrapper wrap(context.raw_vr_interfaces().appi);	// this will force the interface to be bound.
	ApplicationsIndexer &ai(context.get_capture().m_keys.GetApplicationsIndexer()); // taking a reference here will force the indexer to be initialized.
	app_lookup_perf_test(&ai, &wrap);
	app_test_multi_threading(&ai, &wrap);
}