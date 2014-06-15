#include "AudioData.h"
#include "Initializer.h"
#include "Track.pb.h"
#include "EngineSample.h"
#include <fstream>
#include <iostream>
#include <vector>


bool Initializer::InitializeWaveSample(const char *filePath, AudioSample* &sample)
{
	if (sample != nullptr)
	{
		return false;
	}

	audio_data data;
	if (!ParseWaveFile(filePath, data))
	{
		return false;
	}

    sample = new AudioSample(data);
	return true;
}

bool Initializer::InitializeTrack(const char *trackName, EngineTrack* &track)
{
	if (track != nullptr)
	{
		return false;
	}

	// Verify that the version of the library linked against is 
	// compatible with the version the track headers
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	PGTA::Track protoTrack;

	std::fstream input(trackName, std::ios::in | std::ios::binary);

	if (!protoTrack.ParseFromIstream(&input))
	{
		std::cerr << "Failed to parse track." << std::endl;
		return false;
	}

	std::vector<EngineSample> engineSamples;
	std::vector<EngineGroup> engineGroups;

	for (int i = 0; i < protoTrack.samples_size(); ++i)
	{
		const PGTA::Track_Sample &protoSample = protoTrack.samples(i);

		AudioSample *sample = nullptr;

		// TODO need to parse what format the sample is
		if (!InitializeWaveSample(protoSample.filepath().c_str(), sample))
		{
			std::cerr << "Failed to initialize sample " << protoSample.filepath() << "." << std::endl;
			continue;
		}
	
		EngineSample::SampleProps engineSampleProps;
		engineSampleProps.frequency = protoSample.frequency();
		engineSampleProps.probability = protoSample.probability();
		engineSampleProps.volumeMultiplier = protoSample.volumemultiplier();

		engineSamples.emplace_back(EngineSample(sample, engineSampleProps));
	}

	for (int i = 0; i < protoTrack.groups_size(); ++i)
	{
		const PGTA::Track_Group &protoGroup = protoTrack.groups(i);

		std::vector<uint16_t> group;
		for (int j = 0; j < protoGroup.sampleindex_size(); ++j)
		{
			group.emplace_back(protoGroup.sampleindex(j));
		}

		engineGroups.emplace_back(group);
	}

	track = new EngineTrack(std::move(engineSamples), std::move(engineGroups));
	// Delete all global objects allocated by libprotobuf
	google::protobuf::ShutdownProtobufLibrary();
	return true;
}
