#include "capture_controller.h"
#include "capture_config.h"
#include "capture.h"
#include "openvr.h"

#include "capture_test_context.h"

void test_controller()
{
	{
		capture_test_context x;
		x.ForceInitAll();

		capture_controller controller;
		controller.init(&x.get_capture(), x.raw_vr_interfaces());
		
		// zero updates:
		// check that number of frames collected is zero
		const capture& model(controller.get_model());
		assert(model.get_num_updates() == 0);

		// first update:
		// check that after a single update, the number of updates has changed
		controller.update();
		assert(model.get_num_updates() == 1);
		assert(model.get_last_updated_frame() == 0);	// first frame is zero and timestamp should be zero
		assert(model.get_time_stamp(0) == 0);
		assert(model.get_closest_time_index(0) == 0);

		// second update
		controller.update();
		assert(model.get_num_updates() == 2);
		assert(model.get_last_updated_frame() == 1);	
		time_stamp_t last_time = model.get_time_stamp(1);
		assert(last_time > 0);
		assert(model.get_closest_time_index(last_time) == 1);

		// enque a new event
		vr::VREvent_t e;
		controller.enqueue_event(e);
		// since update has not been called make sure it hasn't been applied to the model prior to update
		assert(model.m_vr_events.size() == 0);
		controller.update();
		// make sure it was applied
		assert(model.m_vr_events.size() == 1);

		// add an overlay key
		{
			int num_key_updates_before = size_as_int(model.m_keys_updates.size());

			const char *test_key = "test_overlay";
			assert(model.m_keys.GetOverlayIndexer().get_index_for_key(test_key) == -1);
			VRKeysUpdate new_overlay = VRKeysUpdate::make_new_overlay(test_key);
			controller.enqueue_new_key(new_overlay);

			// check it hasn't taken effect
			assert(model.m_keys_updates.size() == num_key_updates_before);
			assert(model.m_keys.GetOverlayIndexer().get_index_for_key(test_key) == -1);

			controller.update();

			// check the overlay key took effect after update
			assert(model.m_keys.GetOverlayIndexer().get_index_for_key(test_key) != -1);
			// check that the update log grew
			assert(size_as_int(model.m_keys_updates.size()) > num_key_updates_before);
		}

		// add an app key
		{
			int num_key_updates_before = size_as_int(model.m_keys_updates.size());

			const char *test_key = "test_key";
			assert(model.m_keys.GetApplicationsIndexer().get_index_for_key(test_key) == -1);
			VRKeysUpdate new_app = VRKeysUpdate::make_new_app(test_key);
			controller.enqueue_new_key(new_app);

			// check it hasn't taken effect yet
			assert(model.m_keys_updates.size() == num_key_updates_before);
			assert(model.m_keys.GetApplicationsIndexer().get_index_for_key(test_key) == -1);

			controller.update();

			// check the overlay key took effect after update
			assert(model.m_keys.GetApplicationsIndexer().get_index_for_key(test_key) != -1);
			// check that the update log grew
			assert(size_as_int(model.m_keys_updates.size()) > num_key_updates_before);
		}

		// add a setting
		{
			int num_key_updates_before = size_as_int(model.m_keys_updates.size());

			const char *test_section = "test_section";
			SettingsIndexer::SectionSettingType test_setting_type = SettingsIndexer::SETTING_TYPE_BOOL;
			const char *test_setting = "test_setting_key";

			assert(!model.m_keys.GetSettingsIndexer().setting_exists(test_section, test_setting_type, test_setting));
			VRKeysUpdate new_setting = VRKeysUpdate::make_new_setting(test_section, test_setting_type, test_setting);
			controller.enqueue_new_key(new_setting);

			// check it hasn't taken effect yet
			assert(model.m_keys_updates.size() == num_key_updates_before);
			assert(!model.m_keys.GetSettingsIndexer().setting_exists(test_section, test_setting_type, test_setting));

			controller.update();

			// check the overlay key took effect after update
			assert(model.m_keys.GetSettingsIndexer().setting_exists(test_section, test_setting_type, test_setting));
			// check that the update log grew
			assert(size_as_int(model.m_keys_updates.size()) > num_key_updates_before);
		}

		// add a device property
		{
			int num_key_updates_before = size_as_int(model.m_keys_updates.size());

			PropertiesIndexer::PropertySettingType test_prop_type = PropertiesIndexer::PROP_FLOAT;
			const char *test_prop_name = "test_device_prop";

			assert(!model.m_keys.GetDevicePropertiesIndexer().property_exists(test_prop_type, test_prop_name));
			VRKeysUpdate new_device_property = VRKeysUpdate::make_new_device_property(test_prop_type, test_prop_name, 9999999);
			controller.enqueue_new_key(new_device_property);

			// check it hasn't taken effect yet
			assert(model.m_keys_updates.size() == num_key_updates_before);
			assert(!model.m_keys.GetDevicePropertiesIndexer().property_exists(test_prop_type, test_prop_name));

			controller.update();

			// check the overlay key took effect after update
			assert(model.m_keys.GetDevicePropertiesIndexer().property_exists(test_prop_type, test_prop_name));
			// check that the update log grew
			assert(size_as_int(model.m_keys_updates.size()) > num_key_updates_before);
		}

		// add a resource
		{
			int num_key_updates_before = size_as_int(model.m_keys_updates.size());

			std::string test_resource_name = "test_resource";
			std::string test_resource_directory = "c:\\test_resource";
			
			assert(!model.m_keys.GetResourcesIndexer().resource_key_exists(test_resource_name.c_str(), test_resource_directory.c_str()));
			VRKeysUpdate new_resource = VRKeysUpdate::make_new_resource(test_resource_name, test_resource_directory);
			controller.enqueue_new_key(new_resource);

			// check it hasn't taken effect yet
			assert(model.m_keys_updates.size() == num_key_updates_before);
			assert(!model.m_keys.GetResourcesIndexer().resource_key_exists(test_resource_name.c_str(), test_resource_directory.c_str()));

			controller.update();

			// check the overlay key took effect after update
			assert(model.m_keys.GetResourcesIndexer().resource_key_exists(test_resource_name.c_str(), test_resource_directory.c_str()));
			// check that the update log grew
			assert(size_as_int(model.m_keys_updates.size()) > num_key_updates_before);
		}

		// nearz farz
		{
			int num_key_updates_before = size_as_int(model.m_keys_updates.size());

			float test_nearz = 11.f;
			float test_farz = 12.f;

			assert(model.m_keys.GetNearZ() != test_nearz);	// since the number are quite strange should not fire
			assert(model.m_keys.GetFarZ() != test_farz);

			VRKeysUpdate modify_nearzfarz = VRKeysUpdate::make_modify_nearz_farz(test_nearz, test_farz);
			controller.enqueue_new_key(modify_nearzfarz);

			// check it hasn't taken effect yet
			assert(model.m_keys.GetNearZ() != test_nearz);	// since the number are quite strange should not fire
			assert(model.m_keys.GetFarZ() != test_farz);

			controller.update();

			// check the change took effect after update
			assert(model.m_keys.GetNearZ() == test_nearz);	// since the number are quite strange should not fire
			assert(model.m_keys.GetFarZ() == test_farz);
			// check that the update log grew
			assert(size_as_int(model.m_keys_updates.size()) > num_key_updates_before);
		}

	}
}