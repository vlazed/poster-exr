#include <cmath>

#include "interfaces/interfaces.h"
#include "materialsystem/imaterialsystem.h"
#include "hud.h"
#include "tier1/fmtstr.h"

#include <GarrysMod/FactoryLoader.hpp>
#include <GarrysMod/InterfacePointers.hpp>
#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/Lua/LuaConVars.h"

#define TINYEXR_USE_PIZ 1
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

IMaterialSystem* materials = nullptr;

/*
	require "poster_exr"
*/

using namespace GarrysMod::Lua;

int exr_counter = 0;
const std::string screenshots = "garrysmod/screenshots/";

LUA_FUNCTION( WriteEXR )
{
	LUA->CheckType(1, Type::NUMBER);
	float scale = LUA->GetNumber(1);

	++exr_counter;

	//char* date;
	//char* time;
	//if (!BGetLocalFormattedTime(0, date, 100, time, 100))
	//{
	//	Msg("Failed to get local time.\n");
	//	return;
	//}
	std::string filename = "poster_exr";
	filename.insert(0, screenshots);
	//strcat(filename, date);
	//strcat(filename, " ");
	//strcat(filename, time);
	filename.append(" ");
	filename.append(std::to_string(exr_counter));
	filename.append(".exr");

	//int width, height;
	// TODO: Get the current HUD size
	int width{ 1920 };
	int height{ 1080 };

	unsigned char* rgb = (unsigned char*)malloc(width * height * 4); // RGBA data

	CMatRenderContextPtr pRenderContext(materials);

	pRenderContext->ReadPixels(0, 0, width, height, rgb, IMAGE_FORMAT_BGR888);

	// https://github.com/syoyo/tinyexr/blob/cba4843738ff580723bee12e70740204b9560c21/examples/rgbe2exr/rgbe2exr.cc#L24
	EXRImage image;
	InitEXRImage(&image);

	EXRHeader header;
	InitEXRHeader(&header);

	// Print pixel data at the top-left corner
	Msg("""Pixel data at (0, 0): B=%d, G=%d, R=%d\n", 
		rgb[0], rgb[1], rgb[2]);

	std::vector<float> images[3];
	images[0].resize(width * height);
	images[1].resize(width * height);
	images[2].resize(width * height);

	for (int i = 0; i < width * height; i++) {
		images[2][i] = pow(rgb[3 * i + 2] / 255.0, 2.2);
		images[1][i] = pow(rgb[3 * i + 1] / 255.0, 2.2);
		images[0][i] = pow(rgb[3 * i + 0] / 255.0, 2.2);
	}

	float* image_ptr[3];
	image_ptr[0] = &(images[0].at(0)); // B
	image_ptr[1] = &(images[1].at(0)); // G
	image_ptr[2] = &(images[2].at(0)); // R
	
	image.num_channels = 3;
	image.images = (unsigned char**)image_ptr;
	image.width = width;
	image.height = height;

	header.num_channels = 3; // RGBA
	header.compression_type = TINYEXR_COMPRESSIONTYPE_PIZ; // PIZ compression
	header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
	// Must be BGR(A) order, since most of EXR viewers expect this channel order.
	strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
	strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
	strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

	header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
	header.requested_pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
	for (int i = 0; i < header.num_channels; i++) {
		header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
		header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
	}

	const char *err = nullptr;
	int ret = SaveEXRImageToFile(&image, &header, filename.c_str(), &err);
	if (ret != TINYEXR_SUCCESS)
	{
		if (err) {
			Msg("Error saving EXR: %s\n", err);
			free((void*)err);
		}
	} else {
		Msg("EXR file saved successfully: %s\n", filename.c_str());
	}

	free(rgb);
	free(header.channels);
	free(header.pixel_types);
	free(header.requested_pixel_types);

	return 0;
}

GMOD_MODULE_OPEN()
{
	Msg("PosterEXR module loaded.\n");

	SourceSDK::FactoryLoader material_loader("materialsystem");
	materials = (IMaterialSystem*)material_loader.GetInterface<IMaterialSystem>(MATERIAL_SYSTEM_INTERFACE_VERSION);
	if (materials == nullptr) {
		Msg("MaterialSystem interface is unavailable\n");
		return 0;
	}

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->GetField(-1, "render");
	LUA->PushCFunction(WriteEXR);
	LUA->SetField(-2, "WriteEXR");
	LUA->Pop(2); // Pop render and global table

	return 0;
}

GMOD_MODULE_CLOSE()
{
	return 0;
}