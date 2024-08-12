class Audio {
public:
  static const char *Volume;
  static const char *Resample;
  static const char *ResampleRatio;

  static const char *Handle;
  static const char *Synchronize;
  static const char *Frequency;
  static const char *Latency;

  virtual bool cap(const nall::string& ) { return false; }
  virtual nall::any get(const nall::string& ) { return false; }
  virtual bool set(const nall::string& , const nall::any& ) { return false; }

  virtual void sample(uint16_t , uint16_t ) {}
  virtual void clear() {}
  virtual bool init() { return true; }
  virtual void term() {}

  Audio() {}
  virtual ~Audio() {}
};
