#include "eaSoundMixerSequencer.h"

#include "eaSoundMixer.h"
#include "eaSoundEvent.h"

namespace eaSdkEngine {
void
SoundMixerSequencer::Get(int index,
                         int trackIndex,
                         int** start,
                         int** end,
                         float** data,
                         int* dataCount,
                         int* dataChannels,
                         int* type,
                         unsigned int* color)
{
  MySequenceItem& item = myItems[index];
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    const auto& tracks = chann->getAudioTracks();

    if (!tracks.empty())
    {
      item.frameStart = static_cast<int>(tracks[trackIndex].getStartPosition());
      item.frameEnd = item.frameStart + static_cast<int>(tracks[trackIndex].getMaxPositionFreq());

      if (data)
        *data = tracks[trackIndex].getSound()->data;
      if (dataCount)
        *dataCount = tracks[trackIndex].getSound()->count;
      if (dataChannels)
        *dataChannels = tracks[trackIndex].getSound()->numChannels;
    }
  }

  if (color)
    *color = 0xFFAA8080; // same color for everyone, return color based on type
  if (start)
    *start = &item.frameStart;
  if (end)
    *end = &item.frameEnd;
  if (type)
    *type = item.type;
}
int
SoundMixerSequencer::GetItemCount() const
{
  //return static_cast<int>(myItems.size());
  if (m_mixer) return static_cast<int>(m_mixer->getChannelCount());

  return 0;
}
int
SoundMixerSequencer::GetItemTrackCount(int index) const
{
  if (m_mixer && static_cast<int>(m_mixer->getChannelCount()) > index)
  {
    return static_cast<int>(m_mixer->getChannel(index)->getAudioTracks().size());
  }
  return 0;
}
int
SoundMixerSequencer::GetCurrentFrame(int index) const
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    return static_cast<int>(chann->getTimePositionFreq());
  }
  return 0;
}
int
SoundMixerSequencer::GetFirtsFrame(int /*index*/) const
{
  return 0;
}
const char*
SoundMixerSequencer::GetItemLabel(int index) const
{
  static char tmps[512];
  //snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[myItems[index].type]);
  if (m_mixer)
    snprintf(tmps, 512, "%s", m_mixer->getChannel(index)->getName().c_str());
  return tmps;
}
void
SoundMixerSequencer::SetStart(int index, int trackIndex, int newStart)
{
  const auto& chann = m_mixer->getChannel(index);
  chann->moveTrackPCM(trackIndex, static_cast<float>(newStart));
}
int
SoundMixerSequencer::GetEventCount(int index) const
{
  if (m_mixer)
  {
    return static_cast<int>(m_mixer->getChannel(index)->getEvents().size());
  }
  return 0;
}
int
SoundMixerSequencer::GetEventPos(int index, int eventID) const
{
  if (m_mixer)
  {
    auto& events = m_mixer->getChannel(index)->getEvents();
  
    return static_cast<int>(events[eventID]->getFirstTime());
  }
  return 0;
}
//void
//SoundMixerSequencer::DoubleClick(int index)
//{
//  if (myItems[index].expanded)
//  {
//    myItems[index].expanded = false;
//    return;
//  }
//  for (auto& item : myItems)
//    item.expanded = false;
//
//  myItems[index].expanded = !myItems[index].expanded;
//}
void
SoundMixerSequencer::setMixerPtr(SoundMixer* mixerPtr)
{
  m_mixer = mixerPtr;
}
}