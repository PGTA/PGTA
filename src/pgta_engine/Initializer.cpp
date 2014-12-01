
#include "AudioData.h"
#include "Initializer.h"
#include "Track.pb.h"
#include "EngineSample.h"
#include <fstream>
#include <iostream>
#include <vector>

AudioSample* Initializer::InitializeWaveSample(const char *filePath)
{
	audio_data data;
	if (ParseWaveFile(filePath, data))
	{
        return new AudioSample(data);
	}
    return nullptr;
}

EngineTrack* Initializer::InitializeTrack(const char *trackName)
{
	// Verify that the version of the library linked against is 
	// compatible with the version the track headers
	GOOGLE_PROTOBUF_VERIFY_VERSION;

    PGTA::Track protoTrack;
    {
        std::fstream input(trackName, std::ios::in | std::ios::binary);
        if (!input)
        {
            std::cerr << "Invalid track filename " << trackName << "." << std::endl;
            return nullptr;
        }

        bool parsed = protoTrack.ParseFromIstream(&input);
        input.close();

        if (!parsed)
        {
            std::cerr << "Failed to parse track." << std::endl;
            return nullptr;
        }
    }

	std::vector<EngineSample> engineSamples;
	for (const auto &protoSample : protoTrack.samples())
	{
		// TODO need to parse what format the sample is
        std::unique_ptr<AudioSample> sample(InitializeWaveSample(protoSample.filepath().c_str()));
        if (!sample)
		{
			std::cerr << "Failed to initialize sample " << protoSample.filepath() << "." << std::endl;
			continue;
		}

        using namespace std::chrono;
	
		EngineSample::SampleProps engineSampleProps;
		engineSampleProps.frequency = duration_cast<high_resolution_clock::duration>(milliseconds(protoSample.frequency()));
        engineSampleProps.startTime = duration_cast<high_resolution_clock::duration>(milliseconds(protoSample.starttime()));
		engineSampleProps.probability = protoSample.probability();
		engineSampleProps.volumeMultiplier = protoSample.volumemultiplier();

		engineSamples.emplace_back(std::move(sample), engineSampleProps);
	}

    std::vector<EngineGroup> engineGroups;
    for (const auto &protoGroup : protoTrack.groups())
	{
        std::vector<uint16_t> group(protoGroup.sampleindex().begin(),
                                    protoGroup.sampleindex().end());
        engineGroups.emplace_back(std::move(group));
	}

	return new EngineTrack(std::move(engineSamples), std::move(engineGroups), ++instanceNumber, trackName);
}
