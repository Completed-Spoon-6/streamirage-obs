#ifndef StreamirageClient_H
#define StreamirageClient_H

#include <obs-module.h>
#include <plugin-support.h>
#include <string>
#include <vector>
#include <memory>

#define SETTING_BLUR_SIZE "blur_size"
#define SETTING_BLUR_TYPE "blur_type"

class StreamirageClient {
private:
	struct filter_data {
		obs_source_t *context;
		gs_effect_t *effect;

		char *selectedFileName;
		long long selectedFilterIndex;

		obs_properties_t *mainProperties;
		obs_properties_t *filterProperties;
		obs_property_t *filterPropertiesGroup;
	};

	obs_source_info source_info = {};

	static const char *GetName(void *unused);
	static void *CreateSource(obs_data_t *settings, obs_source_t *source);
	static void DestroySource(void *data);
	static void UpdateSource(void *data, obs_data_t *settings);
	static obs_properties_t *GetProperties(void *data);
	static void RenderSource(void *data, gs_effect_t *effect);

public:
	StreamirageClient();
	~StreamirageClient();
	void RegisterSource();
};

#endif // StreamirageClient_H
