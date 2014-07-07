
#pragma once

#include "IPGTA.h"

class PGTA : public IPGTA
{

public:
    virtual void Initialize(const PGTAConfig &config) override
    {
        
    }
    
    virtual void StartPlayback(const std::string &trackName) override
    {
        
    }
    
    virtual void StopPlayback() override
    {
        
    }
    
    virtual void TrasitionEvent(const std::string &event, uint8_t transitionAmount) override
    {
        
    }
    
    virtual void Update() override
    {
        
    }
    
    virtual int GetAudioData(char *buffer, int length) override
    {
        return 0;
    }
    
};
