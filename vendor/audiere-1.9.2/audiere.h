

#ifndef AUDIERE_H
#define AUDIERE_H


#include <vector>
#include <string>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#ifndef __cplusplus
  #error Audiere requires C++
#endif


#ifndef ADR_CALL
  #if defined(WIN32) || defined(_WIN32)
    #define ADR_CALL __stdcall
  #else
    #define ADR_CALL
  #endif
#endif


#ifndef ADR_DECL
#  if defined(WIN32) || defined(_WIN32)
#    ifdef AUDIERE_EXPORTS
#      define ADR_DECL __declspec(dllexport)
#    else
#      define ADR_DECL __declspec(dllimport)
#    endif
#  else
#    define ADR_DECL
#  endif
#endif


#define ADR_FUNCTION(ret, name) extern "C" ADR_DECL ret ADR_CALL name


namespace audiere {

  class RefCounted {
  protected:


    ~RefCounted() { }

  public:


    virtual void ADR_CALL ref() = 0;


    virtual void ADR_CALL unref() = 0;
  };


  template<typename T>
  class RefPtr {
  public:
    RefPtr(T* ptr = 0) {
      m_ptr = 0;
      *this = ptr;
    }

    RefPtr(const RefPtr<T>& ptr) {
      m_ptr = 0;
      *this = ptr;
    }

    ~RefPtr() {
      if (m_ptr) {
        m_ptr->unref();
        m_ptr = 0;
      }
    }

    RefPtr<T>& operator=(T* ptr) {
      if (ptr != m_ptr) {
        if (m_ptr) {
          m_ptr->unref();
        }
        m_ptr = ptr;
        if (m_ptr) {
          m_ptr->ref();
        }
      }
      return *this;
    }

    RefPtr<T>& operator=(const RefPtr<T>& ptr) {
      *this = ptr.m_ptr;
      return *this;
    }

    T* operator->() const {
      return m_ptr;
    }

    T& operator*() const {
      return *m_ptr;
    }

    operator bool() const {
      return (m_ptr != 0);
    }

    T* get() const {
      return m_ptr;
    }

  private:
    T* m_ptr;
  };


  template<class Interface>
  class RefImplementation : public Interface {
  protected:
    RefImplementation() {
      m_ref_count = 0;
    }


    virtual ~RefImplementation() { }

  public:
    void ADR_CALL ref() {
      ++m_ref_count;
    }

    void ADR_CALL unref() {
      if (--m_ref_count == 0) {
        delete this;
      }
    }

  private:
    int m_ref_count;
  };


  class File : public RefCounted {
  protected:
    ~File() { }

  public:


    enum SeekMode {
      BEGIN,
      CURRENT,
      END,
    };


    virtual int ADR_CALL read(void* buffer, int size) = 0;


    virtual bool ADR_CALL seek(int position, SeekMode mode) = 0;


    virtual int ADR_CALL tell() = 0;
  };
  typedef RefPtr<File> FilePtr;


  enum SampleFormat {
    SF_U8,
    SF_S16,
  };


  class SampleSource : public RefCounted {
  protected:
    ~SampleSource() { }

  public:


    virtual void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format) = 0;


    virtual int ADR_CALL read(int frame_count, void* buffer) = 0;


    virtual void ADR_CALL reset() = 0;


    virtual bool ADR_CALL isSeekable() = 0;


    virtual int ADR_CALL getLength() = 0;


    virtual void ADR_CALL setPosition(int position) = 0;


    virtual int ADR_CALL getPosition() = 0;
  };
  typedef RefPtr<SampleSource> SampleSourcePtr;


  class OutputStream : public RefCounted {
  protected:
    ~OutputStream() { }

  public:


    virtual void ADR_CALL play() = 0;


    virtual void ADR_CALL stop() = 0;


    virtual bool ADR_CALL isPlaying() = 0;


    virtual void ADR_CALL reset() = 0;


    virtual void ADR_CALL setRepeat(bool repeat) = 0;


    virtual bool ADR_CALL getRepeat() = 0;


    virtual void ADR_CALL setVolume(float volume) = 0;


    virtual float ADR_CALL getVolume() = 0;


    virtual void ADR_CALL setPan(float pan) = 0;


    virtual float ADR_CALL getPan() = 0;


    virtual void ADR_CALL setPitchShift(float shift) = 0;


    virtual float ADR_CALL getPitchShift() = 0;


    virtual bool ADR_CALL isSeekable() = 0;


    virtual int ADR_CALL getLength() = 0;


    virtual void ADR_CALL setPosition(int position) = 0;


    virtual int ADR_CALL getPosition() = 0;
  };
  typedef RefPtr<OutputStream> OutputStreamPtr;


  class AudioDevice : public RefCounted {
  protected:
    ~AudioDevice() { }

  public:


    virtual void ADR_CALL update() = 0;


    virtual OutputStream* ADR_CALL openStream(SampleSource* source) = 0;


    virtual OutputStream* ADR_CALL openBuffer(
      void* samples,
      int frame_count,
      int channel_count,
      int sample_rate,
      SampleFormat sample_format) = 0;
  };
  typedef RefPtr<AudioDevice> AudioDevicePtr;


  class SampleBuffer : public RefCounted {
  protected:
    ~SampleBuffer() { }

  public:


    virtual void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format) = 0;


    virtual int ADR_CALL getLength() = 0;


    virtual const void* ADR_CALL getSamples() = 0;


    virtual SampleSource* ADR_CALL openStream() = 0;
  };
  typedef RefPtr<SampleBuffer> SampleBufferPtr;


  enum SoundEffectType {
    SINGLE,
    MULTIPLE,
  };


  class SoundEffect : public RefCounted {
  protected:
    ~SoundEffect() { }

  public:


    virtual void ADR_CALL play() = 0;


    virtual void ADR_CALL stop() = 0;


    virtual void ADR_CALL setVolume(float volume) = 0;


    virtual float ADR_CALL getVolume() = 0;


    virtual void ADR_CALL setPan(float pan) = 0;


    virtual float ADR_CALL getPan() = 0;


    virtual void ADR_CALL setPitchShift(float shift) = 0;


    virtual float ADR_CALL getPitchShift() = 0;
  };
  typedef RefPtr<SoundEffect> SoundEffectPtr;


  namespace hidden {


    ADR_FUNCTION(const char*, AdrGetVersion)();


    ADR_FUNCTION(const char*, AdrGetSupportedFileFormats)();


    ADR_FUNCTION(const char*, AdrGetSupportedAudioDevices)();

    ADR_FUNCTION(int, AdrGetSampleSize)(SampleFormat format);

    ADR_FUNCTION(AudioDevice*, AdrOpenDevice)(
      const char* name,
      const char* parameters);

    ADR_FUNCTION(SampleSource*, AdrOpenSampleSource)(const char* filename);
    ADR_FUNCTION(SampleSource*, AdrOpenSampleSourceFromFile)(File* file);
    ADR_FUNCTION(SampleSource*, AdrCreateTone)(double frequency);
    ADR_FUNCTION(SampleSource*, AdrCreateSquareWave)(double frequency);
    ADR_FUNCTION(SampleSource*, AdrCreateWhiteNoise)();
    ADR_FUNCTION(SampleSource*, AdrCreatePinkNoise)();

    ADR_FUNCTION(OutputStream*, AdrOpenSound)(
      AudioDevice* device,
      SampleSource* source,
      bool streaming);

    ADR_FUNCTION(SampleBuffer*, AdrCreateSampleBuffer)(
      void* samples,
      int frame_count,
      int channel_count,
      int sample_rate,
      SampleFormat sample_format);
    ADR_FUNCTION(SampleBuffer*, AdrCreateSampleBufferFromSource)(
      SampleSource* source);

    ADR_FUNCTION(SoundEffect*, AdrOpenSoundEffect)(
      AudioDevice* device,
      SampleSource* source,
      SoundEffectType type);
  }


  inline const char* GetVersion() {
    return hidden::AdrGetVersion();
  }


  inline void SplitString(
    std::vector<std::string>& out,
    const char* in,
    char delim)
  {
    out.clear();
    while (*in) {
      const char* next = strchr(in, delim);
      if (next) {
        out.push_back(std::string(in, next));
      } else {
        out.push_back(in);
      }

      in = (next ? next + 1 : "");
    }
  }


  struct FileFormatDesc {

    std::string description;


    std::vector<std::string> extensions;
  };


  inline void GetSupportedFileFormats(std::vector<FileFormatDesc>& formats) {
    std::vector<std::string> descriptions;
    SplitString(descriptions, hidden::AdrGetSupportedFileFormats(), ';');

    formats.resize(descriptions.size());
    for (unsigned i = 0; i < descriptions.size(); ++i) {
      const char* d = descriptions[i].c_str();
      const char* colon = strchr(d, ':');
      formats[i].description.assign(d, colon);

      SplitString(formats[i].extensions, colon + 1, ',');
    }
  }


  struct AudioDeviceDesc {

    std::string name;


    std::string description;
  };


  inline void GetSupportedAudioDevices(std::vector<AudioDeviceDesc>& devices) {
    std::vector<std::string> descriptions;
    SplitString(descriptions, hidden::AdrGetSupportedAudioDevices(), ';');

    devices.resize(descriptions.size());
    for (unsigned i = 0; i < descriptions.size(); ++i) {
      std::vector<std::string> d;
      SplitString(d, descriptions[i].c_str(), ':');
      devices[i].name        = d[0];
      devices[i].description = d[1];
    }
  }


  inline int GetSampleSize(SampleFormat format) {
    return hidden::AdrGetSampleSize(format);
  }


  inline AudioDevice* OpenDevice(
    const char* name = 0,
    const char* parameters = 0)
  {
    return hidden::AdrOpenDevice(name, parameters);
  }


  inline SampleSource* OpenSampleSource(const char* filename) {
    return hidden::AdrOpenSampleSource(filename);
  }


  inline SampleSource* OpenSampleSource(File* file) {
    return hidden::AdrOpenSampleSourceFromFile(file);
  }


  inline SampleSource* CreateTone(double frequency) {
    return hidden::AdrCreateTone(frequency);
  }


  inline SampleSource* CreateSquareWave(double frequency) {
    return hidden::AdrCreateSquareWave(frequency);
  }


  inline SampleSource* CreateWhiteNoise() {
    return hidden::AdrCreateWhiteNoise();
  }


  inline SampleSource* CreatePinkNoise() {
    return hidden::AdrCreatePinkNoise();
  }


  inline OutputStream* OpenSound(
    AudioDevice* device,
    SampleSource* source,
    bool streaming = false)
  {
    return hidden::AdrOpenSound(device, source, streaming);
  }


  inline OutputStream* OpenSound(
    AudioDevice* device,
    const char* filename,
    bool streaming = false)
  {
    return OpenSound(device, OpenSampleSource(filename), streaming);
  }


  inline OutputStream* OpenSound(
    AudioDevice* device,
    File* file,
    bool streaming = false)
  {
    return OpenSound(device, OpenSampleSource(file), streaming);
  }


  inline SampleBuffer* CreateSampleBuffer(
    void* samples,
    int frame_count,
    int channel_count,
    int sample_rate,
    SampleFormat sample_format)
  {
    return hidden::AdrCreateSampleBuffer(
      samples, frame_count,
      channel_count, sample_rate, sample_format);
  }


  inline SampleBuffer* CreateSampleBuffer(SampleSource* source) {
    return hidden::AdrCreateSampleBufferFromSource(source);
  }


  inline SoundEffect* OpenSoundEffect(
    AudioDevice* device,
    SampleSource* source,
    SoundEffectType type)
  {
    return hidden::AdrOpenSoundEffect(device, source, type);
  }


  inline SoundEffect* OpenSoundEffect(
    AudioDevice* device,
    const char* filename,
    SoundEffectType type)
  {
    return OpenSoundEffect(device, OpenSampleSource(filename), type);
  }


  inline SoundEffect* OpenSoundEffect(
    AudioDevice* device,
    File* file,
    SoundEffectType type)
  {
    return OpenSoundEffect(device, OpenSampleSource(file), type);
  }
}


#endif
