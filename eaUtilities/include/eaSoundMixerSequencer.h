#pragma once
#include "ImSequencer.h"

#include <vector>

namespace eaSdkEngine {
static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };

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

  int GetCurrentFrame(int index) const override;
  int GetFirtsFrame(int index) const override;

  inline int
  GetItemTypeCount() const override
  {
    return sizeof(SequencerItemTypeNames) / sizeof(char*);
  }
  inline const char*
  GetItemTypeName(int typeIndex) const override
  {
    return SequencerItemTypeNames[typeIndex];
  }
  const char*
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
  inline void
  Add(int type) override { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
  inline void
  Del(int index) override { myItems.erase(myItems.begin() + index); }
  inline void
  Duplicate(int index) override { myItems.push_back(myItems[index]); }

  void
  SetStart(int index, int trackIndex, int newStart) override;

  int
  GetEventCount(int index) const override;
  int
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


  SoundMixer* m_mixer = nullptr;
};
}
