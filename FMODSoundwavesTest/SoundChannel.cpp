#include "SoundChannel.h"

#include "SoundEffect.h"
#include "SynthetiseSound.h"

void
SoundChannel::PlaySound(FMODSound* mySound, float _paused)
{
  sound = mySound;
  position = 0;

  paused = _paused;
}
void
SoundChannel::PlaySyntheticSound(SynthetiseSound* _sound)
{
  syntheticSound = _sound;
}
void
SoundChannel::Stop()
{
  //sound = nullptr;
  position = 0;
  paused = true;
}
float
maxF(float f1, float f2)
{
  return f1 > f2 ? f1 : f2;
}
float
minF(float f1, float f2)
{
  return f1 < f2 ? f1 : f2;
}
void
SoundChannel::WriteSoundData(float* data, int count, float parentPitch)
{
  // If there is no sound assigned to the channel do nothing
  if (!sound || !sound->data || sound->numChannels == 0 || paused) return;
  // We need to write "count" samples to the "data" array
  // Since output is stereo it is easier to advance in pairs
  
  //auto effectSoundData = new float[sound->count];
  //memcpy(effectSoundData,
  //       sound->data,
  //       sound->count * sizeof(float));
  //for (auto& eff : m_effects) {
  //  if (eff) eff->applyPreEffect(effectSoundData, sound->count);
  //}

  for (int i = 0; i < count; i += 2) {
    U32 realPosition = static_cast<U32>(position)
                     * static_cast<U32>(sound->numChannels);
    // If we have reached the end of the sound, stop and return
    if (realPosition + sound->numChannels - 1 >= sound->count) {
      if (loop) {
        position = 0;
        realPosition = 0;
      }
      else {
        Stop();
        return;
      }
    }

    // Read value from the sound data at the current position
    if (sound->numChannels == 1) {
      float value = (float)(sound->data[realPosition] * volume);
      //float value = (float)(effectSoundData[realPosition] * volume);
      // Write value to both the left and right channels
      data[i] += (float)(value * leftGain);
      data[i + 1] += (float)(value * rightGain);
    }
    else if (sound->numChannels > 1) {
      // Write value to both the left and right channels
      data[i] += (float)(sound->data[realPosition] * volume * leftGain);
      data[i + 1] += (float)(sound->data[realPosition + 1] * volume * rightGain);
      //data[i] += (float)(effectSoundData[realPosition] * volume * leftGain);
      //data[i + 1] += (float)(effectSoundData[realPosition + 1] * volume * rightGain);
    }
    //if (syntheticSound) syntheticSound->getData(data, i);
    // Advance the position by one sample
    position += pitch * parentPitch;

  }

  //delete[] effectSoundData;

  //for (auto& eff : m_effects) {
  //  if (eff) eff->applyPostEffect(data, count);
  //}
}