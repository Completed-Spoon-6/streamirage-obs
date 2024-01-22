#include "streamirage-client.hpp"
#include <obs-module.h>

StreamirageClient::StreamirageClient() {}
StreamirageClient::~StreamirageClient() {}

const char* S_DIRECTORY = "directory"; 
const char* T_DIRECTORY = "Directory";
const char* S_SESSIONS_ID = "session_id";
const char* T_SESSIONS_ID = "SessionID";
const char* S_DEVICE_NAME = "device_name";
const char* T_DEVICE_NAME = "DeviceName";
const char* S_CONNECT = "connect";
const char* T_CONNECT = "Connect";

void StreamirageClient::RegisterSource()
{
	source_info.id = "streamirage";
	source_info.type = OBS_SOURCE_TYPE_FILTER;
	source_info.output_flags = OBS_SOURCE_VIDEO;
	source_info.get_name = GetName;
	source_info.create = CreateSource;
	source_info.destroy = DestroySource;
	source_info.video_render = RenderSource;
	source_info.update = UpdateSource;
	source_info.get_properties = GetProperties;
	obs_register_source(&source_info);
}


struct dir_watch_media_source {
	obs_source_t *source;
	char *directory;
	char *file;
	char *extension;
	char *delete_file;
	time_t time;
	bool hotkeys_added;
	long long scan_interval;
	float duration;
	bool enabled;
};


const char *StreamirageClient::GetName(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("Streamirage");
}


void *StreamirageClient::CreateSource(obs_data_t *settings, obs_source_t *source)
{
	struct filter_data *filterData =
		(struct filter_data *)bzalloc(sizeof(struct filter_data));

	filterData->context = source;
	filterData->selectedFilterIndex = 0;


	obs_source_update(source, settings);

	return filterData;
}

void StreamirageClient::DestroySource(void *data)
{
	struct filter_data *filterData = (struct filter_data *)data;
	if (filterData->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filterData->effect);
		obs_leave_graphics();
	}
	bfree(data);
}

void StreamirageClient::UpdateSource(void *data, obs_data_t *settings)
{
	// obs_log(LOG_INFO, "Updating Source!");
    struct filter_data *filterData = (struct filter_data *)data;
    if (!filterData) return;

    const char* sessionId = obs_data_get_string(settings, S_SESSIONS_ID);
    const char* deviceName = obs_data_get_string(settings, S_DEVICE_NAME);
	const char* directory = obs_data_get_string(settings, S_DIRECTORY);

	obs_log(LOG_INFO, "SessionID: %s", sessionId);
	obs_log(LOG_INFO, "Device Name: %s", deviceName);
	obs_log(LOG_INFO, "Directory: %s", directory);
}

bool StreamirageClient::ButtonClicked(obs_properties_t* /*props*/, obs_property_t* /*property*/, void* data) {
    struct filter_data *filterData = static_cast<struct filter_data*>(data);
    if (!filterData) {
        obs_log(LOG_ERROR, "Filter data is null");
        return false;
    }

    obs_source_t *source = filterData->context;
    obs_data_t *settings = obs_source_get_settings(source);
    
    const char *sessionID = obs_data_get_string(settings, S_SESSIONS_ID);
    const char *deviceName = obs_data_get_string(settings, S_DEVICE_NAME);

    obs_log(LOG_INFO, "Button Pressed! Session ID: %s, Device Name: %s", sessionID, deviceName);

    obs_data_release(settings);
    return true;
}


obs_properties_t *StreamirageClient::GetProperties(void *data)
{
	struct filter_data *filterData = (struct filter_data *)data;

	if (filterData) {
		filterData->mainProperties = obs_properties_create();
		obs_properties_add_text(filterData->mainProperties, S_SESSIONS_ID, T_SESSIONS_ID, OBS_TEXT_DEFAULT);
		obs_properties_add_text(filterData->mainProperties, S_DEVICE_NAME, T_DEVICE_NAME, OBS_TEXT_DEFAULT);

		auto* s = static_cast<dir_watch_media_source*>(data);


		obs_properties_add_path(filterData->mainProperties, S_DIRECTORY, T_DIRECTORY,
								OBS_PATH_DIRECTORY, nullptr, s->directory);

		obs_properties_add_button(filterData->mainProperties, S_CONNECT, T_CONNECT, ButtonClicked);

		return filterData->mainProperties;
	}

	return nullptr;
}


void StreamirageClient::RenderSource(void *data, gs_effect_t *effect)
{
	struct filter_data *filterData = (struct filter_data *)data;

	if (!obs_source_process_filter_begin(filterData->context, GS_RGBA,
					     OBS_ALLOW_DIRECT_RENDERING))
		return;


	if (!filterData->effect) {
		obs_source_process_filter_end(filterData->context, effect, 0,
					      0);
	} else {
		obs_source_process_filter_end(filterData->context,
					      filterData->effect, 0, 0);
	}
}

