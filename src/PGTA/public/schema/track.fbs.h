
// AUTO GENERATED, DON'T MODIFY
#pragma once

namespace PGTASchemaHeader
{
    static const char* TRACK_FBS = R"(
file_identifier "PGTA";
namespace PGTASchema;

table Sample
{
    name:string;
    defaultFile:string;
    startTime:ulong = 0;
    frequency:ulong;
    probability:float;
    volumeMultiplier:float = 1;
}

table Group
{
    sampleNames:[string];
}

table Track
{
    samples:[Sample];
    groups:[Group];
}

root_type Track;
)";
}
