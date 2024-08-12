#pragma once

#include <QVector>
#include <QString>

namespace Sft {
class DataStream;
}

namespace ImpulseTracker {

class Pattern {

public:
  Pattern( Sft::DataStream &file );
  Pattern();
  ~Pattern();

  uint16_t DataLength;
  uint16_t Rows;
  uint8_t *Data;
};

class SampleData {

public:

  SampleData();

  bool Bits16;
  int Length;
  int LoopStart;
  int LoopEnd;
  int C5Speed;
  int SustainStart;
  int SustainEnd;
  bool Loop;
  bool Sustain;
  bool BidiLoop;
  bool BidiSustain;

  union {
    int8_t* Data8;
    int16_t* Data16;
  };
};

class Sample {

  void LoadData( Sft::DataStream &f );
public:
  Sample( Sft::DataStream &f );
  ~Sample();

  char Name[27];
  char DOSFilename[13];

  uint8_t GlobalVolume;

  bool HasSample;
  bool Stereo;
  bool Compressed;
  

  uint8_t DefaultVolume;
  uint8_t DefaultPanning;
  uint8_t Convert;

  uint8_t VibratoSpeed;
  uint8_t VibratoDepth;
  uint8_t VibratoForm;
  uint8_t VibratoRate;

  SampleData Data;
};

typedef struct {
  uint8_t	Note;
  uint8_t	Sample;
} NotemapEntry;

typedef struct {
  uint8_t	y;
  uint16_t	x;
} EnvelopeEntry;

class Envelope {

public:
  Envelope( Sft::DataStream & );
  bool Enabled;
  bool Loop;
  bool Sustain;
  bool IsFilter;
  
  int Length;
  
  int LoopStart;
  int LoopEnd;
  int SustainStart;
  int SustainEnd;
  
  EnvelopeEntry Nodes[25];
};

class Instrument {

public:
  Instrument( Sft::DataStream & );
  ~Instrument();

  char Name[27];
  char DOSFilename[13];

  uint8_t	NewNoteAction;
  uint8_t	DuplicateCheckType;
  uint8_t	DuplicateCheckAction;

  uint16_t	Fadeout;
  uint8_t	PPS;
  uint8_t	PPC;
  uint8_t	GlobalVolume;
  uint8_t	DefaultPan;
  uint8_t	RandomVolume;
  uint8_t	RandomPanning;
  uint16_t	TrackerVersion;
  uint8_t	NumberOfSamples;
  uint8_t	InitialFilterCutoff;
  uint8_t	InitialFilterResonance;

  uint8_t	MidiChannel;
  uint8_t	MidiProgram;
  uint16_t MidiBank;

  NotemapEntry Notemap[ 120 ];

  Envelope *VolumeEnvelope;
  Envelope *PanningEnvelope;
  Envelope *PitchEnvelope;
};

class Module {
  
public:
  Module(const QString &filename);
  ~Module();

  bool load();

  QString Filename;
  
  QString Title;
  
  uint16_t PatternHighlight;
  uint16_t Length;
  uint16_t InstrumentCount;
  uint16_t SampleCount;
  uint16_t PatternCount;
  uint16_t Cwtv;
  uint16_t Cmwt;
  int Flags;
  int Special;
  uint8_t GlobalVolume;
  uint8_t MixingVolume;
  uint8_t InitialSpeed;
  uint8_t InitialTempo;
  uint8_t Sep;
  uint8_t PWD;

  uint16_t MessageLength;
  char *Message;

  int ChannelPan[64];
  int ChannelVolume[64];

  int Orders[256];

  Instrument **Instruments;
  Sample **Samples;
  Pattern **Patterns;
};
}
