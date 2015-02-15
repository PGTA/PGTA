// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_TRACK_PGTASCHEMA_H_
#define FLATBUFFERS_GENERATED_TRACK_PGTASCHEMA_H_

#include "flatbuffers/flatbuffers.h"


namespace PGTASchema {

struct Sample;
struct Group;
struct Track;

struct Sample FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const flatbuffers::String *name() const { return GetPointer<const flatbuffers::String *>(4); }
  const flatbuffers::String *defaultFile() const { return GetPointer<const flatbuffers::String *>(6); }
  uint64_t startTime() const { return GetField<uint64_t>(8, 0); }
  uint64_t frequency() const { return GetField<uint64_t>(10, 0); }
  float probability() const { return GetField<float>(12, 0); }
  float volumeMultiplier() const { return GetField<float>(14, 1); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* name */) &&
           verifier.Verify(name()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* defaultFile */) &&
           verifier.Verify(defaultFile()) &&
           VerifyField<uint64_t>(verifier, 8 /* startTime */) &&
           VerifyField<uint64_t>(verifier, 10 /* frequency */) &&
           VerifyField<float>(verifier, 12 /* probability */) &&
           VerifyField<float>(verifier, 14 /* volumeMultiplier */) &&
           verifier.EndTable();
  }
};

struct SampleBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_name(flatbuffers::Offset<flatbuffers::String> name) { fbb_.AddOffset(4, name); }
  void add_defaultFile(flatbuffers::Offset<flatbuffers::String> defaultFile) { fbb_.AddOffset(6, defaultFile); }
  void add_startTime(uint64_t startTime) { fbb_.AddElement<uint64_t>(8, startTime, 0); }
  void add_frequency(uint64_t frequency) { fbb_.AddElement<uint64_t>(10, frequency, 0); }
  void add_probability(float probability) { fbb_.AddElement<float>(12, probability, 0); }
  void add_volumeMultiplier(float volumeMultiplier) { fbb_.AddElement<float>(14, volumeMultiplier, 1); }
  SampleBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  SampleBuilder &operator=(const SampleBuilder &);
  flatbuffers::Offset<Sample> Finish() {
    auto o = flatbuffers::Offset<Sample>(fbb_.EndTable(start_, 6));
    return o;
  }
};

inline flatbuffers::Offset<Sample> CreateSample(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::String> name = 0,
   flatbuffers::Offset<flatbuffers::String> defaultFile = 0,
   uint64_t startTime = 0,
   uint64_t frequency = 0,
   float probability = 0,
   float volumeMultiplier = 1) {
  SampleBuilder builder_(_fbb);
  builder_.add_frequency(frequency);
  builder_.add_startTime(startTime);
  builder_.add_volumeMultiplier(volumeMultiplier);
  builder_.add_probability(probability);
  builder_.add_defaultFile(defaultFile);
  builder_.add_name(name);
  return builder_.Finish();
}

struct Group FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *sampleNames() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(4); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* sampleNames */) &&
           verifier.Verify(sampleNames()) &&
           verifier.VerifyVectorOfStrings(sampleNames()) &&
           verifier.EndTable();
  }
};

struct GroupBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_sampleNames(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> sampleNames) { fbb_.AddOffset(4, sampleNames); }
  GroupBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  GroupBuilder &operator=(const GroupBuilder &);
  flatbuffers::Offset<Group> Finish() {
    auto o = flatbuffers::Offset<Group>(fbb_.EndTable(start_, 1));
    return o;
  }
};

inline flatbuffers::Offset<Group> CreateGroup(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> sampleNames = 0) {
  GroupBuilder builder_(_fbb);
  builder_.add_sampleNames(sampleNames);
  return builder_.Finish();
}

struct Track FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  const flatbuffers::Vector<flatbuffers::Offset<Sample>> *samples() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Sample>> *>(4); }
  const flatbuffers::Vector<flatbuffers::Offset<Group>> *groups() const { return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Group>> *>(6); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* samples */) &&
           verifier.Verify(samples()) &&
           verifier.VerifyVectorOfTables(samples()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* groups */) &&
           verifier.Verify(groups()) &&
           verifier.VerifyVectorOfTables(groups()) &&
           verifier.EndTable();
  }
};

struct TrackBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_samples(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Sample>>> samples) { fbb_.AddOffset(4, samples); }
  void add_groups(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Group>>> groups) { fbb_.AddOffset(6, groups); }
  TrackBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  TrackBuilder &operator=(const TrackBuilder &);
  flatbuffers::Offset<Track> Finish() {
    auto o = flatbuffers::Offset<Track>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<Track> CreateTrack(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Sample>>> samples = 0,
   flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Group>>> groups = 0) {
  TrackBuilder builder_(_fbb);
  builder_.add_groups(groups);
  builder_.add_samples(samples);
  return builder_.Finish();
}

inline const Track *GetTrack(const void *buf) { return flatbuffers::GetRoot<Track>(buf); }

inline bool VerifyTrackBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<Track>(); }

inline const char *TrackIdentifier() { return "PGTA"; }

inline bool TrackBufferHasIdentifier(const void *buf) { return flatbuffers::BufferHasIdentifier(buf, TrackIdentifier()); }

inline void FinishTrackBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<Track> root) { fbb.Finish(root, TrackIdentifier()); }

}  // namespace PGTASchema

#endif  // FLATBUFFERS_GENERATED_TRACK_PGTASCHEMA_H_