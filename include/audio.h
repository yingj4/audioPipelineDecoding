#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <vector>
#include <string>
// #include <spatialaudio/Ambisonics.h>
#include <fstream>
#include <../../include/hpvm.h>

#include <../include/AmbisonicCommons.h>

// #include "../libspatialaudio/include/AmbisonicDecoder.h"
// #include "../libspatialaudio/include/AmbisonicEncoder.h"
#include "../include/kiss_fftr.h"

// #include "../include/mit_hrtf.h"
// #include "../libspatialaudio/include/hrtf/sofa_hrtf.h"

// #include <mysofa.h>

#include <memory>

#include "../libspatialaudio/include/AmbisonicPsychoacousticFilters.h"

#define SAMPLERATE 48000
#define BLOCK_SIZE 1024
#define NORDER 3
#define NUM_CHANNELS (OrderToComponents(NORDER, true))
#define NUM_SRCS 16

// Class HRTF
class HRTF
{
public:
    HRTF(unsigned i_sampleRate)
        : i_sampleRate(i_sampleRate), i_len(0)
    { }
    virtual ~HRTF() = default;

    virtual bool get(float f_azimuth, float f_elevation, float** pfHRTF) = 0;

    bool isLoaded() { return i_len != 0; }
    unsigned getHRTFLen() { return i_len; }

// protected:
    unsigned i_sampleRate;
    unsigned i_len;
};

// Class MIT_HRTF
class MIT_HRTF : public HRTF
{
public:
    MIT_HRTF(unsigned i_sampleRate);
    bool get(float f_azimuth, float f_elevation, float **pfHRTF);
};

// Class SOFA_HRTF
// class SOFA_HRTF : public HRTF
// {
// public:
//     SOFA_HRTF(std::string path, unsigned i_sampleRate);
//     ~SOFA_HRTF();
//     bool get(float f_azimuth, float f_elevation, float **pfHRTF);

// // private:
//     struct MYSOFA_EASY *hrtf;

//     unsigned i_filterExtraLength;
//     int i_internalLength;
// };

// Class CAmbisonicBase
class CAmbisonicBase
{
public:
    CAmbisonicBase();
    virtual ~CAmbisonicBase() = default;
    /**
        Gets the order of the current Ambisonic configuration.
    */
    unsigned GetOrder();
    /**
        Gets true or false depending on whether the current Ambisonic
        configuration has height(3D).
    */
    bool GetHeight();
    /**
        Gets the number of B-Format channels in the current Ambisonic
        configuration.
    */
    unsigned GetChannelCount();
    /**
        Re-create the object for the given configuration. Previous data is
        lost.
    */
    virtual bool Configure(unsigned nOrder, bool b3D, unsigned nMisc);
    /**
        Not implemented.
    */
    virtual void Reset() = 0;
    /**
        Not implemented.
    */
    virtual void Refresh() = 0;

// protected:
    unsigned m_nOrder;
    bool m_b3D;
    unsigned m_nChannelCount;
    bool m_bOpt;
};

// Class CAmbisonicSource
/// Base class for encoder and speaker

/** This acts as a base class for single point 3D objects such as encoding a
    mono stream into a 3D soundfield, or a single speaker for decoding a 3D
    soundfield. */

class CAmbisonicSource : public CAmbisonicBase
{
public:
    CAmbisonicSource();
    /**
        Re-create the object for the given configuration. Previous data is
        lost. The last argument is not used, it is just there to match with
        the base class's form. Returns true if successful.
    */
    virtual bool Configure(unsigned nOrder, bool b3D, unsigned nMisc);
    /**
        Not implemented.
    */
    virtual void Reset();
    /**
        Recalculates coefficients.
    */
    virtual void Refresh();
    /**
        Set azimuth, elevation, and distance settings.
    */
    virtual void SetPosition(PolarPoint polPosition);
    /**
        Get azimuth, elevation, and distance settings.
    */
    virtual PolarPoint GetPosition();
    /**
        Sets the weight [0,1] for the spherical harmonics of the given order.
    */
    virtual void SetOrderWeight(unsigned nOrder, float fWeight);
    /**
        Sets the weight [0,1] for the spherical harmonics of all orders.
    */
    virtual void SetOrderWeightAll(float fWeight);
    /**
        Sets the spherical harmonic coefficient for a given component
        Can be used for preset decoders to non-regular arrays where a Sampling decoder is sub-optimal
    */
    virtual void SetCoefficient(unsigned nChannel, float fCoeff);
    /**
        Gets the weight [0,1] for the spherical harmonics of the given order.
    */
    virtual float GetOrderWeight(unsigned nOrder);
    /**
        Gets the coefficient of the specified channel/component. Useful for the
        Binauralizer.
    */
    virtual float GetCoefficient(unsigned nChannel);
    /**
        Sets the source's gain.
    */
    virtual void SetGain(float fGain);
    /**
        Gets the source's gain.
    */
    virtual float GetGain();

// protected:
    std::vector<float> m_pfCoeff;
    std::vector<float> m_pfOrderWeights;
    PolarPoint m_polPosition;
    float m_fGain;
};

// Class CBFormat
/// Storage for BFormat signals.

/** This object is used to store and transfer BFormat signals. Memory is
    allocated for the number of channels needed for the given Ambisonic
    configuration (order and 2D/3D) and the number of samples. */

class CBFormat : public CAmbisonicBase
{
public:
    CBFormat();
    /**
        Returns the number of samples.
    */
    unsigned GetSampleCount();
    /**
        Re-create the buffers needed for the given configuration. Previous
        buffer contents are lost.
    */
    bool Configure(unsigned nOrder, bool b3D, unsigned nSampleCount);
    /**
        Fill the buffer with zeros.
    */
    void Reset();
    /**
        Not implemented.
    */
    void Refresh();
    /**
        Copy a number of samples to a specific channel of the BFormat.
    */
    void InsertStream(float* pfData, unsigned nChannel, unsigned nSamples);
    /**
        Copy a number of samples from a specific channel of the BFormat.
    */
    void ExtractStream(float* pfData, unsigned nChannel, unsigned nSamples);

    /**
        Copy the content of the buffer. It is assumed that the two objects are
        of the same configuration.
    */
    void operator = (const CBFormat &bf);
    /**
        Returns true if the configuration of the two objects match.
    */
    bool operator == (const CBFormat &bf);
    /**
        Returns true if the configuration of the two objects don't match.
    */
    bool operator != (const CBFormat &bf);
    CBFormat& operator += (const CBFormat &bf);
    CBFormat& operator -= (const CBFormat &bf);
    CBFormat& operator *= (const CBFormat &bf);
    CBFormat& operator /= (const CBFormat &bf);
    CBFormat& operator += (const float &fValue);
    CBFormat& operator -= (const float &fValue);
    CBFormat& operator *= (const float &fValue);
    CBFormat& operator /= (const float &fValue);

// protected:
    unsigned m_nSamples;
    unsigned m_nDataLength;
    std::vector<float> m_pfData;
    std::unique_ptr<float*[]> m_ppfChannels;

    //friend classes cannot be pure abstract type,
    //so must list each friend class manually
    friend class CAmbisonicEncoder;
    friend class CAmbisonicEncoderDist;
    friend class CAmbisonicDecoder;
    friend class CAmbisonicSpeaker;
    friend class CAmbisonicMicrophone;
    friend class CAmbisonicProcessor;
    friend class CAmbisonicBinauralizer;
    friend class CAmbisonicZoomer;
};

// Class CAmbisonicEncoder
class CAmbisonicEncoder : public CAmbisonicSource
{
public:
    CAmbisonicEncoder();
    ~CAmbisonicEncoder();
    /**
        Re-create the object for the given configuration. Previous data is
        lost. Returns true if successful.
    */
    virtual bool Configure(unsigned nOrder, bool b3D, unsigned nMisc);
    /**
        Recalculate coefficients, and apply normalisation factors.
    */
    void Refresh();
    /**
        Encode mono stream to B-Format.
    */
    void Process(float* pfSrc, unsigned nSamples, CBFormat* pBFDst);
};

// Class CAmbisonicEncoderDist
const unsigned knSpeedOfSound = 344;
const unsigned knMaxDistance = 150;

/// Ambisonic encoder with distance cues.

/** This is similar to a normal the ambisonic encoder, but takes the source's
    distance into account, delaying the signal, adjusting its gain, and
    implementing "W-Panning"(interior effect). If distance is not an issue,
    then use CAmbisonicEncoder which is more efficient. */

class CAmbisonicEncoderDist : public CAmbisonicEncoder
{
public:
    CAmbisonicEncoderDist();
    ~CAmbisonicEncoderDist();
    /**
        Re-create the object for the given configuration. Previous data is
        lost. Returns true if successful.
    */
    virtual bool Configure(unsigned nOrder, bool b3D, unsigned nSampleRate);
    /**
        Resets members such as delay lines.
    */
    virtual void Reset();
    /**
        Refreshes coefficients.
    */
    virtual void Refresh();
    /**
        Encode mono stream to B-Format.
    */
    void Process(float* pfSrc, unsigned nSamples, CBFormat* pBFDst);
    /**
        Set the radius of the intended playback speaker setup which is used for
        the interior effect (W-Panning).
    */
    void SetRoomRadius(float fRoomRadius);
    /**
        Returns the radius of the intended playback speaker setup, which is
        used for the interior effect (W-Panning).
    */
    float GetRoomRadius();

protected:
    unsigned m_nSampleRate;
    float m_fDelay;
    int m_nDelay;
    unsigned m_nDelayBufferLength;
    float* m_pfDelayBuffer;
    int m_nIn;
    int m_nOutA;
    int m_nOutB;
    float m_fRoomRadius;
    float m_fInteriorGain;
    float m_fExteriorGain;
};

// Class CAmbisonicProcessor
enum ProcessorDOR
{
    kYaw, kRoll, kPitch, kNumProcessorDOR //(Degrees of Rotation)
};

enum ProcessorModes
{
    kYawRollPitch, kYawPitchRoll,
    kRollYawPitch, kRollPitchYaw,
    kPitchYawRoll, kPitchRollYaw,
    kNumProcessorModes
};


class CAmbisonicProcessor;


/// Struct for soundfield rotation.
class Orientation
{
public:
    Orientation(float fYaw, float fPitch, float fRoll)
        : fYaw(fYaw), fPitch(fPitch), fRoll(fRoll)
    {
        float fCosYaw = cosf(fYaw);
        float fSinYaw = sinf(fYaw);
        float fCosRoll = cosf(fRoll);
        float fSinRoll = sinf(fRoll);
        float fCosPitch = cosf(fPitch);
        float fSinPitch = sinf(fPitch);

        /* Conversion from yaw, pitch, roll (ZYX) to ZYZ convention to match rotation matrices
        This method reduces the complexity of the rotation matrices since the Z0 and Z1 rotations are the same form */
        float r33 = fCosPitch * fCosRoll;
        if (r33 == 1.f)
        {
            fBeta = 0.f;
            fGamma = 0.f;
            fAlpha = atan2(fSinYaw, fCosYaw);
        }
        else
        {
            if (r33 == -1.f)
            {
                fBeta = M_PI;
                fGamma = 0.f;
                fAlpha = atan2(-fSinYaw, fCosYaw);
            }
            else
            {

                float r32 = -fCosYaw * fSinRoll + fCosRoll * fSinPitch * fSinYaw ;
                float r31 = fCosRoll * fCosYaw * fSinPitch + fSinRoll * fSinYaw ;
                fAlpha = atan2( r32 , r31 );

                fBeta = acos( r33 );

                float r23 = fCosPitch * fSinRoll;
                float r13 = -fSinPitch;
                fGamma = atan2( r23 , -r13 );
            }
        }
    }

    friend class CAmbisonicProcessor;

//private:
    /** rotation around the Z axis (yaw) */
    float fYaw;
    /** rotation around the Y axis (pitch) */
    float fPitch;
    /** rotation around the X axis (roll) */
    float fRoll;

    /** These angles are obtained from Yaw, Pitch and Roll (ZYX convention)**/
    /** They follow the ZYZ convention to match the rotation equations **/
    /** rotation around the Z axis */
    float fAlpha;
    /** rotation around the X axis */
    float fBeta;
    /** rotation around the new Z axis */
    float fGamma;
};


/// Ambisonic processor.

/** This object is used to rotate the BFormat signal around all three axes.
    Orientation structs are used to define the the soundfield's orientation. */

class CAmbisonicProcessor : public CAmbisonicBase
{
public:
    CAmbisonicProcessor();
    ~CAmbisonicProcessor();
    /**
        Re-create the object for the given configuration. Previous data is
        lost. The last argument is not used, it is just there to match with 
        the base class's form. Returns true if successful.
    */
    bool Configure(unsigned nOrder, bool b3D, unsigned nBlockSize, unsigned nMisc);
    /**
        Not implemented.
    */
    void Reset();
    /**
        Recalculate coefficients.
    */
    void Refresh();
    /**
        Set yaw, roll, and pitch settings.
    */
    void SetOrientation(Orientation orientation);
    /**
        Get yaw, roll, and pitch settings.
    */
    Orientation GetOrientation();
    /**
        Rotate B-Format stream.
    */
    void Process(CBFormat* pBFSrcDst, unsigned nSamples);

// private:
    void ProcessOrder1_3D(CBFormat* pBFSrcDst, unsigned nSamples);
    void ProcessOrder2_3D(CBFormat* pBFSrcDst, unsigned nSamples);
    void ProcessOrder3_3D(CBFormat* pBFSrcDst, unsigned nSamples);
    void ProcessOrder1_2D(CBFormat* pBFSrcDst, unsigned nSamples);
    void ProcessOrder2_2D(CBFormat* pBFSrcDst, unsigned nSamples);
    void ProcessOrder3_2D(CBFormat* pBFSrcDst, unsigned nSamples);

    void ShelfFilterOrder(CBFormat* pBFSrcDst, unsigned nSamples);

// protected:
    Orientation m_orientation;
    float* m_pfTempSample;

    kiss_fftr_cfg m_pFFT_psych_cfg;
    kiss_fftr_cfg m_pIFFT_psych_cfg;

    float* m_pfScratchBufferA;
    float** m_pfOverlap;
    unsigned m_nFFTSize;
    unsigned m_nBlockSize;
    unsigned m_nTaps;
    unsigned m_nOverlapLength;
    unsigned m_nFFTBins;
    float m_fFFTScaler;

    kiss_fft_cpx** m_ppcpPsychFilters;
    kiss_fft_cpx* m_pcpScratch;

    float m_fCosAlpha;
    float m_fSinAlpha;
    float m_fCosBeta;
    float m_fSinBeta;
    float m_fCosGamma;
    float m_fSinGamma;
    float m_fCos2Alpha;
    float m_fSin2Alpha;
    float m_fCos2Beta;
    float m_fSin2Beta;
    float m_fCos2Gamma;
    float m_fSin2Gamma;
    float m_fCos3Alpha;
    float m_fSin3Alpha;
    float m_fCos3Beta;
    float m_fSin3Beta;
    float m_fCos3Gamma;
    float m_fSin3Gamma;
};

// Class CAmbisonicSpeaker
class CAmbisonicSpeaker : public CAmbisonicSource
{
public:
    CAmbisonicSpeaker();
    ~CAmbisonicSpeaker();
    /**
        Re-create the object for the given configuration. Previous data is
        lost. The last argument is not used, it is just there to match with 
        the base class's form. Returns true if successful.
    */
    virtual bool Configure(unsigned nOrder, bool b3D, unsigned nMisc);
    /**
        Recalculate coefficients, and apply normalisation factors.
    */
    void Refresh();
    /**
        Decode B-Format to speaker feed.
    */
    void Process(CBFormat* pBFSrc, unsigned nSamples, float* pfDst);
};

// Class CAmbisonicDecoder
enum Amblib_SpeakerSetUps
{
    kAmblib_CustomSpeakerSetUp = -1,
    ///2D Speaker Setup
    kAmblib_Mono, kAmblib_Stereo, kAmblib_LCR, kAmblib_Quad, kAmblib_50,
    kAmblib_Pentagon, kAmblib_Hexagon, kAmblib_HexagonWithCentre, kAmblib_Octagon, 
    kAmblib_Decadron, kAmblib_Dodecadron, 
    ///3D Speaker Setup
    kAmblib_Cube,
    kAmblib_Dodecahedron,
    kAmblib_Cube2,
    kAmblib_MonoCustom,
    kAmblib_NumOfSpeakerSetUps
};

/// Ambisonic decoder

/** This is a basic decoder, handling both default and custom speaker
    configurations. */

class CAmbisonicDecoder : public CAmbisonicBase
{
public:
    CAmbisonicDecoder();
    ~CAmbisonicDecoder();
    /**
        Re-create the object for the given configuration. Previous data is
        lost. nSpeakerSetUp can be any of the ::SpeakerSetUps enumerations. If
        ::kCustomSpeakerSetUp is used, then nSpeakers must also be given,
        indicating the number of speakers in the custom speaker configuration.
        Else, if using one of the default configurations, nSpeakers does not
        need to be specified. Function returns true if successful.
    */
    bool Configure(unsigned nOrder, bool b3D, int nSpeakerSetUp, unsigned nSpeakers = 0);
    /**
        Resets all the speakers.
    */
    void Reset();
    /**
        Refreshes all the speakers.
    */
    void Refresh();
    /**
        Decode B-Format to speaker feeds.
    */
    void Process(CBFormat* pBFSrc, unsigned nSamples, float** ppfDst);
    /**
        Returns the current speaker setup, which is a ::SpeakerSetUps
        enumeration.
    */
    int GetSpeakerSetUp();
    /**
        Returns the number of speakers in the current speaker setup.
    */
    unsigned GetSpeakerCount();
    /**
        Used when current speaker setup is ::kCustomSpeakerSetUp, to position
        each speaker. Should be used by iterating nSpeaker for the number of speakers
        declared present in the current speaker setup, using polPosition to position
        each on.
    */
    void SetPosition(unsigned nSpeaker, PolarPoint polPosition);
    /**
        Used when current speaker setup is ::kCustomSpeakerSetUp, it returns
        the position of the speaker of index nSpeaker, in the current speaker
        setup.
    */
    PolarPoint GetPosition(unsigned nSpeaker);
    /**
        Sets the weight [0,1] for the spherical harmonics of the given order,
        at the given speaker.
    */
    void SetOrderWeight(unsigned nSpeaker, unsigned nOrder, float fWeight);
    /**
        Returns the weight [0,1] for the spherical harmonics of the given order,
        at the given speaker.
    */
    float GetOrderWeight(unsigned nSpeaker, unsigned nOrder);
    /**
        Gets the coefficient of the specified channel/component of the
        specified speaker. Useful for the Binauralizer.
    */
    virtual float GetCoefficient(unsigned nSpeaker, unsigned nChannel);
    /**
        Sets the coefficient of the specified channel/component of the
        specified speaker. Useful for presets for irregular physical loudspeakery arrays
    */
    void SetCoefficient(unsigned nSpeaker, unsigned nChannel, float fCoeff);

// protected:
    void SpeakerSetUp(int nSpeakerSetUp, unsigned nSpeakers = 1);

    int m_nSpeakerSetUp;
    unsigned m_nSpeakers;
    CAmbisonicSpeaker* m_pAmbSpeakers;
};

// Class CAmbisonicZoomer
/// Ambisonic zoomer.

/** This object is used to apply a zoom effect into BFormat soundfields. */

class CAmbisonicZoomer : public CAmbisonicBase
{
public:
    CAmbisonicZoomer();
    virtual ~CAmbisonicZoomer() = default;
    /**
        Re-create the object for the given configuration. Previous data is
        lost. The last argument is not used, it is just there to match with
        the base class's form. Returns true if successful.
    */
    virtual bool Configure(unsigned nOrder, bool b3D, unsigned nMisc);
    /**
        Not implemented.
    */
    void Reset();
    /**
        Recalculate coefficients.
    */
    void Refresh();
    /**
        Set zoom factor. This is in a range from -1 to 1, with 0 being no zoom,
        1 full forward zoom, and -1 full forward backwards.
    */
    void SetZoom(float fZoom);
    /**
        Get zoom factor.
    */
    float GetZoom();
    /**
        Zoom into B-Format stream.
    */
    void Process(CBFormat* pBFSrcDst, unsigned nSamples);
    /**
        Compute factorial of integer
    */
    float factorial(unsigned M);
// protected:
    CAmbisonicDecoder m_AmbDecoderFront;

    std::unique_ptr<float[]> m_AmbEncoderFront;
    std::unique_ptr<float[]> m_AmbEncoderFront_weighted;
    std::unique_ptr<float[]> a_m;

    float m_fZoom;
    float m_fZoomRed;
    float m_AmbFrontMic;
    float m_fZoomBlend;
};

// Class CAmbisonicBinauralizer
/// Ambisonic binauralizer

/** B-Format to binaural decoder. */

class CAmbisonicBinauralizer : public CAmbisonicBase
{
public:
    CAmbisonicBinauralizer();
    /**
        Re-create the object for the given configuration. Previous data is
        lost. The tailLength variable it updated with the number of taps
        used for the processing, and this can be used to offset the delay
        this causes. The function returns true if the call is successful.
    */
    virtual bool Configure(unsigned nOrder,
                           bool b3D,
                           unsigned nSampleRate,
                           unsigned nBlockSize,
                           unsigned& tailLength,
                           std::string HRTFPath = "");
    /**
        Resets members.
    */
    virtual void Reset();
    /**
        Refreshes coefficients.
    */
    virtual void Refresh();
    /**
        Decode B-Format to binaural feeds. There is no arguement for the number
        of samples to process, as this is determined by the nBlockSize argument
        in the constructor and Configure() function. It is the responsibility of
        program using this library to handle the blocks of the signal by FIFO
        buffers or other means.
    */
    void Process(CBFormat* pBFSrc, float** ppfDst);

// protected:
    CAmbisonicDecoder m_AmbDecoder;

    unsigned m_nBlockSize;
    unsigned m_nTaps;
    unsigned m_nFFTSize;
    unsigned m_nFFTBins;
    float m_fFFTScaler;
    unsigned m_nOverlapLength;

    std::unique_ptr<struct kiss_fftr_state, decltype(&kiss_fftr_free)> m_pFFT_cfg;
    std::unique_ptr<struct kiss_fftr_state, decltype(&kiss_fftr_free)> m_pIFFT_cfg;
    std::vector<std::unique_ptr<kiss_fft_cpx[]>> m_ppcpFilters[2];
    std::unique_ptr<kiss_fft_cpx[]> m_pcpScratch;

    std::vector<float> m_pfScratchBufferA;
    std::vector<std::vector<float>> m_pfScratchBufferB; // Ying: change from std::vector<float> to std::vector<std::vector<float>> with size 16 for 3D spatial audio
    std::vector<float> m_pfScratchBufferC;
    std::vector<float> m_pfOverlap[2];

    HRTF *getHRTF(unsigned nSampleRate, std::string HRTFPath);
    virtual void ArrangeSpeakers();
    virtual void AllocateBuffers();
};

namespace ILLIXR_AUDIO{
	// Class Sound
	class Sound{
	public:
		Sound();
		~Sound();

		Sound(std::string srcFile, unsigned nOrder, bool b3D);
		// set sound src position
		void setSrcPos(PolarPoint& pos);
		// set sound amplitude scale
		void setSrcAmp(float ampScale);
		// read sound samples from mono 16bit WAV file and encode into ambisonics format
		CBFormat* readInBFormat();		
	// private:
		// corresponding sound src file
		std::fstream* srcFile;
		// sample buffer HARDCODE
		float sample[BLOCK_SIZE];
		// ambisonics format sound buffer
		CBFormat* BFormat;
		// ambisonics encoder, containing format info, position info, etc.
		CAmbisonicEncoderDist* BEncoder;
		// ambisonics position
		PolarPoint srcPos;
		// amplitude scale to avoid clipping
		float amp;
	};

	// Class ABAudio
	class ABAudio{

	public:
		// Process types
		enum class ProcessType {
			FULL, 			// FULL for output wav file
			ENCODE, 		// For profiling, do file reading and encoding without file output
			DECODE			// For profiling, do ambisonics decoding without file output
		};
		ABAudio(std::string outputFilePath, ProcessType processType);
		~ABAudio();
		// Process a block (1024) samples of sound
		void processBlock();
		// Load sound source files (predefined)
		void loadSource();
	// private:
		ProcessType processType;
		// a list of sound sources in this audio
		std::vector<Sound*>* soundSrcs;
		// target output file
		std::ofstream* outputFile;
		// decoder associated with this audio
		// CAmbisonicBinauralizer* decoder;
        CAmbisonicBinauralizer* decoder0;   // For HPVM
        CAmbisonicBinauralizer* decoder1;   // For HPVM
		// ambisonics rotator associated with this audio
		CAmbisonicProcessor* rotator;
		// ambisonics zoomer associated with this audio
		CAmbisonicZoomer* zoomer;

		// Generate dummy WAV output file header
		void generateWAVHeader();
		// Read in data from WAV files and encode into ambisonics
		void readNEncode(CBFormat& sumBF);
		// Apply rotation and zoom effects to the ambisonics sound field
		void rotateNZoom(CBFormat& sumBF);
		// Write out a block of samples to the output file
		void writeFile(float** resultSample);

		void updateRotation();
		void updateZoom();
	};
}
#endif
