#pragma once
#include "ImSequencer.h"

#include <vector>

namespace eaSdkEngine {
class SoundMixer;

class SoundMixerSequencer : public ImSequencer::SequenceInterface
{
public:
  inline int
  GetFrameMin() const override { return m_frameMin; }
  inline int
  GetFrameMax() const override { return m_frameMax; }
  int
  GetItemCount() const override;
  int
  GetItemTrackCount(int index) const override;

  uint32
  GetCurrentFrame(int index) const override;
  uint32
  GetFirtsFrame(int index) const override;
  std::string
  GetItemLabel(int index) const override;

  void
  Get(int index,
      int trackIndex,
      int** start,
      int** end,
      float** data,
      int* dataCount,
      int* dataChannels,
      int* type,
      unsigned int* color) override;
  uint32
  getTrackStart(uint32 index, uint32 trackIndex) override;
  void
  setTrackStart(uint32 index, uint32 trackIndex, uint32 start) override;
  uint32
  getTrackEnd(uint32 index, uint32 trackIndex) override;
  void
  setTrackEnd(uint32 index, uint32 trackIndex, uint32 end) override;
  float*
  getTrackData(uint32 index, uint32 trackIndex) override;
  uint32
  getTrackDataCount(uint32 index, uint32 trackIndex) override;
  uint32
  getTrackChannelsCount(uint32 index, uint32 trackIndex) override;
  uint32
  getTrackColor(uint32 index, uint32 trackIndex) override;
  void
  setTrackColor(uint32 index, uint32 trackIndex, uint32 color) override;

  uint32
  getChannelStart(uint32 index) override;
  void
  setChannelStart(uint32 index, uint32 start) override;
  uint32
  getChannelEnd(uint32 index) override;
  void
  setChannelEnd(uint32 index, uint32 end) override;
  uint32
  getChannelSize(uint32 index) override;

  inline void
  Add(int type) override { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
  inline void
  Del(int index) override { myItems.erase(myItems.begin() + index); }
  inline void
  Duplicate(int index) override { myItems.push_back(myItems[index]); }

  uint32
  GetEventCount(int index) const override;
  uint32
  GetEventPos(int index, int eventID) const override;

  inline size_t
  GetCustomHeight(int /*index*/) override { return 0; }//return myItems[index].expanded ? 300 : 0; }

  void
  DoubleClick(int /*index*/) override {};

  void
  setMixerPtr(SoundMixer* mixerPtr);

  // my datas
  SoundMixerSequencer() : m_frameMin(0), m_frameMax(0) {}
  int m_frameMin, m_frameMax;
  struct MySequenceItem
  {
    int type;
    int frameStart, frameEnd;
    bool expanded;
  };
  std::vector<MySequenceItem> myItems;

  struct TrackDisplayInfo
  {
    uint32 color = 0xFFAA8080;
  };

  struct ChannelDisplayInfo
  {
    uint32 startPosition = 0;
    uint32 endPosition = 0;
    std::vector<TrackDisplayInfo> tracksInfo;
  };

  std::vector<ChannelDisplayInfo> m_channelsInfo;

  SoundMixer* m_mixer = nullptr;
};
}
