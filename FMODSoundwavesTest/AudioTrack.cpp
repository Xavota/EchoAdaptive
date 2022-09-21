#include "AudioTrack.h"

void
AudioTrack::writeSoundData(float* data, int i)
{
  // If there is no sound assigned to the channel do nothing
  if (!sound || !sound->data || sound->numChannels == 0) return;
  // We need to write "count" samples to the "data" array
  // Since output is stereo it is easier to advance in pairs

  U32 realPosition = static_cast<U32>(position - startPosition)
                   * static_cast<U32>(sound->numChannels);
  // If we have reached the end of the sound, stop and return
  if (realPosition + sound->numChannels - 1 >= sound->count) {
    return;
  }

  // Read value from the sound data at the current position
  if (sound->numChannels == 1) {
    float value = (float)(sound->data[realPosition] * volume);
    // Write value to both the left and right channels
    data[i] += (float)(value * leftGain);
    data[i + 1] += (float)(value * rightGain);
  }
  else if (sound->numChannels > 1) {
    // Write value to both the left and right channels
    data[i] += (float)(sound->data[realPosition] * volume * leftGain);
    data[i + 1] += (float)(sound->data[realPosition + 1] * volume * rightGain);
  }
}