// testserialization
//

#include "capture_test_context.h"
#include "capture_traverser.h"

using namespace vr;

void test_capture_serialization()
{
	using namespace vr;
	using namespace vr_result;
	using std::range;
	
	log_printf("start of test_capture_serialization\n");
	capture_test_context::reset_globals();

	capture_traverser u;
	{
		capture_test_context contexta;
		assert(contexta.get_capture() == contexta.get_capture());
	}

	{
		capture_test_context contexta;
		capture_test_context contextb;
		assert(contexta.get_capture() == contextb.get_capture());
	}

	{
		log_printf("serializing empty context\n");
		// no updates
		capture_test_context contexta;
		u.save_capture_to_binary_file(&contexta.get_capture(), "c:\\temp\\tracker0.bin");

		capture_test_context contextb;
		u.load_capture_from_binary_file(&contextb.get_capture(), "c:\\temp\\tracker0.bin");
		assert(contexta.get_capture() == contextb.get_capture());
	}

	capture_test_context::reset_globals();
	{
		log_printf("serializing one update\n");
		// one update
		capture_test_context contexta;
		u.update_capture_parallel(&contexta.get_capture(), &contexta.raw_vr_interfaces());
		assert(u.save_capture_to_binary_file(&contexta.get_capture(), "c:\\temp\\tracker1.bin"));
		//contexta.get_capture().m_state_registry.dump();
		
		capture_test_context contextb;
		assert(u.load_capture_from_binary_file(&contextb.get_capture(), "c:\\temp\\tracker1.bin"));
		assert(contexta.get_capture() == contextb.get_capture());
	}
	capture_test_context::reset_globals();
	{
		log_printf("serializing 10 updates\n");
		// ten updates ... 100mb with no textures... 600mb with textures
		capture_test_context contexta;
		for (int i = 0; i < 10; i++)
		{
			u.update_capture_parallel(&contexta.get_capture(), &contexta.raw_vr_interfaces());
		}
		log_printf("writing\n");
		u.save_capture_to_binary_file(&contexta.get_capture(), "c:\\temp\\tracker10.bin");

		log_printf("reading\n");
		capture_test_context contextb;
		u.load_capture_from_binary_file(&contextb.get_capture(), "c:\\temp\\tracker10.bin");
		assert(contexta.get_capture() == contextb.get_capture());
	}
	capture_test_context::reset_globals();
	log_printf("done test_capture_serialization\n");
}

// i can log object changes and probably find them pretty easily
// what about object construction/insertion/spawn

void stream_frame(int frame)
{
	//vector <int> ids_for_frame;
	//write frame id
	//write number of chunks
#if 0
	for (id : frame)
	{
		ObjectIf *p = capture.getObjectForId();
		p->encode(s);
	}
#endif


}

#if 0
// load frame
{

	read frame
		// find the object
		//
		// update the object
		p->decode(s);
}
#endif
