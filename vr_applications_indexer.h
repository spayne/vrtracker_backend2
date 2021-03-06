#pragma once
#include "vr_applications_wrapper.h"
#include "vr_observable_indexer.h"
#include "vr_string_indexer.h"

//
// assigns and tracks monotonically increasing indexes<-> application keys
//
class ApplicationsIndexer : public BasicObservableKeysIndexer
{
public:
	ApplicationsIndexer()
	{}

	bool operator == (const ApplicationsIndexer &rhs) const
	{
		if (&rhs == this)
			return true;
		return m_string_indexer == rhs.m_string_indexer;
	}
	bool operator != (const ApplicationsIndexer &rhs) const
	{
		return !(*this == rhs);
	}

	void WriteToStream(BaseStream &s) const;
	void ReadFromStream(BaseStream &s);

	void update_presence_and_size(vr_result::ApplicationsWrapper *ow);
	
	// number of applications ever seen
	volatile int get_num_applications() { return m_string_indexer.size(); }

	const char *get_key_for_index(uint32_t app_index, int *count = nullptr)
	{
		return m_string_indexer.get_string_for_index(app_index, count);
	}

	int get_index_for_key(const char *key) const
	{
		return m_string_indexer.get_index_for_string(key);
	}

	void add_app_key(const char *key)
	{
		m_string_indexer.add_key_to_set(key, nullptr);
	}

	// access number of currently 'live' indexes
	void read_lock_live_indexes()
	{
		m_string_indexer.read_lock_live_indexes();
	}
	const std::vector<int> &get_live_indexes() 
	{ 
		return m_string_indexer.get_live_indexes();
	}
	void read_unlock_live_indexes()
	{
		m_string_indexer.read_unlock_live_indexes();
	}

private:
	StringIndexer m_string_indexer;
	std::vector<int> working_indexes;	// the published vector is in m_string_indexer. 
										// this vector is just a working buffer
};
