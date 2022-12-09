#pragma once
#include "ImSequencer.h"

#include <vector>

namespace eaSdkEngine {
class SoundMixer;

class SoundMixerSequencer : public ImSequencer::SequenceInterface
{
public:
  inline uint32
  GetFrameMin() const override { return m_frameMin; }
  inline uint32
  GetFrameMax() const override { return m_frameMax; }
  uint32
  GetItemCount() const override;
  uint32
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
  getTrackInnerStart(uint32 index, uint32 trackIndex) override;
  void
  setTrackInnerStart(uint32 index, uint32 trackIndex, uint32 start) override;
  uint32
  getTrackEnd(uint32 index, uint32 trackIndex) override;
  void
  setTrackEnd(uint32 index, uint32 trackIndex, uint32 end) override;
  uint32
  getTrackInnerEnd(uint32 index, uint32 trackIndex) override;
  void
  setTrackInnerEnd(uint32 index, uint32 trackIndex, uint32 end) override;
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

  void
  addChannel() override;
  

  uint32
  moveTrack(uint32 srcChannIndex, uint32 srcTrackIndex,
            uint32 destChannIndex, uint32 position) override;
  void
  removeTrack(uint32 channIndex, uint32 trackIndex) override;

  void
  cutTrack(uint32 index, uint32 trackIndex, uint32 pcmPoint) override;

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

  void
  tempAddTestEvent(uint32 channIndex, uint32 pos) override; // TODO: Esto es una función de test, eliminar.

  inline size_t
  GetCustomHeight(int /*index*/) override { return 0; }//return myItems[index].expanded ? 300 : 0; }

  void
  DoubleClick(int /*index*/) override {};

  void
  setMixerPtr(SoundMixer* mixerPtr);

  // my datas
  SoundMixerSequencer();

  int m_frameMin, m_frameMax;
  struct MySequenceItem
  {
    int type;
    int frameStart, frameEnd;
    bool expanded;
  };
  std::vector<MySequenceItem> myItems;

  enum class eTrackWrapeMode : uint8
  {
    kCut = 0,
    kRepeat,
    kRepeatOnce,
    kMirror,
    kMirrorRepeat,
    kMirrorRepeatOnce
  };
  struct TrackDisplayInfo
  {
    uint32 color = 0xFFAA8080;
    eTrackWrapeMode wrapMode = eTrackWrapeMode::kRepeat;
    uint32 startPos = 0;
    uint32 endPos = 0;
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
