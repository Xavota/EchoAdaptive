#include "eaSoundMixerSequencer.h"

#include "eaSoundMixer.h"
#include "eaSoundEvent.h"

#include "eaSoundEventLoop.h"

#include <vector>

namespace eaSdkEngine {
std::vector<SoundEventLoop*> loopEvents;
uint32 gChannelsStartPos = 0;
uint32 gChannelsEndPos = 0;

SoundMixerSequencer::SoundMixerSequencer() : m_frameMin(0), m_frameMax(0)
{
}

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

    auto& channInfo = m_channelsInfo[index];
    auto& trackInfo = channInfo.tracksInfo[trackIndex];
    return static_cast<uint32>(tracks[trackIndex].getStartPosition() + trackInfo.startPos);
  }
  return 0;
}
void
SoundMixerSequencer::setTrackStart(uint32 index, uint32 trackIndex, uint32 start)
{
  if (m_mixer)
  {
    auto& channInfo = m_channelsInfo[index];
    auto& trackInfo = channInfo.tracksInfo[trackIndex];

    const auto& chann = m_mixer->getChannel(index);
    chann->moveTrackPCM(trackIndex, static_cast<float>(start - trackInfo.startPos));
  }
}
uint32
SoundMixerSequencer::getTrackInnerStart(uint32 index, uint32 trackIndex)
{
  auto& channInfo = m_channelsInfo[index];
  auto& trackInfo = channInfo.tracksInfo[trackIndex];

  return trackInfo.startPos;
}
void
SoundMixerSequencer::setTrackInnerStart(uint32 index, uint32 trackIndex, uint32 start)
{
  auto& channInfo = m_channelsInfo[index];
  auto& trackInfo = channInfo.tracksInfo[trackIndex];

  trackInfo.startPos = start;
}
uint32
SoundMixerSequencer::getTrackEnd(uint32 index, uint32 trackIndex)
{
  if (m_mixer)
  {
    //const auto& chann = m_mixer->getChannel(index);

    auto& channInfo = m_channelsInfo[index];
    auto& trackInfo = channInfo.tracksInfo[trackIndex];
    uint32 trackSize = trackInfo.endPos - trackInfo.startPos;
    uint32 trackStart = getTrackStart(index, trackIndex);
    return trackStart + trackSize;
  }
  return 0;
}
void
SoundMixerSequencer::setTrackEnd(uint32 /*index*/, uint32 /*trackIndex*/, uint32 /*end*/)
{
}
uint32
SoundMixerSequencer::getTrackInnerEnd(uint32 index, uint32 trackIndex)
{
  auto& channInfo = m_channelsInfo[index];
  auto& trackInfo = channInfo.tracksInfo[trackIndex];

  return trackInfo.endPos;
}
void
SoundMixerSequencer::setTrackInnerEnd(uint32 index, uint32 trackIndex, uint32 end)
{
  auto& channInfo = m_channelsInfo[index];
  auto& trackInfo = channInfo.tracksInfo[trackIndex];

  trackInfo.endPos = end;
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
    const auto& track = chann->getAudioTracks()[trackIndex];
    return track.getSound()->count;
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

void
SoundMixerSequencer::addChannel()
{
  if (m_mixer)
  {
    m_mixer->addChannel("New channel");
    auto& channInfo = m_channelsInfo.emplace_back(ChannelDisplayInfo{});

    channInfo.startPosition = gChannelsStartPos;
    channInfo.endPosition = gChannelsEndPos;
  }
}

uint32
SoundMixerSequencer::moveTrack(uint32 srcChannIndex, uint32 srcTrackIndex,
                               uint32 destChannIndex, uint32 position)
{
  auto srcChann = m_mixer->getChannel(srcChannIndex);
  auto srcTrack = srcChann->getAudioTracks()[srcTrackIndex];
  auto destChann = m_mixer->getChannel(destChannIndex);
  uint32 destIndex = destChann->addTrack(srcTrack.getSoundPtr(), static_cast<float>(position));

  auto& srcChannInfo = m_channelsInfo[srcChannIndex];
  auto& srcTrackInfo = srcChannInfo.tracksInfo[srcTrackIndex];
  auto& destChannInfo = m_channelsInfo[destChannIndex];
  auto& destTrackInfo = destChannInfo.tracksInfo.emplace_back(TrackDisplayInfo{});

  destTrackInfo.color = srcTrackInfo.color;
  destTrackInfo.wrapMode = srcTrackInfo.wrapMode;
  destTrackInfo.startPos = srcTrackInfo.startPos;
  destTrackInfo.endPos = srcTrackInfo.endPos;

  removeTrack(srcChannIndex, srcTrackIndex);

  return destIndex;
}
void
SoundMixerSequencer::removeTrack(uint32 channIndex, uint32 trackIndex)
{
  auto chann = m_mixer->getChannel(channIndex);
  chann->removeTrack(trackIndex);

  auto& channInfo = m_channelsInfo[channIndex];
  if (trackIndex < channInfo.tracksInfo.size())
  {
    channInfo.tracksInfo.erase(channInfo.tracksInfo.begin() + trackIndex);
  }
}

void
SoundMixerSequencer::cutTrack(uint32 index, uint32 trackIndex, uint32 pcmPoint)
{
  auto chann = m_mixer->getChannel(index);
  auto track = chann->getAudioTracks()[trackIndex];
  chann->addTrack(track.getSoundPtr(), track.getStartPosition());

  auto& channInfo = m_channelsInfo[index];
  auto& trackInfoFirst = channInfo.tracksInfo[trackIndex];
  auto& trackInfoLast = channInfo.tracksInfo.emplace_back(TrackDisplayInfo{});
  trackInfoLast.color = trackInfoFirst.color;
  trackInfoLast.wrapMode = trackInfoFirst.wrapMode;

  trackInfoLast.startPos = pcmPoint;
  trackInfoLast.endPos = trackInfoFirst.endPos;
  trackInfoFirst.endPos = pcmPoint;
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
uint32
SoundMixerSequencer::GetItemCount() const
{
  //return static_cast<int>(myItems.size());
  if (m_mixer) return static_cast<uint32>(m_mixer->getChannelCount());

  return 0;
}
uint32
SoundMixerSequencer::GetItemTrackCount(int index) const
{
  if (m_mixer && static_cast<int>(m_mixer->getChannelCount()) > index)
  {
    return static_cast<uint32>(m_mixer->getChannel(index)->getAudioTracks().size());
  }
  return 0;
}
uint32
SoundMixerSequencer::GetCurrentFrame(int index) const
{
  if (m_mixer)
  {
    const auto& chann = m_mixer->getChannel(index);
    return static_cast<uint32>(chann->getTimePositionFreq());
  }
  return 0;
}
uint32
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
uint32
SoundMixerSequencer::GetEventCount(int index) const
{
  if (m_mixer)
  {
    return static_cast<uint32>(m_mixer->getChannel(index)->getEvents().size());
  }
  return 0;
}
uint32
SoundMixerSequencer::GetEventPos(int index, int eventID) const
{
  if (m_mixer)
  {
    auto& events = m_mixer->getChannel(index)->getEvents();
  
    return static_cast<uint32>(events[eventID]->getFirstTime());
  }
  return 0;
}
void
SoundMixerSequencer::tempAddTestEvent(uint32 channIndex, uint32 pos)
{
  if (m_mixer)
  {
    auto newEvent = loopEvents.emplace_back(new SoundEventLoop());
    newEvent->setVariable<float>("StartLoopTime", 0);

    m_mixer->getChannel(channIndex)->addEvent(newEvent, static_cast<float>(pos));
  }
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
    uint32 minTrackPos = UINT_MAX;
    uint32 maxTrackPos = 0;
    for (uint32 i = 0; i < channCount; ++i)
    {
      const auto& chann = m_mixer->getChannel(i);
      const auto& tracks = chann->getAudioTracks();
      for (const auto& t : tracks)
      {
        uint32 trackStart = static_cast<uint32>(t.getStartPosition());
        uint32 trackEnd = trackStart + static_cast<uint32>(t.getMaxPositionFreq());
        minTrackPos = trackStart < minTrackPos ? trackStart : minTrackPos;
        maxTrackPos = trackEnd > maxTrackPos ? trackEnd : maxTrackPos;
      }
    }
    for (uint32 i = 0; i < channCount; ++i)
    {
      auto& channInfo = m_channelsInfo.emplace_back(ChannelDisplayInfo{});

      const auto& chann = m_mixer->getChannel(i);
      const auto& tracks = chann->getAudioTracks();

      uint32 channColor = 0xFF000000 | ((rand() & 255) << 16) | ((rand() & 255) << 8) | (rand() & 255);
      for (const auto& t : tracks)
      {
        auto& trackInfo = channInfo.tracksInfo.emplace_back(TrackDisplayInfo{});
        //trackInfo.color = 0xFFAA8080;
        trackInfo.color = channColor;
        trackInfo.wrapMode = eTrackWrapeMode::kRepeat;
        trackInfo.startPos = 0;
        trackInfo.endPos = static_cast<uint32>(t.getMaxPositionFreq());
      }

      gChannelsStartPos = minTrackPos;
      gChannelsEndPos = maxTrackPos;
      channInfo.startPosition = gChannelsStartPos;
      channInfo.endPosition = gChannelsEndPos;
    }
  }
}
}