// OverlaysIndexer	
//
// monotonically increasing index<-> overlay key map
//
#pragma once
#include "openvr_serialization.h"
#include <unordered_map>
#include "vr_overlay_wrapper.h"
#include "vr_string_indexer.h"

// the idea of the overlay  is to make it possible to index this thing using integer indexes
// despite the fact that openvr interfaces index using both overlay handles AND string keys
//
class OverlayIndexer
{
public:
	OverlayIndexer();

	bool operator==(const OverlayIndexer &rhs) const
	{
		if (this == &rhs)
			return true;
		if (m_string_indexer != rhs.m_string_indexer)
			return false;
		return true;
	}

	bool operator != (const OverlayIndexer &rhs) const
	{
		return !(*this == rhs);
	}

	void Init(const char **initial_overlay_names, int num_names);
	void WriteToStream(BaseStream &s) const;
	void ReadFromStream(BaseStream &s);

	void update_presence(vr_result::OverlayWrapper *ow);

	int get_index_for_key(const char *key) const
	{
		return m_string_indexer.get_index_for_string(key);
	}

	const char *get_overlay_key_for_index(const uint32_t overlay_index)
	{
		return m_string_indexer.get_string_for_index(overlay_index);
	}

	void add_overlay_key(const char *key)
	{
		m_string_indexer.add_key_to_set(key, nullptr);
	}

	int get_num_overlays()
	{
		return m_string_indexer.size();
	}

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
	std::vector<int> live_indexes_tmp;  // this is just a temporary - don't encode or use it in determining ==
};
