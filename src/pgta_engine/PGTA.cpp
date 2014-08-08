
#include "PGTA.h"

IPGTA* IPGTA::CreatePGTA()
{
    return new PGTA();
}

void IPGTA::FreePGTA(IPGTA* pgta)
{
    delete pgta;
}

void PGTA::Initialize(const PGTAConfig &config)
{

}

void PGTA::StartPlayback(const std::string &trackName)
{

}

void PGTA::StopPlayback()
{

}

void PGTA::TrasitionEvent(const std::string &event, uint8_t transitionAmount)
{

}

void PGTA::Update()
{

}

int PGTA::GetAudioData(char *buffer, int length)
{
    return 0;
}

const char* PGTA::GetAudioBuffer(int& numSamples)
{
    numSamples = 0;
    return nullptr;
}
