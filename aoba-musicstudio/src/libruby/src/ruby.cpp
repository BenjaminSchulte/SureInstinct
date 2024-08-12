#include <ruby/ruby.hpp>
using namespace nall;

#undef mkdir
#undef usleep
#include "ruby_impl.hpp"

namespace ruby {

AudioInterface audio;

/* AudioInterface */

void AudioInterface::driver(const char *driver) {
  if(p) term();

  if(!driver || !*driver) driver = default_driver();

  if(0);

  #ifdef AUDIO_ALSA
  else if(!strcmp(driver, "ALSA")) p = new AudioALSA();
  #endif

  #ifdef AUDIO_AO
  else if(!strcmp(driver, "libao")) p = new AudioAO();
  #endif

  #ifdef AUDIO_DIRECTSOUND
  else if(!strcmp(driver, "DirectSound")) p = new AudioDS();
  #endif

  #ifdef AUDIO_OPENAL
  else if(!strcmp(driver, "OpenAL")) p = new AudioOpenAL();
  #endif

  #ifdef AUDIO_OSS
  else if(!strcmp(driver, "OSS")) p = new AudioOSS();
  #endif

  #ifdef AUDIO_PULSEAUDIO
  else if(!strcmp(driver, "PulseAudio")) p = new AudioPulseAudio();
  #endif

  #ifdef AUDIO_PULSEAUDIOSIMPLE
  else if(!strcmp(driver, "PulseAudioSimple")) p = new AudioPulseAudioSimple();
  #endif

  #ifdef AUDIO_XAUDIO2
  else if(!strcmp(driver, "XAudio2")) p = new AudioXAudio2();
  #endif

  else p = new Audio();
}

//select the *safest* available driver, not the fastest
const char* AudioInterface::default_driver() {
  #if defined(AUDIO_DIRECTSOUND)
  return "DirectSound";
  #elif defined(AUDIO_XAUDIO2)
  return "XAudio2";
  #elif defined(AUDIO_ALSA)
  return "ALSA";
  #elif defined(AUDIO_OPENAL)
  return "OpenAL";
  #elif defined(AUDIO_PULSEAUDIO)
  return "PulseAudio";
  #elif defined(AUDIO_PULSEAUDIOSIMPLE)
  return "PulseAudioSimple";
  #elif defined(AUDIO_AO)
  return "libao";
  #elif defined(AUDIO_OSS)
  return "OSS";
  #else
  return "None";
  #endif
}

//returns list of available drivers, sorted from most to least optimal
const char* AudioInterface::driver_list() {
  return

  //Windows

  #if defined(AUDIO_DIRECTSOUND)
  "DirectSound;"
  #endif

  #if defined(AUDIO_XAUDIO2)
  "XAudio2;"
  #endif

  //Linux

  #if defined(AUDIO_ALSA)
  "ALSA;"
  #endif

  #if defined(AUDIO_OPENAL)
  "OpenAL;"
  #endif

  #if defined(AUDIO_OSS)
  "OSS;"
  #endif

  #if defined(AUDIO_PULSEAUDIO)
  "PulseAudio;"
  #endif

  #if defined(AUDIO_PULSEAUDIOSIMPLE)
  "PulseAudioSimple;"
  #endif

  #if defined(AUDIO_AO)
  "libao;"
  #endif

  "None";
}

#include "ruby_audio.hpp"
}
