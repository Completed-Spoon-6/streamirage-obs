#include "streamirage-client.hpp"
#include <obs-module.h>

StreamirageClient::StreamirageClient() {}
StreamirageClient::~StreamirageClient() {}

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
	long long blurTypeIndex = obs_data_get_int(settings, SETTING_BLUR_TYPE);
	if (filterData && (filterData->selectedFilterIndex != blurTypeIndex)) {
		filterData->selectedFilterIndex = blurTypeIndex;
	}
}



obs_properties_t *StreamirageClient::GetProperties(void *data)
{
	struct filter_data *filterData = (struct filter_data *)data;

	if (filterData) {
		filterData->mainProperties = obs_properties_create();
		obs_properties_add_text(filterData->mainProperties, "session_id", "SessionID", OBS_TEXT_DEFAULT);
		obs_properties_add_text(filterData->mainProperties, "device_name", "DeviceName", OBS_TEXT_DEFAULT);

		auto* s = static_cast<dir_watch_media_source*>(data);
		const char* S_DIRECTORY = "directory"; // Define the missing identifier
		const char* T_DIRECTORY = "Directory"; // Define the missing identifier

		obs_properties_add_path(filterData->mainProperties, S_DIRECTORY, T_DIRECTORY,
								OBS_PATH_DIRECTORY, nullptr, s->directory);



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

