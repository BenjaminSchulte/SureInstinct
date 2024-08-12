#include <QFile>
#include <QDebug>
#include <sft/song/ItSong.hpp>
#include <sft/utils/DataStream.hpp>

using namespace ImpulseTracker;


template<typename T> static void deletePtrVector( QVector<T*> &vecs ) {
  
  for(typename QVector<T*>::iterator iter = vecs.begin(), ending = vecs.end();
    iter != ending; 
    iter++ ) {
  
    if( *iter )
      delete (*iter);
  }
  
  vecs.clear();
}

// -----------------------------------------------------------------------------
Module::Module(const QString &filename) {
  Filename = filename;
}

// -----------------------------------------------------------------------------
bool Module::load() {
  QFile _file(Filename);
  if (!_file.open(QIODevice::ReadOnly)) {
    return false;
  }

  Sft::DataStream file(&_file);
  file.setByteOrder(QDataStream::LittleEndian);

  if( file.read<uint8_t>() != 'I' ) return false;
  if( file.read<uint8_t>() != 'M' ) return false;
  if( file.read<uint8_t>() != 'P' ) return false;
  if( file.read<uint8_t>() != 'M' ) return false;

  Title = file.readString(26);

  PatternHighlight = file.read<uint16_t>();

  Length = file.read<uint16_t>();
  InstrumentCount = file.read<uint16_t>();
  SampleCount = file.read<uint16_t>();
  PatternCount = file.read<uint16_t>();
  Cwtv = file.read<uint16_t>();
  Cmwt = file.read<uint16_t>();
  Flags = file.read<uint16_t>();
  Special = file.read<uint16_t>();
  GlobalVolume = file.read<uint8_t>();
  MixingVolume = file.read<uint8_t>();
  InitialSpeed = file.read<uint8_t>();
  InitialTempo = file.read<uint8_t>();

  Sep = file.read<uint8_t>();
  PWD = file.read<uint8_t>();
  MessageLength = file.read<uint16_t>();
  
  uint32_t MessageOffset = file.read<uint32_t>();
  
  file.skipRawData( 4 ); // reserved
  
  for( int i = 0; i < 64; i++ )
    ChannelPan[i] = file.read<uint8_t>();

  for( int i = 0; i < 64; i++ )
    ChannelVolume[i] = file.read<uint8_t>();
  
  bool foundend=false;
  int ActualLength=Length;
  for( int i = 0; i < 256; i++ ) {
    Orders[i] = i < Length ? file.read<uint8_t>() : 255;
    if( Orders[i] == 255 && !foundend ) {
      foundend=true;
      ActualLength = i+1;
    }
  }
  
  Length = ActualLength;

  Instruments = new Instrument*[InstrumentCount];
  Samples = new Sample*[SampleCount];
  Patterns = new Pattern*[PatternCount];

  uint32_t *InstrTable = new uint32_t[InstrumentCount];
  uint32_t *SampleTable = new uint32_t[SampleCount];
  uint32_t *PatternTable = new uint32_t[PatternCount];
  
  for( int i = 0; i < InstrumentCount; i++ )
    InstrTable[i] = file.read<uint32_t>();
  for( int i = 0; i < SampleCount; i++ )
    SampleTable[i] = file.read<uint32_t>();
  for( int i = 0; i < PatternCount; i++ )
    PatternTable[i] = file.read<uint32_t>();

  for( int i = 0; i < InstrumentCount; i++ ) {
    file.seek( InstrTable[i] );
    Instruments[i] = new Instrument( file );
  }

  for( int i = 0; i < SampleCount; i++ ) {
    file.seek( SampleTable[i] );
    Samples[i] = new Sample( file );
  }

  for( int i = 0; i < PatternCount; i++ ) {
    if( PatternTable[i] ) {
      file.seek( PatternTable[i] );
      Patterns[i] = new Pattern( file );
    } else {
      Patterns[i] = new Pattern();
    }
  }

  if( MessageLength ) {
    Message = new char[MessageLength+1];
    file.seek( MessageOffset );
    Message[MessageLength] = 0;
    for( int i = 0; i < MessageLength; i++ )
      Message[i] = file.read<uint8_t>();
  } else {
    Message = 0;
  }

  delete[] InstrTable;
  delete[] SampleTable;
  delete[] PatternTable;

  return true;
}

// -----------------------------------------------------------------------------
Module::~Module() {
  for( int i = 0; i < InstrumentCount; i++ )
    delete Instruments[i];
  delete[] Instruments;
  for( int i = 0; i < SampleCount; i++ )
    delete Samples[i];
  delete[] Samples;
  for( int i = 0; i < PatternCount; i++ )
    delete Patterns[i];
  delete[] Patterns;
  if( Message ) {
    delete[] Message;
  }
}

// -----------------------------------------------------------------------------
Instrument::Instrument( Sft::DataStream &file ) {
  file.skipRawData(4); // IMPI
  DOSFilename[12] = 0;
  for( int i = 0; i < 12; i++ )
    DOSFilename[i] = file.read<uint8_t>();
  file.skipRawData(1);	// 00h
  NewNoteAction = file.read<uint8_t>();
  DuplicateCheckType = file.read<uint8_t>();
  DuplicateCheckAction = file.read<uint8_t>();
  Fadeout = file.read<uint16_t>();
  PPS = file.read<uint8_t>();
  PPC = file.read<uint8_t>();
  GlobalVolume = file.read<uint8_t>();
  DefaultPan = file.read<uint8_t>();
  RandomVolume = file.read<uint8_t>();
  RandomPanning = file.read<uint8_t>();
  TrackerVersion = file.read<uint16_t>();
  NumberOfSamples = file.read<uint8_t>();

  Name[26] = 0;
  for( int i = 0; i < 26; i++ )
    Name[i] = file.read<uint8_t>();

  InitialFilterCutoff = file.read<uint8_t>();
  InitialFilterResonance = file.read<uint8_t>();

  MidiChannel = file.read<uint8_t>();
  MidiProgram = file.read<uint8_t>();
  MidiBank = file.read<uint16_t>();

  file.read<uint8_t>(); // reserved
  
  for( int i = 0; i < 120; i++ ) {
    Notemap[i].Note = file.read<uint8_t>();
    Notemap[i].Sample = file.read<uint8_t>();
  }
  
  VolumeEnvelope = new Envelope( file );
  PanningEnvelope = new Envelope( file );
  PitchEnvelope = new Envelope( file );
}

// -----------------------------------------------------------------------------
Instrument::~Instrument() {
  delete VolumeEnvelope;
  delete PanningEnvelope;
  delete PitchEnvelope;
}

// -----------------------------------------------------------------------------
Envelope::Envelope( Sft::DataStream &file ) {
  uint8_t FLG = file.read<uint8_t>();
  Enabled = !!(FLG & 1);
  Loop = !!(FLG & 2);
  Sustain = !!(FLG & 4);
  IsFilter = !!(FLG & 128);

  Length = file.read<uint8_t>();
  
  LoopStart = file.read<uint8_t>();
  LoopEnd = file.read<uint8_t>();
  
  SustainStart = file.read<uint8_t>();
  SustainEnd = file.read<uint8_t>();

  for( int i = 0; i < 25; i++ ) {
    Nodes[i].y = file.read<uint8_t>();
    Nodes[i].x = file.read<uint16_t>();
  }
}

// -----------------------------------------------------------------------------
Sample::Sample( Sft::DataStream &file ) {
  file.skipRawData(4);	// IMPS
  DOSFilename[12] = 0;
  for( int i = 0; i < 12; i++ )
    DOSFilename[i] = file.read<uint8_t>();
  file.skipRawData(1);	// 00h
  GlobalVolume = file.read<uint8_t>();
  uint8_t Flags = file.read<uint8_t>();
  
  HasSample = !!(Flags & 1);
  Data.Bits16 = !!(Flags & 2);
  Stereo = !!(Flags & 4);
  Compressed = !!(Flags & 8);
  Data.Loop = !!(Flags & 16);
  Data.Sustain = !!(Flags & 32);
  Data.BidiLoop = !!(Flags & 64);
  Data.BidiSustain = !!(Flags & 128);
  
  DefaultVolume = file.read<uint8_t>();

  Name[26] = 0;
  for( int i = 0; i < 26; i++ )
    Name[i] = file.read<uint8_t>();
  
  Convert = file.read<uint8_t>();
  DefaultPanning = file.read<uint8_t>();
  
  Data.Length = file.read<uint32_t>();
  Data.LoopStart = file.read<uint32_t>();
  Data.LoopEnd = file.read<uint32_t>();
  Data.C5Speed = file.read<uint32_t>();
  Data.SustainStart = file.read<uint32_t>();
  Data.SustainEnd = file.read<uint32_t>();

  uint32_t SamplePointer = file.read<uint32_t>();

  VibratoSpeed = file.read<uint8_t>();
  VibratoDepth = file.read<uint8_t>();
  VibratoRate = file.read<uint8_t>();
  VibratoForm = file.read<uint8_t>();

  file.seek( SamplePointer );
  LoadData( file );
}

// -----------------------------------------------------------------------------
Sample::~Sample() {
  if( Data.Bits16 ) {
    delete[] Data.Data16;
  } else {
    delete[] Data.Data8;
  }
}

// -----------------------------------------------------------------------------
void Sample::LoadData( Sft::DataStream &file ) {
  if( !Compressed ) {

    // subtract offset for unsigned samples
    int offset = (Convert&1) ? 0 : (Data.Bits16 ? -32768 : -128);

    // signed samples
    if( Data.Bits16 ) {
      Data.Data16 = new int16_t[ Data.Length ];
      for( int i = 0; i < Data.Length; i++ ) {
        Data.Data16[i] = file.read<uint16_t>() + offset;
      }
    } else {
      Data.Data8 = new int8_t[ Data.Length ];
      for( int i = 0; i < Data.Length; i++ ) {
        Data.Data8[i] = file.read<uint8_t>() + offset;
      }
    }
  } else {
    // TODO : accept compressed samples.
    qWarning() << "Unsupported: compressed samples";
    Data.Data16 = nullptr;
  }
}

// -----------------------------------------------------------------------------
Pattern::Pattern( Sft::DataStream &file ) {
  DataLength = file.read<uint16_t>();
  Rows = file.read<uint16_t>();
  file.skipRawData( 4 ); // reserved
  Data = new uint8_t[ DataLength ];
  for( int i = 0; i < DataLength; i++ ) {
    Data[i] = file.read<uint8_t>();
  }
}

// -----------------------------------------------------------------------------
Pattern::Pattern() {
  Rows = 64;
  Data = new uint8_t[ Rows ];
  for( int i = 0; i < Rows; i++ ) {
    Data[i] = 0;
  }
}

// -----------------------------------------------------------------------------
Pattern::~Pattern() {
  if( Data ) {
    delete[] Data;
  }
}

// -----------------------------------------------------------------------------
SampleData::SampleData() {
  
}
