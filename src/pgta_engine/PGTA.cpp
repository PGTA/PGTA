
#include "PGTA.h"

#include "Initializer.h"

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
    m_config = config;
}

void PGTA::StartPlayback(const std::string &trackName)
{
}

void PGTA::StopPlayback()
{

}

void PGTA::TransitionEvent(const std::string &event, uint8_t transitionAmount)
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
