/* Global Headers */

#if defined(PLATFORM_X)
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  #include <X11/Xatom.h>
#elif defined(PLATFORM_OSX)
  #define __INTEL_COMPILER
  #define decimal CocoaDecimal
  #include <Carbon/Carbon.h>
  #undef decimal
#elif defined(PLATFORM_WIN)
  #define _WIN32_WINNT 0x0501
  #include <windows.h>
#endif

/* Audio */

#define DeclareAudio(Name) \
  class Audio##Name : public Audio { \
  public: \
    bool cap(const string& name) { return p.cap(name); } \
    any get(const string& name) { return p.get(name); } \
    bool set(const string& name, const any& value) { return p.set(name, value); } \
    \
    void sample(uint16_t left, uint16_t right) { p.sample(left, right); } \
    void clear() { p.clear(); } \
    bool init() { return p.init(); } \
    void term() { p.term(); } \
    \
    Audio##Name() : p(*new pAudio##Name) {} \
    ~Audio##Name() { delete &p; } \
  \
  private: \
    pAudio##Name &p; \
  };

#ifdef AUDIO_ALSA
  #include "./audio/alsa.hpp"
#endif

#ifdef AUDIO_AO
  #include "./audio/ao.hpp"
#endif

#ifdef AUDIO_DIRECTSOUND
  #include "./audio/directsound.hpp"
#endif

#ifdef AUDIO_OPENAL
  #include "./audio/openal.hpp"
#endif

#ifdef AUDIO_OSS
  #include "./audio/oss.hpp"
#endif

#ifdef AUDIO_PULSEAUDIO
  #include "./audio/pulseaudio.hpp"
#endif

#ifdef AUDIO_PULSEAUDIOSIMPLE
  #include "./audio/pulseaudiosimple.hpp"
#endif

#ifdef AUDIO_XAUDIO2
  #include "./audio/xaudio2.hpp"
#endif
