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
uint32
SoundMixerSequencer::getTrackStart(uint32 index, uint32 trackIndex)
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    const auto& tracks = chann->getAudioTracks();
    return static_cast<uint32>(tracks[trackIndex].getStartPosition());
  }
  return 0;
}
void
SoundMixerSequencer::setTrackStart(uint32 index, uint32 trackIndex, uint32 start)
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    chann->moveTrackPCM(trackIndex, static_cast<float>(start));
  }
}
uint32
SoundMixerSequencer::getTrackEnd(uint32 index, uint32 trackIndex)
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    const auto& tracks = chann->getAudioTracks();
    uint32 trackStart = static_cast<int>(tracks[trackIndex].getStartPosition());
    return trackStart + static_cast<int>(tracks[trackIndex].getMaxPositionFreq());
  }
  return 0;
}
void
SoundMixerSequencer::setTrackEnd(uint32 /*index*/, uint32 /*trackIndex*/, uint32 /*end*/)
{
}
float*
SoundMixerSequencer::getTrackData(uint32 index, uint32 trackIndex)
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    const auto& tracks = chann->getAudioTracks();
    return tracks[trackIndex].getSound()->data;
  }
  return nullptr;
}
uint32
SoundMixerSequencer::getTrackDataCount(uint32 index, uint32 trackIndex)
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    const auto& tracks = chann->getAudioTracks();
    return tracks[trackIndex].getSound()->count;
  }
  return 0;
}
uint32
SoundMixerSequencer::getTrackChannelsCount(uint32 index, uint32 trackIndex)
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    const auto& tracks = chann->getAudioTracks();
    return tracks[trackIndex].getSound()->numChannels;
  }
  return 0;
}
uint32
SoundMixerSequencer::getTrackColor(uint32 index, uint32 trackIndex)
{
  if (static_cast<size_t>(index) < m_channelsInfo.size()
   && static_cast<size_t>(trackIndex) < m_channelsInfo[index].tracksInfo.size())
  {
    return m_channelsInfo[index].tracksInfo[trackIndex].color;
  }
  return 0;
}
void
SoundMixerSequencer::setTrackColor(uint32 index, uint32 trackIndex, uint32 color)
{
  if (static_cast<size_t>(index) < m_channelsInfo.size()
   && static_cast<size_t>(trackIndex) < m_channelsInfo[index].tracksInfo.size())
  {
    m_channelsInfo[index].tracksInfo[trackIndex].color = color;
  }
}
uint32
SoundMixerSequencer::getChannelStart(uint32 index)
{
  if (static_cast<size_t>(index) < m_channelsInfo.size())
  {
    return m_channelsInfo[index].startPosition;
  }
  return 0;
}
void
SoundMixerSequencer::setChannelStart(uint32 index, uint32 start)
{
  if (static_cast<size_t>(index) < m_channelsInfo.size())
  {
    m_channelsInfo[index].startPosition = start;
  }
}
uint32
SoundMixerSequencer::getChannelEnd(uint32 index)
{
  if (static_cast<size_t>(index) < m_channelsInfo.size())
  {
    return m_channelsInfo[index].endPosition;
  }
  return 0;
}
void
SoundMixerSequencer::setChannelEnd(uint32 index, uint32 end)
{
  if (static_cast<size_t>(index) < m_channelsInfo.size())
  {
    m_channelsInfo[index].endPosition = end;
  }
}
uint32
SoundMixerSequencer::getChannelSize(uint32 index)
{
  if (static_cast<size_t>(index) < m_channelsInfo.size())
  {
    return m_channelsInfo[index].endPosition - m_channelsInfo[index].startPosition;
  }
  return 0;
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
std::string
SoundMixerSequencer::GetItemLabel(int index) const
{
  if (m_mixer) return m_mixer->getChannel(index)->getName();
  return "";
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
  if (m_mixer)
  {
    uint32 channCount = m_mixer->getChannelCount();
    for (uint32 i = 0; i < channCount; ++i)
    {
      auto& channInfo = m_channelsInfo.emplace_back(ChannelDisplayInfo{});
      uint32 minTrackPos = UINT_MAX;
      uint32 maxTrackPos = 0;

      const auto& chann = m_mixer->getChannel(i);
      const auto& tracks = chann->getAudioTracks();
      for (const auto& t : tracks)
      {
        auto& trackInfo = channInfo.tracksInfo.emplace_back(TrackDisplayInfo{});
        trackInfo.color = 0xFFAA8080;

        uint32 trackStart = static_cast<uint32>(t.getStartPosition());
        uint32 trackEnd = trackStart + static_cast<uint32>(t.getMaxPositionFreq());
        minTrackPos = trackStart < minTrackPos ? trackStart : minTrackPos;
        maxTrackPos = trackEnd > maxTrackPos ? trackEnd : maxTrackPos;
      }

      channInfo.startPosition = minTrackPos;
      channInfo.endPosition = maxTrackPos;
    }
  }
}
}