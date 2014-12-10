// TrackWriter.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include "Track.pb.h"

using namespace std;


int main(int argc, char *argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc != 2) {
		cerr << "Usage:  " << argv[0] << " Track File" << endl;
		return -1;
	}

	PGTA::Track track;

	//create an input file stream
	fstream input(argv[1], ios::in | ios::binary);

	if (!input)
	{
		cout << argv[1] << " not found. Creating a new file." << endl;
	}
	else if (!track.ParseFromIstream(&input)) {
		cerr << "Failed to parse track." << endl;
		return -1;
	}

	int i = 0;
	for (;;)
	{
		cout << "Enter another sample? (Y/N) ";
		string enterSample;
		cin >> enterSample;
		if (enterSample != "Y")
		{
			break;
		}
		i += 1;
		PGTA::Track_Sample *sample = track.add_samples();
		cout << "Sample " << i << endl;
		cout << "Enter a sample name: ";
		string sampleName;
		cin >> sampleName;
		sample->set_filepath(sampleName);
		cout << "Enter a sample frequency: ";
		uint64_t frequency;
		cin >> frequency;
		sample->set_frequency(frequency);
		cout << "Enter a sample probability: ";
		uint32_t probability;
		cin >> probability;
		sample->set_probability(probability);
		cout << "Enter a sample volumeMultipler: ";
		float volumeMultiplier;
		cin >> volumeMultiplier;
		sample->set_volumemultiplier(volumeMultiplier);
	}
	i = 0;
	for (;;)
	{
		cout << endl << "Enter another group? (Y/N) ";
		string enterGroup;
		cin >> enterGroup;
		if (enterGroup != "Y")
		{
			break;
		}
		i++;
		PGTA::Track_Group *group = track.add_groups();
		cout << "Group " << i << endl;
		for (;;)
		{
			int sampleIndex;
			cin >> sampleIndex;
			if (sampleIndex < 0) { break; }
			group->add_sampleindex(sampleIndex);
		}
	}

	fstream output(argv[1], ios::out | ios::trunc | ios::binary);
	if (!track.SerializePartialToOstream(&output))
	{
		cerr << "failed to write track";
		return -1;
	}

	//delete all global object allocated by libprotobuf
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}

