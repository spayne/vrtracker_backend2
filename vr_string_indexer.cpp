#include "vr_string_indexer.h"

void StringIndexer::WriteToStream(EncodeStream &s) const
{
	int size = updated_size;
	s.memcpy_out_to_stream(&size, sizeof(size));

	write_vector_of_strings_to_stream(s, keys);
	s.forward_container_out_to_stream(live_indexes);
}


void StringIndexer::ReadFromStream(EncodeStream &s)
{
	int size;
	s.memcpy_from_stream(&size, sizeof(size));
	updated_size = size;

	keys.clear();
	keys2index.clear();
	read_vector_of_strings_from_stream(s, keys);

	s.forward_container_from_stream(live_indexes);

	// rebuild hash
	for (int i = 0; i < size_as_int(keys.size()); i++)
	{
		keys2index.insert({ keys[i].c_str(), i });
	}
}

void StringIndexer::maybe_swap_live_indexes(std::vector<int> * v)
{
	if (*v != live_indexes)
	{
		live_index_lock.lock(); // writer lock
		live_indexes.swap(*v);
		live_index_lock.unlock();
	}
}