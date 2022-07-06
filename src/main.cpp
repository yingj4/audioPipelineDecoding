#include <audio.h>
#include <iostream>
#include <algorithm>

#include "../libspatialaudio/build/Debug/include/spatialaudio/config.h"
#include "../include/audio.h"
#include "../include/mit_hrtf_lib.h"
#include "../include/sofa_hrtf.h"

#include <cstdlib>
#include <cmath>
#include <vector>

// #define HAVE_MIT_HRTF 1
// #define HAVE_MYSOFA 1

// For MIT_HRTF
MIT_HRTF::MIT_HRTF(unsigned i_sampleRate)
    : HRTF(i_sampleRate)
{
    i_len = mit_hrtf_availability(0, 0, i_sampleRate);
}


bool MIT_HRTF::get(float f_azimuth, float f_elevation, float** pfHRTF)
{
    int nAzimuth = (int)RadiansToDegrees(-f_azimuth);
    if(nAzimuth > 180)
        nAzimuth -= 360;
    int nElevation = (int)RadiansToDegrees(f_elevation);
    //Get HRTFs for given position
    std::vector<short> psHRTF[2] = {std::vector<short>(i_len), std::vector<short>(i_len)};
    unsigned ret = mit_hrtf_get(&nAzimuth, &nElevation, i_sampleRate, psHRTF[0].data(), psHRTF[1].data());
    if (ret == 0)
        return false;

    //Convert from short to float representation
    for (unsigned t = 0; t < i_len; t++)
    {
        pfHRTF[0][t] = psHRTF[0][t] / 32767.f;
        pfHRTF[1][t] = psHRTF[1][t] / 32767.f;
    }

    return true;
}

// For SOFA_HRTF
// SOFA_HRTF::SOFA_HRTF(std::string path, unsigned i_sampleRate)
//     : HRTF(i_sampleRate), hrtf(nullptr)
// {
//     int err;

//     hrtf = mysofa_open(path.c_str(), i_sampleRate, &i_internalLength, &err);
//     if (hrtf == nullptr)
//     {
//         std::cout << "Could not load the SOFA HRTF." << std::endl;
//         return;
//     }

//     i_filterExtraLength = i_internalLength / 2;
//     i_len = i_internalLength + i_filterExtraLength;
// }


// SOFA_HRTF::~SOFA_HRTF()
// {
//     if (hrtf != nullptr)
//         mysofa_close(hrtf);
// }


// bool SOFA_HRTF::get(float f_azimuth, float f_elevation, float** pfHRTF)
// {
//     float delaysSec[2]; // unit is second.
//     unsigned delaysSamples[2]; // unit is samples.
//     std::vector<float> pfHRTFNotDelayed[2];
//     pfHRTFNotDelayed[0].resize( i_internalLength, 0.f );
//     pfHRTFNotDelayed[1].resize( i_internalLength, 0.f );

//     float p[3] = {RadiansToDegrees(f_azimuth), RadiansToDegrees(f_elevation), 1.f};
//     mysofa_s2c(p);

//     mysofa_getfilter_float(hrtf, p[0], p[1], p[2],
//         pfHRTFNotDelayed[0].data(), pfHRTFNotDelayed[1].data(), &delaysSec[0], &delaysSec[1]);
//     delaysSamples[0] = std::roundf(delaysSec[0] * i_sampleRate);
//     delaysSamples[1] = std::roundf(delaysSec[1] * i_sampleRate);

//     if (delaysSamples[0] > i_filterExtraLength
//         || delaysSamples[1] > i_filterExtraLength)
//     {
//         std::cout << "Too big HRTF delay for the buffer length." << std::endl;
//         return false;
//     }

//     std::fill(pfHRTF[0], pfHRTF[0] + i_len, 0);
//     std::fill(pfHRTF[1], pfHRTF[1] + i_len, 0);

//     std::copy(pfHRTFNotDelayed[0].begin(), pfHRTFNotDelayed[0].end(), pfHRTF[0] + delaysSamples[0]);
//     std::copy(pfHRTFNotDelayed[1].begin(), pfHRTFNotDelayed[1].end(), pfHRTF[1] + delaysSamples[1]);

//     return true;
// }

// For CAmbisonicBase
CAmbisonicBase::CAmbisonicBase()
    : m_nOrder(0)
    , m_b3D(0)
    , m_nChannelCount(0)
    , m_bOpt(0)
{
}

unsigned CAmbisonicBase::GetOrder()
{
    return m_nOrder;
}

bool CAmbisonicBase::GetHeight()
{
    return m_b3D;
}

unsigned CAmbisonicBase::GetChannelCount()
{
    return m_nChannelCount;
}

bool CAmbisonicBase::Configure(unsigned nOrder, bool b3D, unsigned nMisc)
{
    m_nOrder = nOrder;
    m_b3D = b3D;
    m_nChannelCount = OrderToComponents(m_nOrder, m_b3D);

    return true;
}

// For CAmbisonicSource
#define fSqrt32 sqrt(3.f)/2.f
#define fSqrt58 sqrt(5.f/8.f)
#define fSqrt152 sqrt(15.f)/2.f
#define fSqrt38 sqrt(3.f/8.f)

CAmbisonicSource::CAmbisonicSource()
{
    m_polPosition.fAzimuth = 0.f;
    m_polPosition.fElevation = 0.f;
    m_polPosition.fDistance = 1.f;
    m_fGain = 1.f;
}

bool CAmbisonicSource::Configure(unsigned nOrder, bool b3D, unsigned nMisc)
{
    bool success = CAmbisonicBase::Configure(nOrder, b3D, nMisc);
    if(!success)
        return false;

    m_pfCoeff.resize( m_nChannelCount, 0 );
    // for a Basic Ambisonics decoder all of the gains are set to 1.f
    m_pfOrderWeights.resize( m_nOrder + 1, 1.f );

    return true;
}

void CAmbisonicSource::Reset()
{
    //memset(m_pfCoeff, 0, m_nChannelCount * sizeof(float));
}

void CAmbisonicSource::Refresh()
{
    float fCosAzim = cosf(m_polPosition.fAzimuth);
    float fSinAzim = sinf(m_polPosition.fAzimuth);
    float fCosElev = cosf(m_polPosition.fElevation);
    float fSinElev = sinf(m_polPosition.fElevation);

    float fCos2Azim = cosf(2.f * m_polPosition.fAzimuth);
    float fSin2Azim = sinf(2.f * m_polPosition.fAzimuth);
    float fSin2Elev = sinf(2.f * m_polPosition.fElevation);

    if(m_b3D)
    {
        // Uses ACN channel ordering and SN3D normalization scheme (AmbiX format)
        if(m_nOrder >= 0)
        {
            m_pfCoeff[0] = 1.f * m_pfOrderWeights[0]; // W
        }
        if(m_nOrder >= 1)
        {
            m_pfCoeff[1] = (fSinAzim * fCosElev) * m_pfOrderWeights[1]; // Y
            m_pfCoeff[2] = (fSinElev) * m_pfOrderWeights[1]; // Z
            m_pfCoeff[3] = (fCosAzim * fCosElev) * m_pfOrderWeights[1]; // X
        }
        if(m_nOrder >= 2)
        {
            m_pfCoeff[4] = fSqrt32*(fSin2Azim * powf(fCosElev, 2)) * m_pfOrderWeights[2]; // V
            m_pfCoeff[5] = fSqrt32*(fSinAzim * fSin2Elev) * m_pfOrderWeights[2]; // T
            m_pfCoeff[6] = (1.5f * powf(fSinElev, 2.f) - 0.5f) * m_pfOrderWeights[2]; // R
            m_pfCoeff[7] = fSqrt32*(fCosAzim * fSin2Elev) * m_pfOrderWeights[2]; // S
            m_pfCoeff[8] = fSqrt32*(fCos2Azim * powf(fCosElev, 2)) * m_pfOrderWeights[2]; // U
        }
        if(m_nOrder >= 3)
        {
            m_pfCoeff[9] = fSqrt58*(sinf(3.f * m_polPosition.fAzimuth) * powf(fCosElev, 3.f)) * m_pfOrderWeights[3]; // Q
            m_pfCoeff[10] = fSqrt152*(fSin2Azim * fSinElev * powf(fCosElev, 2.f)) * m_pfOrderWeights[3]; // O
            m_pfCoeff[11] = fSqrt38*(fSinAzim * fCosElev * (5.f * powf(fSinElev, 2.f) - 1.f)) * m_pfOrderWeights[3]; // M
            m_pfCoeff[12] = (fSinElev * (5.f * powf(fSinElev, 2.f) - 3.f) * 0.5f) * m_pfOrderWeights[3]; // K
            m_pfCoeff[13] = fSqrt38*(fCosAzim * fCosElev * (5.f * powf(fSinElev, 2.f) - 1.f)) * m_pfOrderWeights[3]; // L
            m_pfCoeff[14] = fSqrt152*(fCos2Azim * fSinElev * powf(fCosElev, 2.f)) * m_pfOrderWeights[3]; // N
            m_pfCoeff[15] = fSqrt58*(cosf(3.f * m_polPosition.fAzimuth) * powf(fCosElev, 3.f)) * m_pfOrderWeights[3]; // P

        }
    }
    else
    {
        if(m_nOrder >= 0)
        {
            m_pfCoeff[0] = m_pfOrderWeights[0];
        }
        if(m_nOrder >= 1)
        {
            m_pfCoeff[1] = (fCosAzim * fCosElev) * m_pfOrderWeights[1];
            m_pfCoeff[2] = (fSinAzim * fCosElev) * m_pfOrderWeights[1];
        }
        if(m_nOrder >= 2)
        {
            m_pfCoeff[3] = (fCos2Azim * powf(fCosElev, 2)) * m_pfOrderWeights[2];
            m_pfCoeff[4] = (fSin2Azim * powf(fCosElev, 2)) * m_pfOrderWeights[2];
        }
        if(m_nOrder >= 3)
        {
            m_pfCoeff[5] = (cosf(3.f * m_polPosition.fAzimuth) * powf(fCosElev, 3.f)) * m_pfOrderWeights[3];
            m_pfCoeff[6] = (sinf(3.f * m_polPosition.fAzimuth) * powf(fCosElev, 3.f)) * m_pfOrderWeights[3];
        }
    }

    for(unsigned ni = 0; ni < m_nChannelCount; ni++)
        m_pfCoeff[ni] *= m_fGain;
}

void CAmbisonicSource::SetPosition(PolarPoint polPosition)
{
    m_polPosition = polPosition;
}

PolarPoint CAmbisonicSource::GetPosition()
{
    return m_polPosition;
}

void CAmbisonicSource::SetOrderWeight(unsigned nOrder, float fWeight)
{
    m_pfOrderWeights[nOrder] = fWeight;
}

void CAmbisonicSource::SetOrderWeightAll(float fWeight)
{
    for(unsigned niOrder = 0; niOrder < m_nOrder + 1; niOrder++)
    {
        m_pfOrderWeights[niOrder] = fWeight;
    }
}

void CAmbisonicSource::SetCoefficient(unsigned nChannel, float fCoeff)
{
    m_pfCoeff[nChannel] = fCoeff;
}

float CAmbisonicSource::GetOrderWeight(unsigned nOrder)
{
    return m_pfOrderWeights[nOrder];
}

float CAmbisonicSource::GetCoefficient(unsigned nChannel)
{
    return m_pfCoeff[nChannel];
}

void CAmbisonicSource::SetGain(float fGain)
{
    m_fGain = fGain;
}

float CAmbisonicSource::GetGain()
{
    return m_fGain;
}

// For CBFormat
CBFormat::CBFormat()
{
    m_nSamples = 0;
    m_nDataLength = 0;
}

unsigned CBFormat::GetSampleCount()
{
    return m_nSamples;
}

bool CBFormat::Configure(unsigned nOrder, bool b3D, unsigned nSampleCount)
{
    bool success = CAmbisonicBase::Configure(nOrder, b3D, nSampleCount);
    if(!success)
        return false;

    m_nSamples = nSampleCount;
    m_nDataLength = m_nSamples * m_nChannelCount;

    m_pfData.resize(m_nDataLength);
    memset(m_pfData.data(), 0, m_nDataLength * sizeof(float));
    m_ppfChannels.reset(new float*[m_nChannelCount]);

    for(unsigned niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        m_ppfChannels[niChannel] = &m_pfData[niChannel * m_nSamples];
    }

    return true;
}

void CBFormat::Reset()
{
    memset(m_pfData.data(), 0, m_nDataLength * sizeof(float));
}

void CBFormat::Refresh()
{

}

void CBFormat::InsertStream(float* pfData, unsigned nChannel, unsigned nSamples)
{
    memcpy(m_ppfChannels[nChannel], pfData, nSamples * sizeof(float));
}

void CBFormat::ExtractStream(float* pfData, unsigned nChannel, unsigned nSamples)
{
    memcpy(pfData, m_ppfChannels[nChannel], nSamples * sizeof(float));
}

void CBFormat::operator = (const CBFormat &bf)
{
    memcpy(m_pfData.data(), bf.m_pfData.data(), m_nDataLength * sizeof(float));
}

bool CBFormat::operator == (const CBFormat &bf)
{
    if(m_b3D == bf.m_b3D && m_nOrder == bf.m_nOrder && m_nDataLength == bf.m_nDataLength)
        return true;
    else
        return false;
}

bool CBFormat::operator != (const CBFormat &bf)
{
    if(m_b3D != bf.m_b3D || m_nOrder != bf.m_nOrder || m_nDataLength != bf.m_nDataLength)
        return true;
    else
        return false;
}

CBFormat& CBFormat::operator += (const CBFormat &bf)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] += bf.m_ppfChannels[niChannel][niSample];
        }
    }

    return *this;
}

CBFormat& CBFormat::operator -= (const CBFormat &bf)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] -= bf.m_ppfChannels[niChannel][niSample];
        }
    }

    return *this;
}

CBFormat& CBFormat::operator *= (const CBFormat &bf)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] *= bf.m_ppfChannels[niChannel][niSample];
        }
    }

    return *this;
}

CBFormat& CBFormat::operator /= (const CBFormat &bf)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] /= bf.m_ppfChannels[niChannel][niSample];
        }
    }

    return *this;
}

CBFormat& CBFormat::operator += (const float &fValue)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] += fValue;
        }
    }

    return *this;
}

CBFormat& CBFormat::operator -= (const float &fValue)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] -= fValue;
        }
    }

    return *this;
}

CBFormat& CBFormat::operator *= (const float &fValue)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] *= fValue;
        }
    }

    return *this;
}

CBFormat& CBFormat::operator /= (const float &fValue)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < m_nSamples; niSample++)
        {
            m_ppfChannels[niChannel][niSample] /= fValue;
        }
    }

    return *this;
}

// For AmbisonicEncoder
CAmbisonicEncoder::CAmbisonicEncoder()
{ }

CAmbisonicEncoder::~CAmbisonicEncoder()
{ }

bool CAmbisonicEncoder::Configure(unsigned nOrder, bool b3D, unsigned nMisc)
{
    bool success = CAmbisonicSource::Configure(nOrder, b3D, nMisc);
    if(!success)
        return false;
    //SetOrderWeight(0, 1.f / sqrtf(2.f)); // Removed as seems to break SN3D normalisation
    
    return true;
}

void CAmbisonicEncoder::Refresh()
{
    CAmbisonicSource::Refresh();
}

void CAmbisonicEncoder::Process(float* pfSrc, unsigned nSamples, CBFormat* pfDst)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSample = 0; niSample < nSamples; niSample++)
        {
            pfDst->m_ppfChannels[niChannel][niSample] = pfSrc[niSample] * m_pfCoeff[niChannel];
        }
    }
}

// For CAmbisonicEncoderDist
CAmbisonicEncoderDist::CAmbisonicEncoderDist()
{
    m_nSampleRate = 0;
    m_fDelay = 0.f;
    m_nDelay = 0;
    m_nDelayBufferLength = 0;
    m_pfDelayBuffer = 0;
    m_nIn = 0;
    m_nOutA = 0;
    m_nOutB = 0;
    m_fRoomRadius = 5.f;
    m_fInteriorGain = 0.f;
    m_fExteriorGain = 0.f;

    Configure(DEFAULT_ORDER, DEFAULT_HEIGHT, DEFAULT_SAMPLERATE);
}

CAmbisonicEncoderDist::~CAmbisonicEncoderDist()
{
    if(m_pfDelayBuffer)
        delete [] m_pfDelayBuffer;
}

bool CAmbisonicEncoderDist::Configure(unsigned nOrder, bool b3D, unsigned nSampleRate)
{
    bool success = CAmbisonicEncoder::Configure(nOrder, b3D, 0);
    if(!success)
        return false;
    m_nSampleRate = nSampleRate;
    m_nDelayBufferLength = (unsigned)((float)knMaxDistance / knSpeedOfSound * m_nSampleRate + 0.5f);
    if(m_pfDelayBuffer)
        delete [] m_pfDelayBuffer;
    m_pfDelayBuffer = new float[m_nDelayBufferLength];
    Reset();
    
    return true;
}

void CAmbisonicEncoderDist::Reset()
{
    memset(m_pfDelayBuffer, 0, m_nDelayBufferLength * sizeof(float));
    m_fDelay = m_polPosition.fDistance / knSpeedOfSound * m_nSampleRate + 0.5f;
    m_nDelay = (int)m_fDelay;
    m_fDelay -= m_nDelay;
    m_nIn = 0;
    m_nOutA = (m_nIn - m_nDelay + m_nDelayBufferLength) % m_nDelayBufferLength;
    m_nOutB = (m_nOutA + 1) % m_nDelayBufferLength;
}

void CAmbisonicEncoderDist::Refresh()
{
    CAmbisonicEncoder::Refresh();

    m_fDelay = fabs(m_polPosition.fDistance) / knSpeedOfSound * m_nSampleRate; //TODO abs() sees float as int!
    m_nDelay = (int)m_fDelay;
    m_fDelay -= m_nDelay;
    m_nOutA = (m_nIn - m_nDelay + m_nDelayBufferLength) % m_nDelayBufferLength;
    m_nOutB = (m_nOutA + 1) % m_nDelayBufferLength;

    //Source is outside speaker array
    if(fabs(m_polPosition.fDistance) >= m_fRoomRadius)
    {
        m_fInteriorGain    = (m_fRoomRadius / fabs(m_polPosition.fDistance)) / 2.f;
        m_fExteriorGain    = m_fInteriorGain;
    }
    else
    {
        m_fInteriorGain = (2.f - fabs(m_polPosition.fDistance) / m_fRoomRadius) / 2.f;
        m_fExteriorGain = (fabs(m_polPosition.fDistance) / m_fRoomRadius) / 2.f;
    }
}

void CAmbisonicEncoderDist::Process(float* pfSrc, unsigned nSamples, CBFormat* pfDst)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    float fSrcSample = 0;

    for(niSample = 0; niSample < nSamples; niSample++)
    {
        //Store
        m_pfDelayBuffer[m_nIn] = pfSrc[niSample];
        //Read
        fSrcSample = m_pfDelayBuffer[m_nOutA] * (1.f - m_fDelay)
                    + m_pfDelayBuffer[m_nOutB] * m_fDelay;

        pfDst->m_ppfChannels[kW][niSample] = fSrcSample * m_fInteriorGain * m_pfCoeff[kW];

        fSrcSample *= m_fExteriorGain;
        for(niChannel = 1; niChannel < m_nChannelCount; niChannel++)
        {
            pfDst->m_ppfChannels[niChannel][niSample] = fSrcSample * m_pfCoeff[niChannel];
        }

        m_nIn = (m_nIn + 1) % m_nDelayBufferLength;
        m_nOutA = (m_nOutA + 1) % m_nDelayBufferLength;
        m_nOutB = (m_nOutB + 1) % m_nDelayBufferLength;
    }
}

void CAmbisonicEncoderDist::SetRoomRadius(float fRoomRadius)
{
    m_fRoomRadius = fRoomRadius;
}

float CAmbisonicEncoderDist::GetRoomRadius()
{
    return m_fRoomRadius;
}

// For CAmbisonicProcessor
CAmbisonicProcessor::CAmbisonicProcessor()
    : m_orientation(0, 0, 0)
{
    m_pfTempSample = nullptr;
    m_pfScratchBufferA = nullptr;
    m_pFFT_psych_cfg = nullptr;
    m_pIFFT_psych_cfg = nullptr;
    m_ppcpPsychFilters = nullptr;
    m_pcpScratch = nullptr;
    m_pfOverlap = nullptr;
}

CAmbisonicProcessor::~CAmbisonicProcessor() {
    if(m_pfTempSample)
        delete [] m_pfTempSample;
    if(m_pfScratchBufferA)
        delete [] m_pfScratchBufferA;
    if(m_pFFT_psych_cfg)
        kiss_fftr_free(m_pFFT_psych_cfg);
    if(m_pIFFT_psych_cfg)
        kiss_fftr_free(m_pIFFT_psych_cfg);
    if (m_ppcpPsychFilters)
    {
        for(unsigned i=0; i<=m_nOrder; i++)
            if(m_ppcpPsychFilters[i])
                delete [] m_ppcpPsychFilters[i];
        delete [] m_ppcpPsychFilters;
    }
    if(m_pcpScratch)
        delete [] m_pcpScratch;
    if(m_pfOverlap){
        for(unsigned i=0; i<m_nChannelCount; i++)
            if(m_pfOverlap[i])
                delete [] m_pfOverlap[i];
        delete [] m_pfOverlap;
    }
}

bool CAmbisonicProcessor::Configure(unsigned nOrder, bool b3D, unsigned nBlockSize, unsigned nMisc) {
    bool success = CAmbisonicBase::Configure(nOrder, b3D, nMisc);
    if(!success)
        return false;
    if(m_pfTempSample)
        delete [] m_pfTempSample;
    m_pfTempSample = new float[m_nChannelCount];
    memset(m_pfTempSample, 0, m_nChannelCount * sizeof(float));

    /* This bool should be set as a user option to turn optimisation on and off*/
    m_bOpt = true;

    // All optimisation filters have the same number of taps so take from the first order 3D impulse response arbitrarily
    unsigned nbTaps = sizeof(first_order_3D[0]) / sizeof(int16_t);

    m_nBlockSize = nBlockSize;
    m_nTaps = nbTaps;

    //What will the overlap size be?
    m_nOverlapLength = m_nBlockSize < m_nTaps ? m_nBlockSize - 1 : m_nTaps - 1;
    //How large does the FFT need to be
    m_nFFTSize = 1;
    while(m_nFFTSize < (m_nBlockSize + m_nTaps + m_nOverlapLength))
        m_nFFTSize <<= 1;
    //How many bins is that
    m_nFFTBins = m_nFFTSize / 2 + 1;
    //What do we need to scale the result of the iFFT by
    m_fFFTScaler = 1.f / m_nFFTSize;

    //Allocate buffers
        m_pfOverlap = new float*[m_nChannelCount];
    for(unsigned i=0; i<m_nChannelCount; i++)
        m_pfOverlap[i] = new float[m_nOverlapLength];

    m_pfScratchBufferA = new float[m_nFFTSize];
    m_ppcpPsychFilters = new kiss_fft_cpx*[m_nOrder+1];
    for(unsigned i = 0; i <= m_nOrder; i++)
        m_ppcpPsychFilters[i] = new kiss_fft_cpx[m_nFFTBins];

    m_pcpScratch = new kiss_fft_cpx[m_nFFTBins];

    //Allocate temporary buffers for retrieving taps of psychoacoustic opimisation filters
    std::vector<std::unique_ptr<float[]>> pfPsychIR;
    for(unsigned i = 0; i <= m_nOrder; i++)
    {
        pfPsychIR.emplace_back(new float[m_nTaps]);
    }

    Reset();

    //Allocate FFT and iFFT for new size
    m_pFFT_psych_cfg = kiss_fftr_alloc(m_nFFTSize, 0, 0, 0);
    m_pIFFT_psych_cfg = kiss_fftr_alloc(m_nFFTSize, 1, 0, 0);

    // get impulse responses for psychoacoustic optimisation based on playback system (2D or 3D) and playback order (1 to 3)
    //Convert from short to float representation
    for (unsigned i_m = 0; i_m <= m_nOrder; i_m++){
        for(unsigned i = 0; i < m_nTaps; i++)
            if(m_b3D){
                switch(m_nOrder){
                    case 1: pfPsychIR[i_m][i] = 2.f*first_order_3D[i_m][i] / 32767.f; break;
                    case 2: pfPsychIR[i_m][i] = 2.f*second_order_3D[i_m][i] / 32767.f; break;
                    case 3: pfPsychIR[i_m][i] = 2.f*third_order_3D[i_m][i] / 32767.f; break;
                }
                }
                else{
                    switch(m_nOrder){
                    case 1: pfPsychIR[i_m][i] = 2.f*first_order_2D[i_m][i] / 32767.f; break;
                    case 2: pfPsychIR[i_m][i] = 2.f*second_order_2D[i_m][i] / 32767.f; break;
                    case 3: pfPsychIR[i_m][i] = 2.f*third_order_2D[i_m][i] / 32767.f; break;
                }
            }
        // Convert the impulse responses to the frequency domain
        memcpy(m_pfScratchBufferA, pfPsychIR[i_m].get(), m_nTaps * sizeof(float));
        memset(&m_pfScratchBufferA[m_nTaps], 0, (m_nFFTSize - m_nTaps) * sizeof(float));
        kiss_fftr(m_pFFT_psych_cfg, m_pfScratchBufferA, m_ppcpPsychFilters[i_m]);
    }

    return true;
}

void CAmbisonicProcessor::Reset() {
    for(unsigned i=0; i<m_nChannelCount; i++)
        memset(m_pfOverlap[i], 0, m_nOverlapLength * sizeof(float));
}

void CAmbisonicProcessor::Refresh() {
    // Trig terms used multiple times in rotation equations
    m_fCosAlpha = cosf(m_orientation.fAlpha);
    m_fSinAlpha = sinf(m_orientation.fAlpha);
    m_fCosBeta = cosf(m_orientation.fBeta);
    m_fSinBeta = sinf(m_orientation.fBeta);
    m_fCosGamma = cosf(m_orientation.fGamma);
    m_fSinGamma = sinf(m_orientation.fGamma);
    m_fCos2Alpha = cosf(2.f * m_orientation.fAlpha);
    m_fSin2Alpha = sinf(2.f * m_orientation.fAlpha);
    m_fCos2Beta = cosf(2.f * m_orientation.fBeta);
    m_fSin2Beta = sinf(2.f * m_orientation.fBeta);
    m_fCos2Gamma = cosf(2.f * m_orientation.fGamma);
    m_fSin2Gamma = sinf(2.f * m_orientation.fGamma);
    m_fCos3Alpha = cosf(3.f * m_orientation.fAlpha);
    m_fSin3Alpha = sinf(3.f * m_orientation.fAlpha);
    m_fCos3Beta = cosf(3.f * m_orientation.fBeta);
    m_fSin3Beta = sinf(3.f * m_orientation.fBeta);
    m_fCos3Gamma = cosf(3.f * m_orientation.fGamma);
    m_fSin3Gamma = sinf(3.f * m_orientation.fGamma);
}

void CAmbisonicProcessor::SetOrientation(Orientation orientation)
{
    m_orientation = orientation;
}

Orientation CAmbisonicProcessor::GetOrientation()
{
    return m_orientation;
}

void CAmbisonicProcessor::Process(CBFormat* pBFSrcDst, unsigned nSamples)
{

    /* Rotate the sound scene based on the rotation angle from the 360 video*/
    /* Before the rotation we apply the psychoacoustic optimisation filters */
    if(m_bOpt)
    {
        ShelfFilterOrder(pBFSrcDst, nSamples);
    }
    else
    {
        // No filtering required
    }

    /* 3D Ambisonics input expected so perform 3D rotations */
    if(m_nOrder >= 1)
        ProcessOrder1_3D(pBFSrcDst, nSamples);
    if(m_nOrder >= 2)
        ProcessOrder2_3D(pBFSrcDst, nSamples);
    if(m_nOrder >= 3)
        ProcessOrder3_3D(pBFSrcDst, nSamples);
}

void CAmbisonicProcessor::ProcessOrder1_3D(CBFormat* pBFSrcDst, unsigned nSamples)
{
    /* Rotations are performed in the following order:
        1 - rotation around the z-axis
        2 - rotation around the *new* y-axis (y')
        3 - rotation around the new z-axis (z'')
    This is different to the rotations obtained from the video, which are around z, y' then x''.
    The rotation equations used here work for third order. However, for higher orders a recursive algorithm
    should be considered.*/
    for(unsigned niSample = 0; niSample < nSamples; niSample++)
    {
        // Alpha rotation
        m_pfTempSample[kY] = -pBFSrcDst->m_ppfChannels[kX][niSample] * m_fSinAlpha
                            + pBFSrcDst->m_ppfChannels[kY][niSample] * m_fCosAlpha;
        m_pfTempSample[kZ] = pBFSrcDst->m_ppfChannels[kZ][niSample];
        m_pfTempSample[kX] = pBFSrcDst->m_ppfChannels[kX][niSample] * m_fCosAlpha
                            + pBFSrcDst->m_ppfChannels[kY][niSample] * m_fSinAlpha;

        // Beta rotation
        pBFSrcDst->m_ppfChannels[kY][niSample] = m_pfTempSample[kY];
        pBFSrcDst->m_ppfChannels[kZ][niSample] = m_pfTempSample[kZ] * m_fCosBeta
                            +  m_pfTempSample[kX] * m_fSinBeta;
        pBFSrcDst->m_ppfChannels[kX][niSample] = m_pfTempSample[kX] * m_fCosBeta
                            - m_pfTempSample[kZ] * m_fSinBeta;

        // Gamma rotation
        m_pfTempSample[kY] = -pBFSrcDst->m_ppfChannels[kX][niSample] * m_fSinGamma
                            + pBFSrcDst->m_ppfChannels[kY][niSample] * m_fCosGamma;
        m_pfTempSample[kZ] = pBFSrcDst->m_ppfChannels[kZ][niSample];
        m_pfTempSample[kX] = pBFSrcDst->m_ppfChannels[kX][niSample] * m_fCosGamma
                            + pBFSrcDst->m_ppfChannels[kY][niSample] * m_fSinGamma;

        pBFSrcDst->m_ppfChannels[kX][niSample] = m_pfTempSample[kX];
        pBFSrcDst->m_ppfChannels[kY][niSample] = m_pfTempSample[kY];
        pBFSrcDst->m_ppfChannels[kZ][niSample] = m_pfTempSample[kZ];
    }
}

void CAmbisonicProcessor::ProcessOrder2_3D(CBFormat* pBFSrcDst, unsigned nSamples)
{
    float fSqrt3 = sqrt(3.f);

    for(unsigned niSample = 0; niSample < nSamples; niSample++)
    {
        // Alpha rotation
        m_pfTempSample[kV] = - pBFSrcDst->m_ppfChannels[kU][niSample] * m_fSin2Alpha
                            + pBFSrcDst->m_ppfChannels[kV][niSample] * m_fCos2Alpha;
        m_pfTempSample[kT] = - pBFSrcDst->m_ppfChannels[kS][niSample] * m_fSinAlpha
                            + pBFSrcDst->m_ppfChannels[kT][niSample] * m_fCosAlpha;
        m_pfTempSample[kR] = pBFSrcDst->m_ppfChannels[kR][niSample];
        m_pfTempSample[kS] = pBFSrcDst->m_ppfChannels[kS][niSample] * m_fCosAlpha
                            + pBFSrcDst->m_ppfChannels[kT][niSample] * m_fSinAlpha;
        m_pfTempSample[kU] = pBFSrcDst->m_ppfChannels[kU][niSample] * m_fCos2Alpha
                            + pBFSrcDst->m_ppfChannels[kV][niSample] * m_fSin2Alpha;

        // Beta rotation
        pBFSrcDst->m_ppfChannels[kV][niSample] = -m_fSinBeta * m_pfTempSample[kT]
                                        + m_fCosBeta * m_pfTempSample[kV];
        pBFSrcDst->m_ppfChannels[kT][niSample] = -m_fCosBeta * m_pfTempSample[kT]
                                        + m_fSinBeta * m_pfTempSample[kV];
        pBFSrcDst->m_ppfChannels[kR][niSample] = (0.75f * m_fCos2Beta + 0.25f) * m_pfTempSample[kR]
                            + (0.5 * fSqrt3 * pow(m_fSinBeta,2.0) ) * m_pfTempSample[kU]
                            + (fSqrt3 * m_fSinBeta * m_fCosBeta) * m_pfTempSample[kS];
        pBFSrcDst->m_ppfChannels[kS][niSample] = m_fCos2Beta * m_pfTempSample[kS]
                            - fSqrt3 * m_fCosBeta * m_fSinBeta * m_pfTempSample[kR]
                            + m_fCosBeta * m_fSinBeta * m_pfTempSample[kU];
        pBFSrcDst->m_ppfChannels[kU][niSample] = (0.25f * m_fCos2Beta + 0.75f) * m_pfTempSample[kU]
                            - m_fCosBeta * m_fSinBeta * m_pfTempSample[kS]
                            +0.5 * fSqrt3 * pow(m_fSinBeta,2.0) * m_pfTempSample[kR];

        // Gamma rotation
        m_pfTempSample[kV] = - pBFSrcDst->m_ppfChannels[kU][niSample] * m_fSin2Gamma
                            + pBFSrcDst->m_ppfChannels[kV][niSample] * m_fCos2Gamma;
        m_pfTempSample[kT] = - pBFSrcDst->m_ppfChannels[kS][niSample] * m_fSinGamma
                            + pBFSrcDst->m_ppfChannels[kT][niSample] * m_fCosGamma;

        m_pfTempSample[kR] = pBFSrcDst->m_ppfChannels[kR][niSample];
        m_pfTempSample[kS] = pBFSrcDst->m_ppfChannels[kS][niSample] * m_fCosGamma
                            + pBFSrcDst->m_ppfChannels[kT][niSample] * m_fSinGamma;
        m_pfTempSample[kU] = pBFSrcDst->m_ppfChannels[kU][niSample] * m_fCos2Gamma
                            + pBFSrcDst->m_ppfChannels[kV][niSample] * m_fSin2Gamma;

        pBFSrcDst->m_ppfChannels[kR][niSample] = m_pfTempSample[kR];
        pBFSrcDst->m_ppfChannels[kS][niSample] = m_pfTempSample[kS];
        pBFSrcDst->m_ppfChannels[kT][niSample] = m_pfTempSample[kT];
        pBFSrcDst->m_ppfChannels[kU][niSample] = m_pfTempSample[kU];
        pBFSrcDst->m_ppfChannels[kV][niSample] = m_pfTempSample[kV];
    }
}

void CAmbisonicProcessor::ProcessOrder3_3D(CBFormat* pBFSrcDst, unsigned nSamples)
{
        /* (should move these somewhere that does recompute each time) */
        float fSqrt3_2 = sqrt(3.f/2.f);
        float fSqrt15 = sqrt(15.f);
        float fSqrt5_2 = sqrt(5.f/2.f);

    for(unsigned niSample = 0; niSample < nSamples; niSample++)
    {
        // Alpha rotation
        m_pfTempSample[kQ] = - pBFSrcDst->m_ppfChannels[kP][niSample] * m_fSin3Alpha
                            + pBFSrcDst->m_ppfChannels[kQ][niSample] * m_fCos3Alpha;
        m_pfTempSample[kO] = - pBFSrcDst->m_ppfChannels[kN][niSample] * m_fSin2Alpha
                            + pBFSrcDst->m_ppfChannels[kO][niSample] * m_fCos2Alpha;
        m_pfTempSample[kM] = - pBFSrcDst->m_ppfChannels[kL][niSample] * m_fSinAlpha
                            + pBFSrcDst->m_ppfChannels[kM][niSample] * m_fCosAlpha;
        m_pfTempSample[kK] = pBFSrcDst->m_ppfChannels[kK][niSample];
        m_pfTempSample[kL] = pBFSrcDst->m_ppfChannels[kL][niSample] * m_fCosAlpha
                            + pBFSrcDst->m_ppfChannels[kM][niSample] * m_fSinAlpha;
        m_pfTempSample[kN] = pBFSrcDst->m_ppfChannels[kN][niSample] * m_fCos2Alpha
                            + pBFSrcDst->m_ppfChannels[kO][niSample] * m_fSin2Alpha;
        m_pfTempSample[kP] = pBFSrcDst->m_ppfChannels[kP][niSample] * m_fCos3Alpha
                            + pBFSrcDst->m_ppfChannels[kQ][niSample] * m_fSin3Alpha;

        // Beta rotation
        pBFSrcDst->m_ppfChannels[kQ][niSample] = 0.125f * m_pfTempSample[kQ] * (5.f + 3.f*m_fCos2Beta)
                    - fSqrt3_2 * m_pfTempSample[kO] *m_fCosBeta * m_fSinBeta
                    + 0.25f * fSqrt15 * m_pfTempSample[kM] * pow(m_fSinBeta,2.0f);
        pBFSrcDst->m_ppfChannels[kO][niSample] = m_pfTempSample[kO] * m_fCos2Beta
                    - fSqrt5_2 * m_pfTempSample[kM] * m_fCosBeta * m_fSinBeta
                    + fSqrt3_2 * m_pfTempSample[kQ] * m_fCosBeta * m_fSinBeta;
        pBFSrcDst->m_ppfChannels[kM][niSample] = 0.125f * m_pfTempSample[kM] * (3.f + 5.f*m_fCos2Beta)
                    - fSqrt5_2 * m_pfTempSample[kO] *m_fCosBeta * m_fSinBeta
                    + 0.25f * fSqrt15 * m_pfTempSample[kQ] * pow(m_fSinBeta,2.0f);
        pBFSrcDst->m_ppfChannels[kK][niSample] = 0.25f * m_pfTempSample[kK] * m_fCosBeta * (-1.f + 15.f*m_fCos2Beta)
                    + 0.5f * fSqrt15 * m_pfTempSample[kN] * m_fCosBeta * pow(m_fSinBeta,2.f)
                    + 0.5f * fSqrt5_2 * m_pfTempSample[kP] * pow(m_fSinBeta,3.f)
                    + 0.125f * fSqrt3_2 * m_pfTempSample[kL] * (m_fSinBeta + 5.f * m_fSin3Beta);
        pBFSrcDst->m_ppfChannels[kL][niSample] = 0.0625f * m_pfTempSample[kL] * (m_fCosBeta + 15.f * m_fCos3Beta)
                    + 0.25f * fSqrt5_2 * m_pfTempSample[kN] * (1.f + 3.f * m_fCos2Beta) * m_fSinBeta
                    + 0.25f * fSqrt15 * m_pfTempSample[kP] * m_fCosBeta * pow(m_fSinBeta,2.f)
                    - 0.125 * fSqrt3_2 * m_pfTempSample[kK] * (m_fSinBeta + 5.f * m_fSin3Beta);
        pBFSrcDst->m_ppfChannels[kN][niSample] = 0.125f * m_pfTempSample[kN] * (5.f * m_fCosBeta + 3.f * m_fCos3Beta)
                    + 0.25f * fSqrt3_2 * m_pfTempSample[kP] * (3.f + m_fCos2Beta) * m_fSinBeta
                    + 0.5f * fSqrt15 * m_pfTempSample[kK] * m_fCosBeta * pow(m_fSinBeta,2.f)
                    + 0.125 * fSqrt5_2 * m_pfTempSample[kL] * (m_fSinBeta - 3.f * m_fSin3Beta);
        pBFSrcDst->m_ppfChannels[kP][niSample] = 0.0625f * m_pfTempSample[kP] * (15.f * m_fCosBeta + m_fCos3Beta)
                    - 0.25f * fSqrt3_2 * m_pfTempSample[kN] * (3.f + m_fCos2Beta) * m_fSinBeta
                    + 0.25f * fSqrt15 * m_pfTempSample[kL] * m_fCosBeta * pow(m_fSinBeta,2.f)
                    - 0.5 * fSqrt5_2 * m_pfTempSample[kK] * pow(m_fSinBeta,3.f);

        // Gamma rotation
        m_pfTempSample[kQ] = - pBFSrcDst->m_ppfChannels[kP][niSample] * m_fSin3Gamma
                            + pBFSrcDst->m_ppfChannels[kQ][niSample] * m_fCos3Gamma;
        m_pfTempSample[kO] = - pBFSrcDst->m_ppfChannels[kN][niSample] * m_fSin2Gamma
                            + pBFSrcDst->m_ppfChannels[kO][niSample] * m_fCos2Gamma;
        m_pfTempSample[kM] = - pBFSrcDst->m_ppfChannels[kL][niSample] * m_fSinGamma
                            + pBFSrcDst->m_ppfChannels[kM][niSample] * m_fCosGamma;
        m_pfTempSample[kK] = pBFSrcDst->m_ppfChannels[kK][niSample];
        m_pfTempSample[kL] = pBFSrcDst->m_ppfChannels[kL][niSample] * m_fCosGamma
                            + pBFSrcDst->m_ppfChannels[kM][niSample] * m_fSinGamma;
        m_pfTempSample[kN] = pBFSrcDst->m_ppfChannels[kN][niSample] * m_fCos2Gamma
                            + pBFSrcDst->m_ppfChannels[kO][niSample] * m_fSin2Gamma;
        m_pfTempSample[kP] = pBFSrcDst->m_ppfChannels[kP][niSample] * m_fCos3Gamma
                            + pBFSrcDst->m_ppfChannels[kQ][niSample] * m_fSin3Gamma;

        pBFSrcDst->m_ppfChannels[kQ][niSample] = m_pfTempSample[kQ];
        pBFSrcDst->m_ppfChannels[kO][niSample] = m_pfTempSample[kO];
        pBFSrcDst->m_ppfChannels[kM][niSample] = m_pfTempSample[kM];
        pBFSrcDst->m_ppfChannels[kK][niSample] = m_pfTempSample[kK];
        pBFSrcDst->m_ppfChannels[kL][niSample] = m_pfTempSample[kL];
        pBFSrcDst->m_ppfChannels[kN][niSample] = m_pfTempSample[kN];
        pBFSrcDst->m_ppfChannels[kP][niSample] = m_pfTempSample[kP];
    }
}

// ACN/SN3D is generally only ever produced for 3D Ambisonics.
// If 2D Ambisonics is required then these equations need to be modified (they can be found in the 3D code for the first Z-rotation).
// Generally, 2D-only rotations do not make sense for use with 360 degree videos.
/*
void CAmbisonicProcessor::ProcessOrder1_2D(CBFormat* pBFSrcDst, unsigned nSamples)
{
    for(unsigned niSample = 0; niSample < nSamples; niSample++)
    {
        //Yaw
        m_pfTempSample[kX] = pBFSrcDst->m_ppfChannels[kX][niSample] * m_fCosYaw
                            - pBFSrcDst->m_ppfChannels[kY][niSample] * m_fSinYaw;
        m_pfTempSample[kY] = pBFSrcDst->m_ppfChannels[kX][niSample] * m_fSinYaw
                            + pBFSrcDst->m_ppfChannels[kY][niSample] * m_fCosYaw;

        pBFSrcDst->m_ppfChannels[kX][niSample] = m_pfTempSample[kX];
        pBFSrcDst->m_ppfChannels[kY][niSample] = m_pfTempSample[kY];
    }
}

void CAmbisonicProcessor::ProcessOrder2_2D(CBFormat* pBFSrcDst, unsigned nSamples)
{
    for(unsigned niSample = 0; niSample < nSamples; niSample++)
    {
        //Yaw
        m_pfTempSample[kS] = pBFSrcDst->m_ppfChannels[kS][niSample] * m_fCosYaw
                            - pBFSrcDst->m_ppfChannels[kT][niSample] * m_fSinYaw;
        m_pfTempSample[kT] = pBFSrcDst->m_ppfChannels[kS][niSample] * m_fSinYaw
                            + pBFSrcDst->m_ppfChannels[kT][niSample] * m_fCosYaw;
        m_pfTempSample[kU] = pBFSrcDst->m_ppfChannels[kU][niSample] * m_fCos2Yaw
                            - pBFSrcDst->m_ppfChannels[kV][niSample] * m_fSin2Yaw;
        m_pfTempSample[kV] = pBFSrcDst->m_ppfChannels[kU][niSample] * m_fSin2Yaw
                            + pBFSrcDst->m_ppfChannels[kV][niSample] * m_fCos2Yaw;

        pBFSrcDst->m_ppfChannels[kS][niSample] = m_pfTempSample[kS];
        pBFSrcDst->m_ppfChannels[kT][niSample] = m_pfTempSample[kT];
        pBFSrcDst->m_ppfChannels[kU][niSample] = m_pfTempSample[kU];
        pBFSrcDst->m_ppfChannels[kV][niSample] = m_pfTempSample[kV];
    }
}

void CAmbisonicProcessor::ProcessOrder3_2D(CBFormat* pBFSrcDst, unsigned nSamples)
{
    //TODO
}
*/

void CAmbisonicProcessor::ShelfFilterOrder(CBFormat* pBFSrcDst, unsigned nSamples)
{
    kiss_fft_cpx cpTemp;

    unsigned iChannelOrder = 0;

    // Filter the Ambisonics channels
    // All  channels are filtered using linear phase FIR filters.
    // In the case of the 0th order signal (W channel) this takes the form of a delay
    // For all other channels shelf filters are used
    memset(m_pfScratchBufferA, 0, m_nFFTSize * sizeof(float));
    for(unsigned niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {

        iChannelOrder = int(sqrt(niChannel));    //get the order of the current channel

        memcpy(m_pfScratchBufferA, pBFSrcDst->m_ppfChannels[niChannel], m_nBlockSize * sizeof(float));
        memset(&m_pfScratchBufferA[m_nBlockSize], 0, (m_nFFTSize - m_nBlockSize) * sizeof(float));
        kiss_fftr(m_pFFT_psych_cfg, m_pfScratchBufferA, m_pcpScratch);
        // Perform the convolution in the frequency domain
        for(unsigned ni = 0; ni < m_nFFTBins; ni++)
        {
            cpTemp.r = m_pcpScratch[ni].r * m_ppcpPsychFilters[iChannelOrder][ni].r
                        - m_pcpScratch[ni].i * m_ppcpPsychFilters[iChannelOrder][ni].i;
            cpTemp.i = m_pcpScratch[ni].r * m_ppcpPsychFilters[iChannelOrder][ni].i
                        + m_pcpScratch[ni].i * m_ppcpPsychFilters[iChannelOrder][ni].r;
            m_pcpScratch[ni] = cpTemp;
        }
        // Convert from frequency domain back to time domain
        kiss_fftri(m_pIFFT_psych_cfg, m_pcpScratch, m_pfScratchBufferA);
        for(unsigned ni = 0; ni < m_nFFTSize; ni++)
            m_pfScratchBufferA[ni] *= m_fFFTScaler;
                memcpy(pBFSrcDst->m_ppfChannels[niChannel], m_pfScratchBufferA, m_nBlockSize * sizeof(float));
        for(unsigned ni = 0; ni < m_nOverlapLength; ni++)
                {
                        pBFSrcDst->m_ppfChannels[niChannel][ni] += m_pfOverlap[niChannel][ni];
                }
                memcpy(m_pfOverlap[niChannel], &m_pfScratchBufferA[m_nBlockSize], m_nOverlapLength * sizeof(float));
    }
}

// For CAmbisonicSpeaker
CAmbisonicSpeaker::CAmbisonicSpeaker()
{ }

CAmbisonicSpeaker::~CAmbisonicSpeaker()
{ }

bool CAmbisonicSpeaker::Configure(unsigned nOrder, bool b3D, unsigned nMisc)
{
    bool success = CAmbisonicSource::Configure(nOrder, b3D, nMisc);
    if(!success)
        return false;
    
    return true;
}

void CAmbisonicSpeaker::Refresh()
{
    CAmbisonicSource::Refresh();
}

void CAmbisonicSpeaker::Process(CBFormat* pBFSrc, unsigned nSamples, float* pfDst)
{
    unsigned niChannel = 0;
    unsigned niSample = 0;
    memset(pfDst, 0, nSamples * sizeof(float));
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        float *in = pBFSrc->m_ppfChannels[niChannel];
        float *out = pfDst;

        if(m_b3D){ /* Decode to a 3D loudspeaker array */
            // The spherical harmonic coefficients are multiplied by (2*order + 1) to provide the correct decoder
            // for SN3D normalised Ambisonic inputs.
            const float coeff = m_pfCoeff[niChannel] * (2*floor(sqrt(niChannel)) + 1);
            for(niSample = 0; niSample < nSamples; niSample++)
                *out++ += (*in++) * coeff;
        }
        else
        {    /* Decode to a 2D loudspeaker array */
            // The spherical harmonic coefficients are multiplied by 2 to provide the correct decoder
            // for SN3D normalised Ambisonic inputs decoded to a horizontal loudspeaker array
            const float coeff = m_pfCoeff[niChannel] * 2.f;
            for(niSample = 0; niSample < nSamples; niSample++)
                *out++ += (*in++) * coeff;
        }
    }
}

// For CAmbisonicDecoder
CAmbisonicDecoder::CAmbisonicDecoder()
{
    m_nSpeakerSetUp = 0;
    m_nSpeakers = 0;
    m_pAmbSpeakers = nullptr;
}

CAmbisonicDecoder::~CAmbisonicDecoder()
{
    if(m_pAmbSpeakers)
        delete [] m_pAmbSpeakers;
}

bool CAmbisonicDecoder::Configure(unsigned nOrder, bool b3D, int nSpeakerSetUp, unsigned nSpeakers)
{
    bool success = CAmbisonicBase::Configure(nOrder, b3D, nSpeakerSetUp);
    if(!success)
        return false;
    SpeakerSetUp(nSpeakerSetUp, nSpeakers);
    Refresh();
    
    return true;
}

void CAmbisonicDecoder::Reset()
{
    for(unsigned niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        m_pAmbSpeakers[niSpeaker].Reset();
}

void CAmbisonicDecoder::Refresh()
{
    for(unsigned niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        m_pAmbSpeakers[niSpeaker].Refresh();
}

void CAmbisonicDecoder::Process(CBFormat* pBFSrc, unsigned nSamples, float** ppfDst)
{
    for(unsigned niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
    {
        m_pAmbSpeakers[niSpeaker].Process(pBFSrc, nSamples, ppfDst[niSpeaker]);
    }
}

int CAmbisonicDecoder::GetSpeakerSetUp()
{
    return m_nSpeakerSetUp;
}

unsigned CAmbisonicDecoder::GetSpeakerCount()
{
    return m_nSpeakers;
}

void CAmbisonicDecoder::SetPosition(unsigned nSpeaker, PolarPoint polPosition)
{
    m_pAmbSpeakers[nSpeaker].SetPosition(polPosition);
}

PolarPoint CAmbisonicDecoder::GetPosition(unsigned nSpeaker)
{
    return m_pAmbSpeakers[nSpeaker].GetPosition();
}

void CAmbisonicDecoder::SetOrderWeight(unsigned nSpeaker, unsigned nOrder, float fWeight)
{
    m_pAmbSpeakers[nSpeaker].SetOrderWeight(nOrder, fWeight);
}

float CAmbisonicDecoder::GetOrderWeight(unsigned nSpeaker, unsigned nOrder)
{
    return m_pAmbSpeakers[nSpeaker].GetOrderWeight(nOrder);
}

float CAmbisonicDecoder::GetCoefficient(unsigned nSpeaker, unsigned nChannel)
{
    return m_pAmbSpeakers[nSpeaker].GetCoefficient(nChannel);
}

void CAmbisonicDecoder::SetCoefficient(unsigned nSpeaker, unsigned nChannel, float fCoeff)
{
    m_pAmbSpeakers[nSpeaker].SetCoefficient(nChannel, fCoeff);
}

void CAmbisonicDecoder::SpeakerSetUp(int nSpeakerSetUp, unsigned nSpeakers)
{
    m_nSpeakerSetUp = nSpeakerSetUp;

    if(m_pAmbSpeakers)
        delete [] m_pAmbSpeakers;

    PolarPoint polPosition = {0.f, 0.f, 1.f};
    unsigned niSpeaker = 0;
    float fSpeakerGain = 0.f;

    switch(m_nSpeakerSetUp)
    {
    case kAmblib_CustomSpeakerSetUp:
        m_nSpeakers = nSpeakers;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
        }
        break;
    case kAmblib_Mono:
        m_nSpeakers = 1;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        m_pAmbSpeakers[0].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[0].SetPosition(polPosition);
        break;
    case kAmblib_Stereo:
        m_nSpeakers = 2;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        polPosition.fAzimuth = DegreesToRadians(30.f);
        m_pAmbSpeakers[0].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[0].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(-30.f);
        m_pAmbSpeakers[1].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[1].SetPosition(polPosition);
        break;
    case kAmblib_LCR:
        m_nSpeakers = 3;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        polPosition.fAzimuth = DegreesToRadians(30.f);
        m_pAmbSpeakers[0].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[0].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(0.f);
        m_pAmbSpeakers[1].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[1].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(-30.f);
        m_pAmbSpeakers[2].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[2].SetPosition(polPosition);
        break;
    case kAmblib_Quad:
        m_nSpeakers = 4;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        polPosition.fAzimuth = DegreesToRadians(45.f);
        m_pAmbSpeakers[0].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[0].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(-45.f);
        m_pAmbSpeakers[1].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[1].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(135.f);
        m_pAmbSpeakers[2].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[2].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(-135.f);
        m_pAmbSpeakers[3].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[3].SetPosition(polPosition);
        break;
    case kAmblib_50:
        m_nSpeakers = 5;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        polPosition.fAzimuth = DegreesToRadians(30.f);
        m_pAmbSpeakers[0].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[0].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(-30.f);
        m_pAmbSpeakers[1].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[1].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(0.f);
        m_pAmbSpeakers[2].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[2].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(110.f);
        m_pAmbSpeakers[3].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[3].SetPosition(polPosition);
        polPosition.fAzimuth = DegreesToRadians(-110.f);
        m_pAmbSpeakers[4].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[4].SetPosition(polPosition);
        break;
    case kAmblib_Pentagon:
        m_nSpeakers = 5;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / m_nSpeakers);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_Hexagon:
        m_nSpeakers = 6;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / m_nSpeakers + 30.f);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_HexagonWithCentre:
        m_nSpeakers = 6;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / m_nSpeakers);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_Octagon:
        m_nSpeakers = 8;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / m_nSpeakers);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_Decadron:
        m_nSpeakers = 10;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / m_nSpeakers);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_Dodecadron:
        m_nSpeakers = 12;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / m_nSpeakers);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_Cube:
        m_nSpeakers = 8;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        polPosition.fElevation = DegreesToRadians(45.f);
        for(niSpeaker = 0; niSpeaker < m_nSpeakers / 2; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / (m_nSpeakers / 2) + 45.f);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        polPosition.fElevation = DegreesToRadians(-45.f);
        for(niSpeaker = m_nSpeakers / 2; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians((niSpeaker - 4) * 360.f / (m_nSpeakers / 2) + 45.f);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_Dodecahedron:
        // This arrangement is used for second and third orders
        m_nSpeakers = 20;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        // Loudspeaker 1
        polPosition.fElevation = DegreesToRadians(-69.1f);
        polPosition.fAzimuth = DegreesToRadians(90.f);
        m_pAmbSpeakers[0].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[0].SetPosition(polPosition);
        // Loudspeaker 2
        polPosition.fAzimuth = DegreesToRadians(-90.f);
        m_pAmbSpeakers[1].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[1].SetPosition(polPosition);

        // Loudspeaker 3
        polPosition.fElevation = DegreesToRadians(-35.3f);
        polPosition.fAzimuth = DegreesToRadians(45.f);
        m_pAmbSpeakers[2].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[2].SetPosition(polPosition);
        // Loudspeaker 4
        polPosition.fAzimuth = DegreesToRadians(135.f);
        m_pAmbSpeakers[3].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[3].SetPosition(polPosition);
        // Loudspeaker 5
        polPosition.fAzimuth = DegreesToRadians(-45.f);
        m_pAmbSpeakers[4].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[4].SetPosition(polPosition);
        // Loudspeaker 6
        polPosition.fAzimuth = DegreesToRadians(-135.f);
        m_pAmbSpeakers[5].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[5].SetPosition(polPosition);

        // Loudspeaker 7
        polPosition.fElevation = DegreesToRadians(-20.9f);
        polPosition.fAzimuth = DegreesToRadians(180.f);
        m_pAmbSpeakers[6].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[6].SetPosition(polPosition);
        // Loudspeaker 8
        polPosition.fAzimuth = DegreesToRadians(0.f);
        m_pAmbSpeakers[7].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[7].SetPosition(polPosition);

        // Loudspeaker 9
        polPosition.fElevation = DegreesToRadians(0.f);
        polPosition.fAzimuth = DegreesToRadians(69.1f);
        m_pAmbSpeakers[8].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[8].SetPosition(polPosition);
        // Loudspeaker 10
        polPosition.fAzimuth = DegreesToRadians(110.9f);
        m_pAmbSpeakers[9].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[9].SetPosition(polPosition);
        // Loudspeaker 11
        polPosition.fAzimuth = DegreesToRadians(-69.1f);
        m_pAmbSpeakers[10].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[10].SetPosition(polPosition);
        // Loudspeaker 12
        polPosition.fAzimuth = DegreesToRadians(-110.9f);
        m_pAmbSpeakers[11].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[11].SetPosition(polPosition);

        // Loudspeaker 13
        polPosition.fElevation = DegreesToRadians(20.9f);
        polPosition.fAzimuth = DegreesToRadians(180.f);
        m_pAmbSpeakers[12].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[12].SetPosition(polPosition);
        // Loudspeaker 14
        polPosition.fAzimuth = DegreesToRadians(0.f);
        m_pAmbSpeakers[13].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[13].SetPosition(polPosition);

        // Loudspeaker 15
        polPosition.fElevation = DegreesToRadians(35.3f);
        polPosition.fAzimuth = DegreesToRadians(45.f);
        m_pAmbSpeakers[14].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[14].SetPosition(polPosition);
        // Loudspeaker 16
        polPosition.fAzimuth = DegreesToRadians(135.f);
        m_pAmbSpeakers[15].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[15].SetPosition(polPosition);
        // Loudspeaker 17
        polPosition.fAzimuth = DegreesToRadians(-45.f);
        m_pAmbSpeakers[16].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[16].SetPosition(polPosition);
        // Loudspeaker 18
        polPosition.fAzimuth = DegreesToRadians(-135.f);
        m_pAmbSpeakers[17].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[17].SetPosition(polPosition);

        // Loudspeaker 19
        polPosition.fElevation = DegreesToRadians(69.1f);
        polPosition.fAzimuth = DegreesToRadians(90.f);
        m_pAmbSpeakers[18].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[18].SetPosition(polPosition);
        // Loudspeaker 20
        polPosition.fAzimuth = DegreesToRadians(-90.f);
        m_pAmbSpeakers[19].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[19].SetPosition(polPosition);
        break;
    case kAmblib_Cube2:
        // This configuration is a standard for first order decoding
        m_nSpeakers = 8;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        polPosition.fElevation = DegreesToRadians(35.2f);
        for(niSpeaker = 0; niSpeaker < m_nSpeakers / 2; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians(niSpeaker * 360.f / (m_nSpeakers / 2) + 45.f);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        polPosition.fElevation = DegreesToRadians(-35.2f);
        for(niSpeaker = m_nSpeakers / 2; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = -DegreesToRadians((niSpeaker - 4) * 360.f / (m_nSpeakers / 2) + 45.f);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);
        }
        break;
    case kAmblib_MonoCustom:
        m_nSpeakers = 17;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        polPosition.fAzimuth = 0.f;
        polPosition.fElevation = 0.f;
        polPosition.fDistance = 1.f;
        for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        {
            polPosition.fAzimuth = DegreesToRadians(0.f);
            m_pAmbSpeakers[niSpeaker].Configure(m_nOrder, m_b3D, 0);
            m_pAmbSpeakers[niSpeaker].SetPosition(polPosition);

        }
        break;
    default:
        m_nSpeakers = 1;
        m_pAmbSpeakers = new CAmbisonicSpeaker[m_nSpeakers];
        m_pAmbSpeakers[0].Configure(m_nOrder, m_b3D, 0);
        m_pAmbSpeakers[0].SetPosition(polPosition);
        break;
    };

    fSpeakerGain = 1.f / sqrtf((float)m_nSpeakers);
    for(niSpeaker = 0; niSpeaker < m_nSpeakers; niSpeaker++)
        m_pAmbSpeakers[niSpeaker].SetGain(fSpeakerGain);
}

// For CAmbisonicZoomer
CAmbisonicZoomer::CAmbisonicZoomer()
{
    m_fZoom = 0;
}

bool CAmbisonicZoomer::Configure(unsigned nOrder, bool b3D, unsigned nMisc)
{
    bool success = CAmbisonicBase::Configure(nOrder, b3D, nMisc);
    if(!success)
        return false;

    m_AmbDecoderFront.Configure(m_nOrder, 1, kAmblib_Mono, 1);

    //Calculate all the speaker coefficients
    m_AmbDecoderFront.Refresh();

    m_fZoomRed = 0.f;

    m_AmbEncoderFront.reset(new float[m_nChannelCount]);
    m_AmbEncoderFront_weighted.reset(new float[m_nChannelCount]);
    a_m.reset(new float[m_nOrder + 1]);

    // These weights a_m are applied to the channels of a corresponding order within the Ambisonics signals.
    // When applied to the encoded channels and decoded to a particular loudspeaker direction they will create a
    // virtual microphone pattern with no rear lobes.
    // When used for decoding this is known as in-phase decoding.
    for(unsigned iOrder = 0; iOrder < m_nOrder + 1; iOrder++)
        a_m[iOrder] = (2*iOrder+1)*factorial(m_nOrder)*factorial(m_nOrder+1) / (factorial(m_nOrder+iOrder+1)*factorial(m_nOrder-iOrder));

    unsigned iDegree=0;
    for(unsigned iChannel = 0; iChannel<m_nChannelCount; iChannel++)
    {
        m_AmbEncoderFront[iChannel] = m_AmbDecoderFront.GetCoefficient(0, iChannel);
        iDegree = (int)floor(sqrt(iChannel));
        m_AmbEncoderFront_weighted[iChannel] = m_AmbEncoderFront[iChannel] * a_m[iDegree];
        // Normalisation factor
        m_AmbFrontMic += m_AmbEncoderFront[iChannel] * m_AmbEncoderFront_weighted[iChannel];
    }

    return true;
}

void CAmbisonicZoomer::Reset()
{

}

void CAmbisonicZoomer::Refresh()
{
    m_fZoomRed = sqrtf(1.f - m_fZoom * m_fZoom);
    m_fZoomBlend = 1.f - m_fZoom;
}

void CAmbisonicZoomer::SetZoom(float fZoom)
{
    // Limit the zoom value to always preserve the spacial effect.
    m_fZoom = std::min(fZoom, 0.99f);
}

float CAmbisonicZoomer::GetZoom()
{
    return m_fZoom;
}

void CAmbisonicZoomer::Process(CBFormat* pBFSrcDst, unsigned nSamples)
{
    for(unsigned niSample = 0; niSample < nSamples; niSample++)
    {
        float fMic = 0.f;

        for(unsigned iChannel=0; iChannel<m_nChannelCount; iChannel++)
        {
            // virtual microphone with polar pattern narrowing as Ambisonic order increases
            fMic += m_AmbEncoderFront_weighted[iChannel] * pBFSrcDst->m_ppfChannels[iChannel][niSample];
        }
        for(unsigned iChannel=0; iChannel<m_nChannelCount; iChannel++)
        {
            if(std::abs(m_AmbEncoderFront[iChannel])>1e-6)
            {
                // Blend original channel with the virtual microphone pointed directly to the front
                // Only do this for Ambisonics components that aren't zero for an encoded frontal source
                pBFSrcDst->m_ppfChannels[iChannel][niSample] = (m_fZoomBlend * pBFSrcDst->m_ppfChannels[iChannel][niSample]
                    + m_AmbEncoderFront[iChannel]*m_fZoom*fMic) / (m_fZoomBlend + std::fabs(m_fZoom)*m_AmbFrontMic);
            }
            else{
                // reduce the level of the Ambisonic components that are zero for a frontal source
                pBFSrcDst->m_ppfChannels[iChannel][niSample] = pBFSrcDst->m_ppfChannels[iChannel][niSample] * m_fZoomRed;
            }
        }
    }
}

float CAmbisonicZoomer::factorial(unsigned M)
{
    unsigned ret = 1;
    for(unsigned int i = 1; i <= M; ++i)
        ret *= i;
    return ret;
}

// For CAmbisonicBinauralizer
CAmbisonicBinauralizer::CAmbisonicBinauralizer()
    : m_pFFT_cfg(nullptr, kiss_fftr_free)
    , m_pIFFT_cfg(nullptr, kiss_fftr_free)
{
    m_nBlockSize = 0;
    m_nTaps = 0;
    m_nFFTSize = 0;
    m_nFFTBins = 0;
    m_fFFTScaler = 0.f;
    m_nOverlapLength = 0;
}

bool CAmbisonicBinauralizer::Configure(unsigned nOrder,
                                       bool b3D,
                                       unsigned nSampleRate,
                                       unsigned nBlockSize,
                                       unsigned& tailLength,
                                       std::string HRTFPath)
{
    //Iterators
    unsigned niEar = 0;
    unsigned niChannel = 0;
    unsigned niSpeaker = 0;
    unsigned niTap = 0;

    HRTF *p_hrtf = getHRTF(nSampleRate, HRTFPath);
    // printf("Done with getHRTF\n");
    if (p_hrtf == nullptr){
        // printf("p_hrtf is NULL\n");
        return false;
    }

    tailLength = m_nTaps = p_hrtf->getHRTFLen();
    m_nBlockSize = nBlockSize;

    //What will the overlap size be?
    m_nOverlapLength = m_nBlockSize < m_nTaps ? m_nBlockSize - 1 : m_nTaps - 1;
    //How large does the FFT need to be
    m_nFFTSize = 1;
    while(m_nFFTSize < (m_nBlockSize + m_nTaps + m_nOverlapLength))
        m_nFFTSize <<= 1;
    //How many bins is that
    m_nFFTBins = m_nFFTSize / 2 + 1;
    //What do we need to scale the result of the iFFT by
    m_fFFTScaler = 1.f / m_nFFTSize;

    CAmbisonicBase::Configure(nOrder, b3D, 0);
    //Position speakers and recalculate coefficients
    ArrangeSpeakers();

    unsigned nSpeakers = m_AmbDecoder.GetSpeakerCount();

    //Allocate buffers with new settings
    AllocateBuffers();

    //Allocate temporary buffers for retrieving taps from mit_hrtf_lib
    float* pfHRTF[2];
    for(niEar = 0; niEar < 2; niEar++)
        pfHRTF[niEar] = new float[m_nTaps];

    //Allocate buffers for HRTF accumulators
    float** ppfAccumulator[2];
    for(niEar = 0; niEar < 2; niEar++)
    {
        ppfAccumulator[niEar] = new float*[m_nChannelCount];
        for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
            ppfAccumulator[niEar][niChannel] = new float[m_nTaps]();
    }

    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
    {
        for(niSpeaker = 0; niSpeaker < nSpeakers; niSpeaker++)
        {
            //What is the position of the current speaker
            PolarPoint position = m_AmbDecoder.GetPosition(niSpeaker);

            bool b_found = p_hrtf->get(position.fAzimuth, position.fElevation, pfHRTF);
            if (!b_found)
                return false;

            //Scale the HRTFs by the coefficient of the current channel/component
            // The spherical harmonic coefficients are multiplied by (2*order + 1) to provide the correct decoder
            // for SN3D normalised Ambisonic inputs.
            float fCoefficient = m_AmbDecoder.GetCoefficient(niSpeaker, niChannel) * (2*floor(sqrt(niChannel)) + 1);
            for(niTap = 0; niTap < m_nTaps; niTap++)
            {
                pfHRTF[0][niTap] *= fCoefficient;
                pfHRTF[1][niTap] *= fCoefficient;
            }
            //Accumulate channel/component HRTF
            for(niTap = 0; niTap < m_nTaps; niTap++)
            {
                ppfAccumulator[0][niChannel][niTap] += pfHRTF[0][niTap];
                ppfAccumulator[1][niChannel][niTap] += pfHRTF[1][niTap];
            }
        }
    }

    delete p_hrtf;

    //Find the maximum tap
    float fMax = 0;

    // encode a source at azimuth 90deg and elevation 0
    CAmbisonicEncoder myEncoder;
    myEncoder.Configure(m_nOrder, true, 0);

    PolarPoint position90;
    position90.fAzimuth = DegreesToRadians(90.f);
    position90.fElevation = 0.f;
    position90.fDistance = 5.f;
    myEncoder.SetPosition(position90);
    myEncoder.Refresh();

    float* pfLeftEar90;
    pfLeftEar90 = new float[m_nTaps]();
    for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        for(niTap = 0; niTap < m_nTaps; niTap++)
            pfLeftEar90[niTap] += myEncoder.GetCoefficient(niChannel) * ppfAccumulator[0][niChannel][niTap];

    //Find the maximum value for a source encoded at 90degrees
    for(niTap = 0; niTap < m_nTaps; niTap++)
    {
        float val = fabs(pfLeftEar90[niTap]);
        fMax = val > fMax ? val : fMax;
    }

    //Normalize to pre-defined value
    float fUpperSample = 1.f;
    float fScaler = fUpperSample / fMax;
    fScaler *= 0.35f;
    for(niEar = 0; niEar < 2; niEar++)
    {
        for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            for(niTap = 0; niTap < m_nTaps; niTap++)
            {
                ppfAccumulator[niEar][niChannel][niTap] *= fScaler;
            }
        }
    }

    //Convert frequency domain filters
    for(niEar = 0; niEar < 2; niEar++)
    {
        for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            memcpy(m_pfScratchBufferA.data(), ppfAccumulator[niEar][niChannel], m_nTaps * sizeof(float));
            memset(&m_pfScratchBufferA[m_nTaps], 0, (m_nFFTSize - m_nTaps) * sizeof(float));
            kiss_fftr(m_pFFT_cfg.get(), m_pfScratchBufferA.data(), m_ppcpFilters[niEar][niChannel].get());
        }
    }

    for(niEar = 0; niEar < 2; niEar++)
        delete [] pfHRTF[niEar];

    for(niEar = 0; niEar < 2; niEar++)
    {
        for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
            delete [] ppfAccumulator[niEar][niChannel];
        delete [] ppfAccumulator[niEar];
    }
    delete[] pfLeftEar90;

    return true;
}

void CAmbisonicBinauralizer::Reset()
{
    memset(m_pfOverlap[0].data(), 0, m_nOverlapLength * sizeof(float));
    memset(m_pfOverlap[1].data(), 0, m_nOverlapLength * sizeof(float));
}

void CAmbisonicBinauralizer::Refresh() {}

void CAmbisonicBinauralizer::Process(CBFormat* pBFSrc,
                                     float** ppfDst)
{
    unsigned niEar = 0;
    unsigned niChannel = 0;
    unsigned ni = 0;
    kiss_fft_cpx cpTemp;


    /* If CPU load needs to be reduced then perform the convolution for each of the Ambisonics/spherical harmonic
    decompositions of the loudspeakers HRTFs for the left ear. For the left ear the results of these convolutions
    are summed to give the ear signal. For the right ear signal, the properties of the spherical harmonic decomposition
    can be use to to create the ear signal. This is done by either adding or subtracting the correct channels.
    Channels 1, 4, 5, 9, 10 and 11 are subtracted from the accumulated signal. All others are added.
    For example, for a first order signal the ears are generated from:
        SignalL = W x HRTF_W + Y x HRTF_Y + Z x HRTF_Z + X x HRTF_X
        SignalR = W x HRTF_W - Y x HRTF_Y + Z x HRTF_Z + X x HRTF_X
    where 'x' is a convolution, W/Y/Z/X are the Ambisonic signal channels and HRTF_x are the spherical harmonic
    decompositions of the virtual loudspeaker array HRTFs.
    This has the effect of assuming a completel symmetric head. */

    /* TODO: This bool flag should be either an automatic or user option depending on CPU. It should be 'true' if
    CPU load needs to be limited */
    bool bLowCPU = false;
    if(bLowCPU){
        // Perform the convolutions for the left ear and generate the right ear from a modified accumulation of these channels
        niEar = 0;
        memset(m_pfScratchBufferA.data(), 0, m_nFFTSize * sizeof(float));
        memset(m_pfScratchBufferC.data(), 0, m_nFFTSize * sizeof(float));
        for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
        {
            memcpy(m_pfScratchBufferB.data(), pBFSrc->m_ppfChannels[niChannel], m_nBlockSize * sizeof(float));
            memset(&m_pfScratchBufferB[m_nBlockSize], 0, (m_nFFTSize - m_nBlockSize) * sizeof(float));
            kiss_fftr(m_pFFT_cfg.get(), m_pfScratchBufferB.data(), m_pcpScratch.get());
            for(ni = 0; ni < m_nFFTBins; ni++)
            {
                cpTemp.r = m_pcpScratch[ni].r * m_ppcpFilters[niEar][niChannel][ni].r
                            - m_pcpScratch[ni].i * m_ppcpFilters[niEar][niChannel][ni].i;
                cpTemp.i = m_pcpScratch[ni].r * m_ppcpFilters[niEar][niChannel][ni].i
                            + m_pcpScratch[ni].i * m_ppcpFilters[niEar][niChannel][ni].r;
                m_pcpScratch[ni] = cpTemp;
            }
            kiss_fftri(m_pIFFT_cfg.get(), m_pcpScratch.get(), m_pfScratchBufferB.data());
            for(ni = 0; ni < m_nFFTSize; ni++)
                m_pfScratchBufferA[ni] += m_pfScratchBufferB[ni];

            for(ni = 0; ni < m_nFFTSize; ni++){
                // Subtract certain channels (such as Y) to generate right ear.
                if((niChannel==1) || (niChannel==4) || (niChannel==5) ||
                    (niChannel==9) || (niChannel==10)|| (niChannel==11))
                {
                    m_pfScratchBufferC[ni] -= m_pfScratchBufferB[ni];
                }
                else{
                    m_pfScratchBufferC[ni] += m_pfScratchBufferB[ni];
                }
            }
        }
        for(ni = 0; ni < m_nFFTSize; ni++){
            m_pfScratchBufferA[ni] *= m_fFFTScaler;
            m_pfScratchBufferC[ni] *= m_fFFTScaler;
        }
        memcpy(ppfDst[0], m_pfScratchBufferA.data(), m_nBlockSize * sizeof(float));
        memcpy(ppfDst[1], m_pfScratchBufferC.data(), m_nBlockSize * sizeof(float));
        for(ni = 0; ni < m_nOverlapLength; ni++){
            ppfDst[0][ni] += m_pfOverlap[0][ni];
            ppfDst[1][ni] += m_pfOverlap[1][ni];
        }
        memcpy(m_pfOverlap[0].data(), &m_pfScratchBufferA[m_nBlockSize], m_nOverlapLength * sizeof(float));
        memcpy(m_pfOverlap[1].data(), &m_pfScratchBufferC[m_nBlockSize], m_nOverlapLength * sizeof(float));
    }
    else
    {
        // Perform the convolution on both ears. Potentially more realistic results but requires double the number of
        // convolutions.
        for(niEar = 0; niEar < 2; niEar++)
        {
            memset(m_pfScratchBufferA.data(), 0, m_nFFTSize * sizeof(float));
            for(niChannel = 0; niChannel < m_nChannelCount; niChannel++)
            {
                memcpy(m_pfScratchBufferB.data(), pBFSrc->m_ppfChannels[niChannel], m_nBlockSize * sizeof(float));
                memset(&m_pfScratchBufferB[m_nBlockSize], 0, (m_nFFTSize - m_nBlockSize) * sizeof(float));
                // FFT
                kiss_fftr(m_pFFT_cfg.get(), m_pfScratchBufferB.data(), m_pcpScratch.get());
                // FIR Filter
                for(ni = 0; ni < m_nFFTBins; ni++)
                {
                    cpTemp.r = m_pcpScratch[ni].r * m_ppcpFilters[niEar][niChannel][ni].r
                                - m_pcpScratch[ni].i * m_ppcpFilters[niEar][niChannel][ni].i;
                    cpTemp.i = m_pcpScratch[ni].r * m_ppcpFilters[niEar][niChannel][ni].i
                                + m_pcpScratch[ni].i * m_ppcpFilters[niEar][niChannel][ni].r;
                    m_pcpScratch[ni] = cpTemp;
                }
                // IFFT
                kiss_fftri(m_pIFFT_cfg.get(), m_pcpScratch.get(), m_pfScratchBufferB.data());
                for(ni = 0; ni < m_nFFTSize; ni++)
                    m_pfScratchBufferA[ni] += m_pfScratchBufferB[ni];
            }
            for(ni = 0; ni < m_nFFTSize; ni++)
                m_pfScratchBufferA[ni] *= m_fFFTScaler;
            memcpy(ppfDst[niEar], m_pfScratchBufferA.data(), m_nBlockSize * sizeof(float));
            for(ni = 0; ni < m_nOverlapLength; ni++)
                ppfDst[niEar][ni] += m_pfOverlap[niEar][ni];
            memcpy(m_pfOverlap[niEar].data(), &m_pfScratchBufferA[m_nBlockSize], m_nOverlapLength * sizeof(float));
        }
    }
}

void CAmbisonicBinauralizer::ArrangeSpeakers()
{
    unsigned nSpeakerSetUp;
    //How many speakers will be needed? Add one for right above the listener
    unsigned nSpeakers = OrderToSpeakers(m_nOrder, m_b3D);
    //Custom speaker setup
    // Select cube layout for first order a dodecahedron for 2nd and 3rd
    if (m_nOrder == 1)
    {
        std::cout << "Getting first order cube" << std::endl;
        nSpeakerSetUp = kAmblib_Cube2;
    }
    else
    {
        std::cout << "Getting second/third order dodecahedron" << std::endl;
        nSpeakerSetUp = kAmblib_Dodecahedron;
    }

    m_AmbDecoder.Configure(m_nOrder, m_b3D, nSpeakerSetUp, nSpeakers);

    //Calculate all the speaker coefficients
    m_AmbDecoder.Refresh();
}


HRTF *CAmbisonicBinauralizer::getHRTF(unsigned nSampleRate, std::string HRTFPath)
{
    HRTF *p_hrtf = nullptr;

#ifdef HAVE_MYSOFA
# ifdef HAVE_MIT_HRTF
    if (HRTFPath == ""){
        printf("In HRTFPath==""\n");
        p_hrtf = new MIT_HRTF(nSampleRate);
        printf("p_hrtf is MIT_HRTF\n");
    }
    else
# endif
        p_hrtf = new SOFA_HRTF(HRTFPath, nSampleRate);
#else
# ifdef HAVE_MIT_HRTF
    p_hrtf = new MIT_HRTF(nSampleRate);
# else
# error At least MySOFA or MIT_HRTF need to be enabled
# endif
#endif

    if (p_hrtf == nullptr)
        return nullptr;

    if (!p_hrtf->isLoaded())
    {
        delete p_hrtf;
        return nullptr;
    }

    return p_hrtf;
}


void CAmbisonicBinauralizer::AllocateBuffers() {
    //Allocate scratch buffers
    m_pfScratchBufferA.resize(m_nFFTSize);
    m_pfScratchBufferB.resize(m_nFFTSize);
    m_pfScratchBufferC.resize(m_nFFTSize);

    //Allocate overlap-add buffers
    m_pfOverlap[0].resize(m_nOverlapLength);
    m_pfOverlap[1].resize(m_nOverlapLength);

    //Allocate FFT and iFFT for new size
    m_pFFT_cfg.reset(kiss_fftr_alloc(m_nFFTSize, 0, 0, 0));
    m_pIFFT_cfg.reset(kiss_fftr_alloc(m_nFFTSize, 1, 0, 0));

    //Allocate the FFTBins for each channel, for each ear
    for(unsigned niEar = 0; niEar < 2; niEar++)
    {
        m_ppcpFilters[niEar].resize(m_nChannelCount);
        for(unsigned niChannel = 0; niChannel < m_nChannelCount; niChannel++)
            m_ppcpFilters[niEar][niChannel].reset(new kiss_fft_cpx[m_nFFTBins]);
    }

    m_pcpScratch.reset(new kiss_fft_cpx[m_nFFTBins]);
}

// For Sound
ILLIXR_AUDIO::Sound::Sound(std::string srcFilename, unsigned nOrder, bool b3D){
    amp = 1.0;
    srcFile = new std::fstream(srcFilename, std::fstream::in);

    // NOTE: This is currently only accepts mono channel 16-bit depth WAV file
    // TODO: Change brutal read from wav file
    char temp[44];
    srcFile->read((char*)temp, 44);

    // BFormat file initialization
    BFormat = new CBFormat();
    bool ok = BFormat->Configure(nOrder, true, BLOCK_SIZE);
    BFormat->Refresh();

    // Encoder initialization
    BEncoder = new CAmbisonicEncoderDist();
    ok &= BEncoder->Configure(nOrder, true, SAMPLERATE);
    BEncoder->Refresh();
    srcPos.fAzimuth = 0;
    srcPos.fElevation = 0;
    srcPos.fDistance = 0;
    BEncoder->SetPosition(srcPos);
    BEncoder->Refresh();
}

void ILLIXR_AUDIO::Sound::setSrcPos(PolarPoint& pos){
    srcPos.fAzimuth = pos.fAzimuth;
    srcPos.fElevation = pos.fElevation;
    srcPos.fDistance = pos.fDistance;
    BEncoder->SetPosition(srcPos);
    BEncoder->Refresh();
}

void ILLIXR_AUDIO::Sound::setSrcAmp(float ampScale){
    amp = ampScale;
}

//TODO: Change brutal read from wav file
CBFormat* ILLIXR_AUDIO::Sound::readInBFormat(){
    short sampleTemp[BLOCK_SIZE];
    srcFile->read((char*)sampleTemp, BLOCK_SIZE * sizeof(short));
    // normalize samples to -1 to 1 float, with amplitude scale
    for (int i = 0; i < BLOCK_SIZE; ++i){
        sample[i] = amp * (sampleTemp[i] / 32767.0);
    }
    BEncoder->Process(sample, BLOCK_SIZE, BFormat);
    return BFormat;
}

ILLIXR_AUDIO::Sound::~Sound(){
    srcFile->close();
    delete srcFile;
    delete BFormat;
    delete BEncoder;
}

// For ABAudio
ILLIXR_AUDIO::ABAudio::ABAudio(std::string outputFilePath, ProcessType procTypeIn){
    processType = procTypeIn;
    if (processType == ILLIXR_AUDIO::ABAudio::ProcessType::FULL){
        outputFile = new std::ofstream(outputFilePath, std::ios_base::out| std::ios_base::binary);
        generateWAVHeader();
    }

    soundSrcs = new std::vector<Sound*>;
    // binauralizer as ambisonics decoder
    // decoder = new CAmbisonicBinauralizer();
    decoder0 = new CAmbisonicBinauralizer();
    decoder1 = new CAmbisonicBinauralizer();
    unsigned temp;
    // bool ok = decoder->Configure(NORDER, true, SAMPLERATE, BLOCK_SIZE, temp, "");
    bool ok = decoder0->Configure(NORDER, true, SAMPLERATE, BLOCK_SIZE, temp, "") && decoder1->Configure(NORDER, true, SAMPLERATE, BLOCK_SIZE, temp, "");
    if (!ok){
        printf("Binauralizer Configuration failed!\n");
    }
    // Processor to rotate
    rotator = new CAmbisonicProcessor();
    rotator->Configure(NORDER, true, BLOCK_SIZE, 0);
    // Processor to zoom
    zoomer = new CAmbisonicZoomer();
    zoomer->Configure(NORDER, true, 0);
}

ILLIXR_AUDIO::ABAudio::~ABAudio(){
    if (processType == ILLIXR_AUDIO::ABAudio::ProcessType::FULL){
        free(outputFile);
    }
    for (unsigned int soundIdx = 0; soundIdx < soundSrcs->size(); ++soundIdx){
        free((*soundSrcs)[soundIdx]);
    }
    free(soundSrcs);
    // free(decoder);
    free(decoder0);
    free(decoder1);
    free(rotator);
    free(zoomer);
}

void ILLIXR_AUDIO::ABAudio::loadSource(){
    // Add a bunch of sound sources
    Sound* inSound;
    PolarPoint position;

    if (processType == ILLIXR_AUDIO::ABAudio::ProcessType::FULL){
        inSound = new Sound("samples/lectureSample.wav", NORDER, true);
        position.fAzimuth = -0.1;
        position.fElevation = 3.14/2;
        position.fDistance = 1;
        inSound->setSrcPos(position);
        soundSrcs->push_back(inSound);
        inSound = new Sound("samples/radioMusicSample.wav", NORDER, true);
        position.fAzimuth = 1.0;
        position.fElevation = 0;
        position.fDistance = 5;
        inSound->setSrcPos(position);
        soundSrcs->push_back(inSound);
    }else{
        for (unsigned i = 0; i < NUM_SRCS; i++) {
            inSound = new Sound("samples/lectureSample.wav", NORDER, true);
            position.fAzimuth = -0.1 * i;
            position.fElevation = 3.14/2 * i;
            position.fDistance = 1 * i;
            inSound->setSrcPos(position);
            soundSrcs->push_back(inSound);
        }
    }
}

void ILLIXR_AUDIO::ABAudio::processBlock(){
    float** resultSample = new float*[2];
    resultSample[0] = new float[BLOCK_SIZE];
    resultSample[1] = new float[BLOCK_SIZE];

    // temporary BFormat file to sum up ambisonics
    CBFormat sumBF;
    sumBF.Configure(NORDER, true, BLOCK_SIZE);

    if (processType != ILLIXR_AUDIO::ABAudio::ProcessType::DECODE){
        readNEncode(sumBF);
    }
    if (processType != ILLIXR_AUDIO::ABAudio::ProcessType::ENCODE){
        // processing garbage data if just decoding
        rotateNZoom(sumBF);
        // decoder->Process(&sumBF, resultSample);
        decoder0->Process(&sumBF, resultSample);
        decoder1->Process(&sumBF, resultSample);
    }

    if (processType == ILLIXR_AUDIO::ABAudio::ProcessType::FULL){
        writeFile(resultSample);
    }

    delete[] resultSample[0];
    delete[] resultSample[1];
    delete[] resultSample;
}

// Read from WAV files and encode into ambisonics format
void ILLIXR_AUDIO::ABAudio::readNEncode(CBFormat& sumBF){
    CBFormat* tempBF;
    for (unsigned int soundIdx = 0; soundIdx < soundSrcs->size(); ++soundIdx){
        tempBF = (*soundSrcs)[soundIdx]->readInBFormat();
        if (soundIdx == 0)
            sumBF = *tempBF;
        else
            sumBF += *tempBF;
   }
}

// Simple rotation
void ILLIXR_AUDIO::ABAudio::updateRotation(){
	static int frame = 0;
	frame++;
	Orientation head(0, 0, 1.0*frame/1500*3.14*2);
	rotator->SetOrientation(head);
	rotator->Refresh();
}
// Simple zoom
void ILLIXR_AUDIO::ABAudio::updateZoom(){
	static int frame = 0;
	frame++;
	zoomer->SetZoom(sinf(frame/100));
	zoomer->Refresh();
}
// Process some rotation and zoom effects
void ILLIXR_AUDIO::ABAudio::rotateNZoom(CBFormat& sumBF){
	updateRotation();
	rotator->Process(&sumBF, BLOCK_SIZE);
	updateZoom();
	zoomer->Process(&sumBF, BLOCK_SIZE);
}

void ILLIXR_AUDIO::ABAudio::writeFile(float** resultSample){
	// Normalize(Clipping), then write into file
	for(int sampleIdx = 0; sampleIdx < BLOCK_SIZE; ++sampleIdx){
		resultSample[0][sampleIdx] = std::max(std::min(resultSample[0][sampleIdx], +1.0f), -1.0f);
		resultSample[1][sampleIdx] = std::max(std::min(resultSample[1][sampleIdx], +1.0f), -1.0f);
		int16_t tempSample0 = (int16_t)(resultSample[0][sampleIdx]/1.0 * 32767);
		int16_t tempSample1 = (int16_t)(resultSample[1][sampleIdx]/1.0 * 32767);
		outputFile->write((char*)&tempSample0,sizeof(short));
		outputFile->write((char*)&tempSample1,sizeof(short));
	}
}

namespace ILLIXR_AUDIO{
    // NOTE: WAV FILE SIZE is not correct
    typedef struct __attribute__ ((packed)) WAVHeader_t
    {
    	unsigned int sGroupID = 0x46464952;
    	unsigned int dwFileLength = 48000000;		// A large enough random number
    	unsigned int sRiffType = 0x45564157;
    	unsigned int subchunkID = 0x20746d66;
    	unsigned int subchunksize = 16;
    	unsigned short audioFormat = 1;
    	unsigned short NumChannels = 2;
    	unsigned int SampleRate = 48000;
    	unsigned int byteRate = 48000*2*2;
    	unsigned short BlockAlign = 2*2;
    	unsigned short BitsPerSample = 16;
    	unsigned int dataChunkID = 0x61746164;	
    	unsigned int dataChunkSize = 48000000;		// A large enough random number
    } WAVHeader;
}
void ILLIXR_AUDIO::ABAudio::generateWAVHeader(){
	// brute force wav header
	WAVHeader wavh;
	outputFile->write((char*)&wavh, sizeof(WAVHeader));
}

extern "C" {
// The root arg for audio decoding
struct __attribute__((__packed__)) RootArg {
    /*0*/ CAmbisonicProcessor* rotator;
    /*1*/ size_t bytes_rotator;
    /*2*/ CBFormat* sumBF;
    /*3*/ size_t bytes_sumBF;
    /*4*/ CAmbisonicZoomer* zoomer;
    /*5*/ size_t bytes_zoomer;
    /*6*/ CAmbisonicBinauralizer* decoder0;
    /*7*/ size_t bytes_decoder0;
    /*8*/ float* resultSample0;
    /*9*/ size_t bytes_resultSample0;
    /*10*/ long nSample;
    /*11*/ float** channelpart1;
    /*12*/ size_t bytes_channelpart1;
    /*13*/ float** channelpart2;
    /*14*/ size_t bytes_channelpart2;
    /*15*/ float** channelpart3;
    /*16*/ size_t bytes_channelpart3;
    /*17*/ CAmbisonicBinauralizer* decoder1;
    /*18*/ size_t bytes_decoder1;
    /*19*/ float* resultSample1;
    /*20*/ size_t bytes_resultSample1;
    /*21*/ long decoder0ChannelCount;
    /*22*/ long decoder0FFTBins;
    /*23*/ long decoder1ChannelCount;
    /*24*/ long decoder1FFTBins;
};

struct __attribute__((__packed__)) RootOut {
    size_t result;
};

// A leaf node for the rotator
// void rotator_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ unsigned nSample) {
//     __hpvm__hint(hpvm::DEVICE);
//     __hpvm__attributes(2, rotator, sumBF, 1, sumBF);    // rotator = audioAddr->rotator

//     static int frame = 0;
// 	frame++;
// 	Orientation head(0, 0, 1.0*frame/1500*3.14*2);
// 	rotator->SetOrientation(head);
// 	rotator->Refresh();
//     rotator->Process(sumBF, nSample);

//     __hpvm__return(1, bytes_sumBF);
// }

// A leaf node to set the rotator orientation
void rotatorSet_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(1, rotator, 1, rotator);

    // printf("rotatorSet -> psychoFilter: rotator: %zu\n", bytes_rotator);

    // printf("rotatorSet starts\n");
    static int frame = 0;
	frame++;
	Orientation head(0, 0, 1.0*frame/1500*3.14*2);
	rotator->SetOrientation(head);
	rotator->Refresh();
    // printf("rotatorSet ends\n");

    __hpvm__return(1, bytes_rotator);
}

void wrapperRotatorSet_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(1, rotator, 1, rotator);

    void* rotateSetNode = __hpvm__createNodeND(0, rotatorSet_fxp);

    __hpvm__bindIn(rotateSetNode, 0, 0, 0);
    __hpvm__bindIn(rotateSetNode, 1, 1, 0);

    __hpvm__bindOut(rotateSetNode, 0, 0, 0);
}

// A leaf node to add the psychoacoustic shelf filter
// void psychoFilter_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ unsigned nSample) {
//     __hpvm__hint(hpvm::DEVICE);
//     __hpvm__attributes(2, rotator, sumBF, 1, sumBF);

//     // printf("psychoFilter starts\n");
//     if (rotator->m_bOpt) {
//         rotator->ShelfFilterOrder(sumBF, nSample);
//     }
//     else {}
//     // printf("psychoFilter ends\n");

//     __hpvm__return(2, bytes_rotator, bytes_sumBF);
// }

void psychoFilter_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ long nSample, /*5*/ float** channelpart1, /*6*/ size_t bytes_channelpart1, /*7*/ float** channelpart2, /*8*/ size_t bytes_channelpart2, /*9*/ float** channelpart3, /*10*/ size_t bytes_channelpart3) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(5, rotator, sumBF, channelpart1, channelpart2, channelpart3, 5, rotator, sumBF, channelpart1, channelpart2, channelpart3);

    // printf("psychoFilter -> rotateOrder1: channelpart1: %zu\n", bytes_channelpart1);
    // printf("psychoFilter -> rotateOrder1: rotator: %zu\n", bytes_rotator);
    // printf("psychoFilter -> rotateOrder2: channelpart2: %zu\n", bytes_channelpart2);
    // printf("psychoFilter -> rotateOrder2: rotator: %zu\n", bytes_rotator);
    // printf("psychoFilter -> rotateOrder3: channelpart3: %zu\n", bytes_channelpart3);
    // printf("psychoFilter -> rotateOrder3: rotator: %zu\n", bytes_rotator);
    // printf("psychoFilter -> zoomProcess: sumBF: %zu\n", bytes_sumBF);

    // printf("psychoFilter starts\n");
    if (rotator->m_bOpt) {
        rotator->ShelfFilterOrder(sumBF, nSample);
    }
    else {}
    // printf("psychoFilter ends\n");

    for (int i = 0; i < nSample; ++i) {
        channelpart1[0][i] = sumBF->m_ppfChannels[kX][i];
        channelpart1[1][i] = sumBF->m_ppfChannels[kY][i];
        channelpart1[2][i] = sumBF->m_ppfChannels[kZ][i];

        channelpart2[0][i] = sumBF->m_ppfChannels[kR][i];
        channelpart2[1][i] = sumBF->m_ppfChannels[kS][i];
        channelpart2[2][i] = sumBF->m_ppfChannels[kT][i];
        channelpart2[3][i] = sumBF->m_ppfChannels[kU][i];
        channelpart2[4][i] = sumBF->m_ppfChannels[kV][i];

        channelpart3[0][i] = sumBF->m_ppfChannels[kQ][i];
        channelpart3[1][i] = sumBF->m_ppfChannels[kO][i];
        channelpart3[2][i] = sumBF->m_ppfChannels[kM][i];
        channelpart3[3][i] = sumBF->m_ppfChannels[kK][i];
        channelpart3[4][i] = sumBF->m_ppfChannels[kL][i];
        channelpart3[5][i] = sumBF->m_ppfChannels[kN][i];
        channelpart3[6][i] = sumBF->m_ppfChannels[kP][i];
    }    

    __hpvm__return(7, bytes_rotator, bytes_rotator, bytes_rotator, bytes_sumBF, bytes_channelpart1, bytes_channelpart2, bytes_channelpart3);
}

void wrapperPsychoFilter_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ long nSample, /*5*/ float** channelpart1, /*6*/ size_t bytes_channelpart1, /*7*/ float** channelpart2, /*8*/ size_t bytes_channelpart2, /*9*/ float** channelpart3, /*10*/ size_t bytes_channelpart3) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(5, rotator, sumBF, channelpart1, channelpart2, channelpart3, 5, rotator, sumBF, channelpart1, channelpart2, channelpart3);

    // void* psychoNode = __hpvm__createNodeND(1, psychoFilter_fxp, nSample);
    void* psychoNode = __hpvm__createNodeND(0, psychoFilter_fxp);

    __hpvm__bindIn(psychoNode, 0, 0, 0);
    __hpvm__bindIn(psychoNode, 1, 1, 0);
    __hpvm__bindIn(psychoNode, 2, 2, 0);
    __hpvm__bindIn(psychoNode, 3, 3, 0);
    __hpvm__bindIn(psychoNode, 4, 4, 0);
    __hpvm__bindIn(psychoNode, 5, 5, 0);
    __hpvm__bindIn(psychoNode, 6, 6, 0);
    __hpvm__bindIn(psychoNode, 7, 7, 0);
    __hpvm__bindIn(psychoNode, 8, 8, 0);
    __hpvm__bindIn(psychoNode, 9, 9, 0);
    __hpvm__bindIn(psychoNode, 10, 10, 0);

    __hpvm__bindOut(psychoNode, 0, 0, 0);
    __hpvm__bindOut(psychoNode, 1, 1, 0);
    __hpvm__bindOut(psychoNode, 2, 2, 0);
    __hpvm__bindOut(psychoNode, 3, 3, 0);
    __hpvm__bindOut(psychoNode, 4, 4, 0);
    __hpvm__bindOut(psychoNode, 5, 5, 0);
    __hpvm__bindOut(psychoNode, 6, 6, 0);
}

// A leaf node to process rotate_order_1
// void rotateOrder1_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ unsigned nSample) {
//     __hpvm__hint(hpvm::DEVICE);
//     __hpvm__attributes(2, rotator, sumBF, 1, rotator);

//     // printf("rotateOrder1 starts\n");
//     // printf("%u\n", rotator->m_nOrder);
//     if (rotator->m_nOrder >= 1) {
//         rotator->ProcessOrder1_3D(sumBF, nSample);
//     }
//     // printf("rotateOrder1 ends\n");

//     __hpvm__return(2, bytes_rotator, bytes_sumBF);
// }

void rotateOrder1_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ long nSample, /*3*/ float** channelpart1, /*4*/ size_t bytes_channelpart1) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(2, rotator, channelpart1, 1, channelpart1);

    // printf("rotateOrder1 starts\n");
    // printf("%u\n", rotator->m_nOrder);
    if (rotator->m_nOrder >= 1) {
        // rotator->ProcessOrder1_3D(sumBF, nSample);
        // float fSqrt3 = sqrt(3.f);

        float valueY;
        float valueZ;
        float valueX;

        // This is the non-parallel version
        for(unsigned niSample = 0; niSample < nSample; niSample++) {
            valueY = -channelpart1[0][niSample] * rotator->m_fSinAlpha + channelpart1[1][niSample] * rotator->m_fCosAlpha;
            valueZ = channelpart1[2][niSample];
            valueX = channelpart1[0][niSample] * rotator->m_fCosAlpha + channelpart1[1][niSample] * rotator->m_fSinAlpha;

            channelpart1[1][niSample] = valueY;
            channelpart1[2][niSample] = valueZ * rotator->m_fCosBeta + valueX * rotator->m_fSinBeta;
            channelpart1[0][niSample] = valueX * rotator->m_fCosBeta + valueZ * rotator->m_fSinBeta;

            valueY = -channelpart1[0][niSample] * rotator->m_fSinGamma + channelpart1[1][niSample] * rotator->m_fCosGamma;
            valueZ = channelpart1[2][niSample];
            valueX = channelpart1[0][niSample] * rotator->m_fCosGamma + channelpart1[1][niSample] * rotator->m_fSinGamma;

            channelpart1[0][niSample] = valueX;
            channelpart1[1][niSample] = valueY;
            channelpart1[2][niSample] = valueZ;
        }

        // This is the parallel version
        // void* thisNode = __hpvm__getNode();
        // long niSample = __hpvm__getNodeInstanceID_x(thisNode);

        // if (niSample < nSample) {
        //     valueY = -channelpart1[0][niSample] * rotator->m_fSinAlpha + channelpart1[1][niSample] * rotator->m_fCosAlpha;
        //     valueZ = channelpart1[2][niSample];
        //     valueX = channelpart1[0][niSample] * rotator->m_fCosAlpha + channelpart1[1][niSample] * rotator->m_fSinAlpha;

        //     channelpart1[1][niSample] = valueY;
        //     channelpart1[2][niSample] = valueZ * rotator->m_fCosBeta + valueX * rotator->m_fSinBeta;
        //     channelpart1[0][niSample] = valueX * rotator->m_fCosBeta + valueZ * rotator->m_fSinBeta;

        //     valueY = -channelpart1[0][niSample] * rotator->m_fSinGamma + channelpart1[1][niSample] * rotator->m_fCosGamma;
        //     valueZ = channelpart1[2][niSample];
        //     valueX = channelpart1[0][niSample] * rotator->m_fCosGamma + channelpart1[1][niSample] * rotator->m_fSinGamma;

        //     channelpart1[0][niSample] = valueX;
        //     channelpart1[1][niSample] = valueY;
        //     channelpart1[2][niSample] = valueZ;
        // }


    }
    // printf("rotateOrder1 ends\n");

    __hpvm__return(1, bytes_channelpart1);
}

void wrapperRotateOrder1_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ long nSample, /*3*/ float** channelpart1, /*4*/ size_t bytes_channelpart1) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(2, rotator, channelpart1, 1, channelpart1);

    // void* ro1Node = __hpvm__createNodeND(1, rotateOrder1_fxp, nSample);
    void* ro1Node = __hpvm__createNodeND(0, rotateOrder1_fxp);

    __hpvm__bindIn(ro1Node, 0, 0, 0);
    __hpvm__bindIn(ro1Node, 1, 1, 0);
    __hpvm__bindIn(ro1Node, 2, 2, 0);
    __hpvm__bindIn(ro1Node, 3, 3, 0);
    __hpvm__bindIn(ro1Node, 4, 4, 0);

    __hpvm__bindOut(ro1Node, 0, 0, 0);
}

// A leaf node to process rotate_order_2
// void rotateOrder2_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ unsigned nSample) {
//     __hpvm__hint(hpvm::DEVICE);
//     __hpvm__attributes(2, rotator, sumBF, 1, rotator);

//     // printf("rotateOrder2 starts\n");
//     if (rotator->m_nOrder >= 2) {
//         rotator->ProcessOrder2_3D(sumBF, nSample);
//     }
//     // printf("rotateOrder2 ends\n");

//     __hpvm__return(2, bytes_rotator, bytes_sumBF);
// }

void rotateOrder2_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ long nSample, /*3*/ float** channelpart2, /*4*/ size_t bytes_channelpart2) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(2, rotator, channelpart2, 1, channelpart2);

    // printf("rotateOrder2 starts\n");
    // printf("%u\n", rotator->m_nOrder);
    if (rotator->m_nOrder >= 2) {
        // rotator->ProcessOrder2_3D(sumBF, nSample);
        float fSqrt3 = sqrt(3.f);

        float valueV;
        float valueT;
        float valueR;
        float valueS;
        float valueU;

        // This is the non-parallel version
        for (unsigned niSample = 0; niSample < nSample; niSample++) {  // R-0 S-1 T-2 U-3 V-4
            valueV = -channelpart2[3][niSample] * rotator->m_fSin2Alpha + channelpart2[4][niSample] * rotator->m_fCos2Alpha;
            valueT = -channelpart2[1][niSample] * rotator->m_fSinAlpha + channelpart2[2][niSample] * rotator->m_fCosAlpha;
            valueR = channelpart2[0][niSample];
            valueS = channelpart2[1][niSample] * rotator->m_fCosAlpha + channelpart2[2][niSample] * rotator->m_fSinAlpha;
            valueU = channelpart2[3][niSample] * rotator->m_fCos2Alpha + channelpart2[4][niSample] * rotator->m_fSin2Alpha;

            channelpart2[4][niSample] = -rotator->m_fSinBeta * valueT + rotator->m_fCosBeta * valueV;
            channelpart2[2][niSample] = -rotator->m_fCosBeta * valueT + rotator->m_fSinBeta * valueV;
            channelpart2[0][niSample] = (0.75f * rotator->m_fCos2Beta + 0.25f) * valueR + (0.5 * fSqrt3 * pow(rotator->m_fSinBeta, 2.0)) * valueU + (fSqrt3 * rotator->m_fSinBeta * rotator->m_fCosBeta) * valueS;
            channelpart2[1][niSample] = rotator->m_fCos2Beta * valueS - fSqrt3 * rotator->m_fCosBeta * rotator->m_fSinBeta * valueR + rotator->m_fCosBeta * rotator->m_fSinBeta * valueU;
            channelpart2[3][niSample] = (0.25f * rotator->m_fCos2Beta + 0.75f) * valueU - rotator->m_fCosBeta * rotator->m_fSinBeta * valueS + 0.5 * fSqrt3 * pow(rotator->m_fSinBeta, 2.0) * valueR;

            valueV = -channelpart2[3][niSample] * rotator->m_fSin2Gamma + channelpart2[4][niSample] * rotator->m_fCos2Gamma;
            valueT = -channelpart2[1][niSample] * rotator->m_fSinGamma + channelpart2[2][niSample] * rotator->m_fCosGamma;
            valueR = channelpart2[0][niSample];
            valueS = channelpart2[1][niSample] * rotator->m_fCosGamma + channelpart2[2][niSample] * rotator->m_fSinGamma;
            valueU = channelpart2[3][niSample] * rotator->m_fCos2Gamma + channelpart2[4][niSample] * rotator->m_fSin2Gamma;

            channelpart2[0][niSample] = valueR;
            channelpart2[1][niSample] = valueS;
            channelpart2[2][niSample] = valueT;
            channelpart2[3][niSample] = valueU;
            channelpart2[4][niSample] = valueV;
        }

        // This is the parallel version
        // void* thisNode = __hpvm__getNode();
        // long niSample = __hpvm__getNodeInstanceID_x(thisNode);

        // if (niSample < nSample) {
        //     valueV = -channelpart2[3][niSample] * rotator->m_fSin2Alpha + channelpart2[4][niSample] * rotator->m_fCos2Alpha;
        //     valueT = -channelpart2[1][niSample] * rotator->m_fSinAlpha + channelpart2[2][niSample] * rotator->m_fCosAlpha;
        //     valueR = channelpart2[0][niSample];
        //     valueS = channelpart2[1][niSample] * rotator->m_fCosAlpha + channelpart2[2][niSample] * rotator->m_fSinAlpha;
        //     valueU = channelpart2[3][niSample] * rotator->m_fCos2Alpha + channelpart2[4][niSample] * rotator->m_fSin2Alpha;

        //     channelpart2[4][niSample] = -rotator->m_fSinBeta * valueT + rotator->m_fCosBeta * valueV;
        //     channelpart2[2][niSample] = -rotator->m_fCosBeta * valueT + rotator->m_fSinBeta * valueV;
        //     channelpart2[0][niSample] = (0.75f * rotator->m_fCos2Beta + 0.25f) * valueR + (0.5 * fSqrt3 * pow(rotator->m_fSinBeta, 2.0)) * valueU + (fSqrt3 * rotator->m_fSinBeta * rotator->m_fCosBeta) * valueS;
        //     channelpart2[1][niSample] = rotator->m_fCos2Beta * valueS - fSqrt3 * rotator->m_fCosBeta * rotator->m_fSinBeta * valueR + rotator->m_fCosBeta * rotator->m_fSinBeta * valueU;
        //     channelpart2[3][niSample] = (0.25f * rotator->m_fCos2Beta + 0.75f) * valueU - rotator->m_fCosBeta * rotator->m_fSinBeta * valueS + 0.5 * fSqrt3 * pow(rotator->m_fSinBeta, 2.0) * valueR;

        //     valueV = -channelpart2[3][niSample] * rotator->m_fSin2Gamma + channelpart2[4][niSample] * rotator->m_fCos2Gamma;
        //     valueT = -channelpart2[1][niSample] * rotator->m_fSinGamma + channelpart2[2][niSample] * rotator->m_fCosGamma;
        //     valueR = channelpart2[0][niSample];
        //     valueS = channelpart2[1][niSample] * rotator->m_fCosGamma + channelpart2[2][niSample] * rotator->m_fSinGamma;
        //     valueU = channelpart2[3][niSample] * rotator->m_fCos2Gamma + channelpart2[4][niSample] * rotator->m_fSin2Gamma;

        //     channelpart2[0][niSample] = valueR;
        //     channelpart2[1][niSample] = valueS;
        //     channelpart2[2][niSample] = valueT;
        //     channelpart2[3][niSample] = valueU;
        //     channelpart2[4][niSample] = valueV;
        // }


    }
    // printf("rotateOrder2 ends\n");

    __hpvm__return(1, bytes_channelpart2);
}

void wrapperRotateOrder2_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ long nSample, /*3*/ float** channelpart2, /*4*/ size_t bytes_channelpart2) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(2, rotator, channelpart2, 1, channelpart2);

    // void* ro2Node = __hpvm__createNodeND(1, rotateOrder2_fxp, nSample);
    void* ro2Node = __hpvm__createNodeND(0, rotateOrder2_fxp);

    __hpvm__bindIn(ro2Node, 0, 0, 0);
    __hpvm__bindIn(ro2Node, 1, 1, 0);
    __hpvm__bindIn(ro2Node, 2, 2, 0);
    __hpvm__bindIn(ro2Node, 3, 3, 0);
    __hpvm__bindIn(ro2Node, 4, 4, 0);

    __hpvm__bindOut(ro2Node, 0, 0, 0);
}

// A leaf node to process rotate_order_3
// void rotateOrder3_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ unsigned nSample) {
//     __hpvm__hint(hpvm::DEVICE);
//     __hpvm__attributes(2, rotator, sumBF, 1, rotator);

//     // printf("rotateOrder3 starts\n");
//     if (rotator->m_nOrder >= 3) {
//         rotator->ProcessOrder3_3D(sumBF, nSample);
//     }
//     // printf("rotateOrder3 ends\n");

//     __hpvm__return(1, bytes_sumBF);
// }

void rotateOrder3_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ long nSample, /*3*/ float** channelpart3, /*4*/ size_t bytes_channelpart3) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(2, rotator, channelpart3, 1, channelpart3);

    // printf("rotateOrder3 starts\n");
    // printf("%u\n", rotator->m_nOrder);
    if (rotator->m_nOrder >= 3) {
        // rotator->ProcessOrder3_3D(sumBF, nSample);
        float fSqrt3_2 = sqrt(3.f/2.f);
        float fSqrt15 = sqrt(15.f);
        float fSqrt5_2 = sqrt(5.f/2.f);

        float valueQ;
        float valueO;
        float valueM;
        float valueK;
        float valueL;
        float valueN;
        float valueP;

        // This is the non-parallel version
        for (unsigned niSample = 0; niSample < nSample; niSample++) {  // Q-0 O-1 M-2 K-3 L-4 N-5 P-6
            valueQ = -channelpart3[6][niSample] * rotator->m_fSin3Alpha + channelpart3[0][niSample] * rotator->m_fCos3Alpha;
            valueO = -channelpart3[5][niSample] * rotator->m_fSin2Alpha + channelpart3[1][niSample] * rotator->m_fCos2Alpha;
            valueM = -channelpart3[4][niSample] * rotator->m_fSinAlpha + channelpart3[2][niSample] * rotator->m_fCosAlpha;
            valueK = channelpart3[3][niSample];
            valueL = channelpart3[4][niSample] * rotator->m_fCosAlpha + channelpart3[2][niSample] * rotator->m_fSinAlpha;
            valueN = channelpart3[5][niSample] * rotator->m_fCos2Alpha + channelpart3[1][niSample] * rotator->m_fSin2Alpha;
            valueP = channelpart3[6][niSample] * rotator->m_fCos3Alpha + channelpart3[0][niSample] * rotator->m_fSin3Alpha;

            channelpart3[0][niSample] = 0.125f * valueQ * (5.f + 3.f * rotator->m_fCos2Beta) - fSqrt3_2 * valueO * rotator->m_fCosBeta * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueM * pow(rotator->m_fSinBeta, 2.0f);
            channelpart3[1][niSample] = valueO * rotator->m_fCos2Beta - fSqrt5_2 * valueM * rotator->m_fCosBeta * rotator->m_fSinBeta + fSqrt3_2 * valueQ * rotator->m_fCosBeta * rotator->m_fSinBeta; 
            channelpart3[2][niSample] = 0.125f * valueM * (3.f + 5.f * rotator->m_fCos2Beta) - fSqrt5_2 * valueO * rotator->m_fCosBeta * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueQ * pow(rotator->m_fSinBeta, 2.0f);
            channelpart3[3][niSample] = 0.25f * valueK * rotator->m_fCosBeta * (-1.f + 15.f * rotator->m_fCos2Beta) + 0.5f * fSqrt15 * valueN * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) + 0.5f * fSqrt5_2 * valueP * pow(rotator->m_fSinBeta, 3.f) + 0.125f * fSqrt3_2 * valueL * (rotator->m_fSinBeta + 5.f * rotator->m_fSin3Beta);
            channelpart3[4][niSample] = 0.0625f * valueL * (rotator->m_fCosBeta + 15.f * rotator->m_fCos3Beta) + 0.25f * fSqrt5_2 * valueN * (1.f + 3.f * rotator->m_fCos2Beta) * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueP * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) - 0.125 * fSqrt3_2 * valueK * (rotator->m_fSinBeta + 5.f * rotator->m_fSin3Beta);
            channelpart3[5][niSample] = 0.125f * valueN * (5.f * rotator->m_fCosBeta + 3.f * rotator->m_fCos3Beta) + 0.25f * fSqrt3_2 * valueP * (3.f + rotator->m_fCos2Beta) * rotator->m_fSinBeta + 0.5f * fSqrt15 * valueK * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) + 0.125 * fSqrt5_2 * valueL * (rotator->m_fSinBeta - 3.f * rotator->m_fSin3Beta);
            channelpart3[6][niSample] = 0.0625f * valueP * (15.f * rotator->m_fCosBeta + rotator->m_fCos3Beta) - 0.25f * fSqrt3_2 * valueN * (3.f + rotator->m_fCos2Beta) * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueL * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) - 0.5 * fSqrt5_2 * valueK * pow(rotator->m_fSinBeta, 3.f);

            valueQ = -channelpart3[6][niSample] * rotator->m_fSin3Gamma + channelpart3[0][niSample] * rotator->m_fCos3Gamma;
            valueO = -channelpart3[5][niSample] * rotator->m_fSin2Gamma + channelpart3[1][niSample] * rotator->m_fCos2Gamma;
            valueM = -channelpart3[4][niSample] * rotator->m_fSinGamma + channelpart3[2][niSample] * rotator->m_fCosGamma;
            valueK = channelpart3[3][niSample];
            valueL = channelpart3[4][niSample] * rotator->m_fCosGamma + channelpart3[2][niSample] * rotator->m_fSinGamma;
            valueN = channelpart3[5][niSample] * rotator->m_fCos2Gamma + channelpart3[1][niSample] * rotator->m_fSin2Gamma;
            valueP = channelpart3[6][niSample] * rotator->m_fCos3Gamma + channelpart3[0][niSample] * rotator->m_fSin3Gamma;
            
            channelpart3[0][niSample] = valueQ;
            channelpart3[1][niSample] = valueO;
            channelpart3[2][niSample] = valueM;
            channelpart3[3][niSample] = valueK;
            channelpart3[4][niSample] = valueL;
            channelpart3[5][niSample] = valueN;
            channelpart3[6][niSample] = valueP;
        }

        // This is the parallel version
        // void* thisNode = __hpvm__getNode();
        // long niSample = __hpvm__getNodeInstanceID_x(thisNode);

        // if (niSample < nSample) {
        //     valueQ = -channelpart3[6][niSample] * rotator->m_fSin3Alpha + channelpart3[0][niSample] * rotator->m_fCos3Alpha;
        //     valueO = -channelpart3[5][niSample] * rotator->m_fSin2Alpha + channelpart3[1][niSample] * rotator->m_fCos2Alpha;
        //     valueM = -channelpart3[4][niSample] * rotator->m_fSinAlpha + channelpart3[2][niSample] * rotator->m_fCosAlpha;
        //     valueK = channelpart3[3][niSample];
        //     valueL = channelpart3[4][niSample] * rotator->m_fCosAlpha + channelpart3[2][niSample] * rotator->m_fSinAlpha;
        //     valueN = channelpart3[5][niSample] * rotator->m_fCos2Alpha + channelpart3[1][niSample] * rotator->m_fSin2Alpha;
        //     valueP = channelpart3[6][niSample] * rotator->m_fCos3Alpha + channelpart3[0][niSample] * rotator->m_fSin3Alpha;

        //     channelpart3[0][niSample] = 0.125f * valueQ * (5.f + 3.f * rotator->m_fCos2Beta) - fSqrt3_2 * valueO * rotator->m_fCosBeta * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueM * pow(rotator->m_fSinBeta, 2.0f);
        //     channelpart3[1][niSample] = valueO * rotator->m_fCos2Beta - fSqrt5_2 * valueM * rotator->m_fCosBeta * rotator->m_fSinBeta + fSqrt3_2 * valueQ * rotator->m_fCosBeta * rotator->m_fSinBeta; 
        //     channelpart3[2][niSample] = 0.125f * valueM * (3.f + 5.f * rotator->m_fCos2Beta) - fSqrt5_2 * valueO * rotator->m_fCosBeta * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueQ * pow(rotator->m_fSinBeta, 2.0f);
        //     channelpart3[3][niSample] = 0.25f * valueK * rotator->m_fCosBeta * (-1.f + 15.f * rotator->m_fCos2Beta) + 0.5f * fSqrt15 * valueN * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) + 0.5f * fSqrt5_2 * valueP * pow(rotator->m_fSinBeta, 3.f) + 0.125f * fSqrt3_2 * valueL * (rotator->m_fSinBeta + 5.f * rotator->m_fSin3Beta);
        //     channelpart3[4][niSample] = 0.0625f * valueL * (rotator->m_fCosBeta + 15.f * rotator->m_fCos3Beta) + 0.25f * fSqrt5_2 * valueN * (1.f + 3.f * rotator->m_fCos2Beta) * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueP * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) - 0.125 * fSqrt3_2 * valueK * (rotator->m_fSinBeta + 5.f * rotator->m_fSin3Beta);
        //     channelpart3[5][niSample] = 0.125f * valueN * (5.f * rotator->m_fCosBeta + 3.f * rotator->m_fCos3Beta) + 0.25f * fSqrt3_2 * valueP * (3.f + rotator->m_fCos2Beta) * rotator->m_fSinBeta + 0.5f * fSqrt15 * valueK * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) + 0.125 * fSqrt5_2 * valueL * (rotator->m_fSinBeta - 3.f * rotator->m_fSin3Beta);
        //     channelpart3[6][niSample] = 0.0625f * valueP * (15.f * rotator->m_fCosBeta + rotator->m_fCos3Beta) - 0.25f * fSqrt3_2 * valueN * (3.f + rotator->m_fCos2Beta) * rotator->m_fSinBeta + 0.25f * fSqrt15 * valueL * rotator->m_fCosBeta * pow(rotator->m_fSinBeta, 2.f) - 0.5 * fSqrt5_2 * valueK * pow(rotator->m_fSinBeta, 3.f);

        //     valueQ = -channelpart3[6][niSample] * rotator->m_fSin3Gamma + channelpart3[0][niSample] * rotator->m_fCos3Gamma;
        //     valueO = -channelpart3[5][niSample] * rotator->m_fSin2Gamma + channelpart3[1][niSample] * rotator->m_fCos2Gamma;
        //     valueM = -channelpart3[4][niSample] * rotator->m_fSinGamma + channelpart3[2][niSample] * rotator->m_fCosGamma;
        //     valueK = channelpart3[3][niSample];
        //     valueL = channelpart3[4][niSample] * rotator->m_fCosGamma + channelpart3[2][niSample] * rotator->m_fSinGamma;
        //     valueN = channelpart3[5][niSample] * rotator->m_fCos2Gamma + channelpart3[1][niSample] * rotator->m_fSin2Gamma;
        //     valueP = channelpart3[6][niSample] * rotator->m_fCos3Gamma + channelpart3[0][niSample] * rotator->m_fSin3Gamma;
            
        //     channelpart3[0][niSample] = valueQ;
        //     channelpart3[1][niSample] = valueO;
        //     channelpart3[2][niSample] = valueM;
        //     channelpart3[3][niSample] = valueK;
        //     channelpart3[4][niSample] = valueL;
        //     channelpart3[5][niSample] = valueN;
        //     channelpart3[6][niSample] = valueP;
        // }

        
    }
    // printf("rotateOrder3 ends\n");

    __hpvm__return(1, bytes_channelpart3);
}

void wrapperRotateOrder3_fxp(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ long nSample, /*3*/ float** channelpart3, /*4*/ size_t bytes_channelpart3) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(2, rotator, channelpart3, 1, channelpart3);

    // void* ro3Node = __hpvm__createNodeND(1, rotateOrder3_fxp, nSample);
    void* ro3Node = __hpvm__createNodeND(0, rotateOrder3_fxp);

    __hpvm__bindIn(ro3Node, 0, 0, 0);
    __hpvm__bindIn(ro3Node, 1, 1, 0);
    __hpvm__bindIn(ro3Node, 2, 2, 0);
    __hpvm__bindIn(ro3Node, 3, 3, 0);
    __hpvm__bindIn(ro3Node, 4, 4, 0);

    __hpvm__bindOut(ro3Node, 0, 0, 0);
}

// A leaf node for the zoomer setting
void zoomSet_fxp(/*0*/ CAmbisonicZoomer* zoomer, /*1*/ size_t bytes_zoomer) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(1, zoomer, 1, zoomer);  // zoomer = audioAddr->zoomer

    // printf("zoomSet -> zoomProcess: zoomer: %zu\n", bytes_zoomer);

    // printf("zoomSet starts\n");
    static int frame = 0;
	frame++;
	zoomer->SetZoom(sinf(frame/100));
	zoomer->Refresh();
    // printf("zoomSet ends\n");

    __hpvm__return(1, bytes_zoomer);
}

void wrapperZoomSet_fxp(/*0*/ CAmbisonicZoomer* zoomer, /*1*/ size_t bytes_zoomer) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(1, zoomer, 1, zoomer);  // zoomer = audioAddr->zoomer

    void* zsNode = __hpvm__createNodeND(0, zoomSet_fxp);

    __hpvm__bindIn(zsNode, 0, 0, 0);
    __hpvm__bindIn(zsNode, 1, 1, 0);

    __hpvm__bindOut(zsNode, 0, 0, 0);
}

// A leaf node for the zoomer processing
// void zoomProcess_fxp(/*0*/ CAmbisonicZoomer* zoomer, /*1*/ size_t bytes_zoomer, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ unsigned nSample) {
//     __hpvm__hint(hpvm::DEVICE);
//     __hpvm__attributes(2, zoomer, sumBF, 1, sumBF);

//     // printf("zoomProcess starts\n");
//     zoomer->Process(sumBF, nSample);
//     // printf("zoomProcess ends\n");
    
//     __hpvm__return(1, bytes_sumBF);
// }

void zoomProcess_fxp(/*0*/ CAmbisonicZoomer* zoomer, /*1*/ size_t bytes_zoomer, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ long nSample, /*5*/ float** channelpart1, /*6*/ size_t bytes_channelpart1, /*7*/ float** channelpart2, /*8*/ size_t bytes_channelpart2, /*9*/ float** channelpart3, /*10*/ size_t bytes_channelpart3) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(5, zoomer, sumBF, channelpart1, channelpart2, channelpart3, 1, sumBF);

    // printf("zoomProcess -> setAndFFT_left: sumBF: %zu\n", bytes_sumBF);
    // printf("zoomProcess -> setAndFFT_right: sumBF: %zu\n", bytes_sumBF);

    // printf("zoomProcess starts\n");
    for (unsigned i = 0; i < nSample; ++i) {
        sumBF->m_ppfChannels[kX][i] = channelpart1[0][i];
        sumBF->m_ppfChannels[kY][i] = channelpart1[1][i];
        sumBF->m_ppfChannels[kZ][i] = channelpart1[2][i];

        sumBF->m_ppfChannels[kR][i] = channelpart2[0][i];
        sumBF->m_ppfChannels[kS][i] = channelpart2[1][i];
        sumBF->m_ppfChannels[kT][i] = channelpart2[2][i];
        sumBF->m_ppfChannels[kU][i] = channelpart2[3][i];
        sumBF->m_ppfChannels[kV][i] = channelpart2[4][i];

        sumBF->m_ppfChannels[kQ][i] = channelpart3[0][i];
        sumBF->m_ppfChannels[kO][i] = channelpart3[1][i];
        sumBF->m_ppfChannels[kM][i] = channelpart3[2][i];
        sumBF->m_ppfChannels[kK][i] = channelpart3[3][i];
        sumBF->m_ppfChannels[kL][i] = channelpart3[4][i];
        sumBF->m_ppfChannels[kN][i] = channelpart3[5][i];
        sumBF->m_ppfChannels[kP][i] = channelpart3[6][i];
    }

    // printf("zoomProcess starts\n");
    zoomer->Process(sumBF, nSample);
    // printf("zoomProcess ends\n");
    
    __hpvm__return(2, bytes_sumBF, bytes_sumBF);
}

void wrapperZoomProcess_fxp(/*0*/ CAmbisonicZoomer* zoomer, /*1*/ size_t bytes_zoomer, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ long nSample, \
                            /*5*/ float** channelpart1, /*6*/ size_t bytes_channelpart1, /*7*/ float** channelpart2, /*8*/ size_t bytes_channelpart2, /*9*/ float** channelpart3, /*10*/ size_t bytes_channelpart3) {
    __hpvm__hint(hpvm::DEVICE);
    __hpvm__attributes(5, zoomer, sumBF, channelpart1, channelpart2, channelpart3, 1, sumBF);

    void* zpNode = __hpvm__createNodeND(0, zoomProcess_fxp);

    __hpvm__bindIn(zpNode, 0, 0, 0);
    __hpvm__bindIn(zpNode, 1, 1, 0);
    __hpvm__bindIn(zpNode, 2, 2, 0);
    __hpvm__bindIn(zpNode, 3, 3, 0);
    __hpvm__bindIn(zpNode, 4, 4, 0);
    __hpvm__bindIn(zpNode, 5, 5, 0);
    __hpvm__bindIn(zpNode, 6, 6, 0);
    __hpvm__bindIn(zpNode, 7, 7, 0);
    __hpvm__bindIn(zpNode, 8, 8, 0);
    __hpvm__bindIn(zpNode, 9, 9, 0);
    __hpvm__bindIn(zpNode, 10, 10, 0);

    __hpvm__bindOut(zpNode, 0, 0, 0);
    __hpvm__bindOut(zpNode, 1, 1, 0);
}

// A leaf node for the decoder processing setting
// void decodeProcessSet_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder) {
//     __hpvm__hint(hpvm::CPU_TARGET);
//     __hpvm__attributes(1, decoder, 1, decoder);

//     // decoder->m_pfScratchBufferA.resize(m_nFFTSize);
//     // for (int i = 0; i < m_nFFTSize; ++i) {
//     //     decoder->m_pfScratchBufferA[i] = 0;
//     // }
//     memset(decoder->m_pfScratchBufferA.data(), 0, (decoder->m_nFFTSize) * sizeof(float));

//     __hpvm__return(1, bytes_decoder);
// }

// A leaf node for the memcpy and memset and the FFT part
void setAndFFT_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ CBFormat* sumBF, \
                        /*3*/ size_t bytes_sumBF) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, sumBF, 1, decoder);

    // printf("setAndFFT_left -> FIR_left: decoder: %zu\n", bytes_decoder);

    // printf("setAndFFT_left starts\n");
    memset(decoder->m_pfScratchBufferA.data(), 0, (decoder->m_nFFTSize) * sizeof(float));

    for (unsigned niChannel = 0; niChannel < decoder->m_nChannelCount; niChannel++) {
        memcpy(decoder->m_pfScratchBufferB.data(), sumBF->m_ppfChannels[niChannel], decoder->m_nBlockSize * sizeof(float));
        memset(&(decoder->m_pfScratchBufferB[decoder->m_nBlockSize]), 0, (decoder->m_nFFTSize - decoder->m_nBlockSize) * sizeof(float));
        kiss_fftr(decoder->m_pFFT_cfg.get(), decoder->m_pfScratchBufferB.data(), decoder->m_pcpScratch.get());
    }
    // printf("setAndFFT ends\n");

    __hpvm__return(1, bytes_decoder);
}

void wrapperSetAndFFT_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ CBFormat* sumBF, \
                                /*3*/ size_t bytes_sumBF) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, sumBF, 1, decoder);

    void* sfftlNode = __hpvm__createNodeND(0, setAndFFT_left_fxp);

    __hpvm__bindIn(sfftlNode, 0, 0, 0);
    __hpvm__bindIn(sfftlNode, 1, 1, 0);
    __hpvm__bindIn(sfftlNode, 2, 2, 0);
    __hpvm__bindIn(sfftlNode, 3, 3, 0);

    __hpvm__bindOut(sfftlNode, 0, 0, 0);
}

void setAndFFT_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ CBFormat* sumBF, \
                        /*3*/ size_t bytes_sumBF) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, sumBF, 1, decoder);

    // printf("setAndFFT_right -> FIR_right: decoder: %zu\n", bytes_decoder);

    // printf("setAndFFT_right starts\n");
    memset(decoder->m_pfScratchBufferA.data(), 0, (decoder->m_nFFTSize) * sizeof(float));

    for (unsigned niChannel = 0; niChannel < decoder->m_nChannelCount; niChannel++) {
        memcpy(decoder->m_pfScratchBufferB.data(), sumBF->m_ppfChannels[niChannel], decoder->m_nBlockSize * sizeof(float));
        memset(&(decoder->m_pfScratchBufferB[decoder->m_nBlockSize]), 0, (decoder->m_nFFTSize - decoder->m_nBlockSize) * sizeof(float));
        kiss_fftr(decoder->m_pFFT_cfg.get(), decoder->m_pfScratchBufferB.data(), decoder->m_pcpScratch.get());
    }
    // printf("setAndFFT ends\n");

    __hpvm__return(1, bytes_decoder);
}

void wrapperSetAndFFT_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, sumBF, 1, decoder);

    void* sfftrNode = __hpvm__createNodeND(0, setAndFFT_right_fxp);

    __hpvm__bindIn(sfftrNode, 0, 0, 0);
    __hpvm__bindIn(sfftrNode, 1, 1, 0);
    __hpvm__bindIn(sfftrNode, 2, 2, 0);
    __hpvm__bindIn(sfftrNode, 3, 3, 0);

    __hpvm__bindOut(sfftrNode, 0, 0, 0);
}

// FFT
// void FFT_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder) {
//     __hpvm__hint(hpvm::CPU_TARGET);
//     __hpvm__attributes(1, decoder, 1, decoder);

//     kiss_fftr(decoder->m_pFFT_cfg.get(), decoder->m_pfScratchBufferB.data(), decoder->m_pcpScratch.get());

//     __hpvm__return(1, bytes_decoder);
// }

// FIR Filter
void FIR_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ long decoder0ChannelCount, /*3*/ long decoder0FFTBins) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);

    // printf("FIR_left -> IFFT_left: decoder: %zu\n", bytes_decoder);

    // printf("FIR_left starts\n");
    kiss_fft_cpx cpTemp[decoder->m_nFFTBins];

    // This is the non-parallel version
    for (unsigned niChannel = 0; niChannel < decoder0ChannelCount; niChannel++) {
        for (int ni = 0; ni < decoder0FFTBins; ni++) {
            cpTemp[ni].r = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[0][niChannel][ni].r - decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[0][niChannel][ni].i;
            cpTemp[ni].i = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[0][niChannel][ni].i + decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[0][niChannel][ni].r;
            decoder->m_pcpScratch[ni] = cpTemp[ni];
        }
    }

    // This is the parallel version
    // void* thisNode = __hpvm__getNode();
    // long niChannel = __hpvm__getNodeInstanceID_x(thisNode);
    // long ni = __hpvm__getNodeInstanceID_y(thisNode);

    // if (niChannel < decoder0ChannelCount && ni < decoder0FFTBins) {
    //     cpTemp[ni].r = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[0][niChannel][ni].r - decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[0][niChannel][ni].i;
    //     cpTemp[ni].i = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[0][niChannel][ni].i + decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[0][niChannel][ni].r;
    //     decoder->m_pcpScratch[ni] = cpTemp[ni];
    // }


    // printf("FIR_left ends\n");

    __hpvm__return(1, bytes_decoder);
}

void wrapperFIR_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ long decoder0ChannelCount, /*3*/ long decoder0FFTBins) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);


    // void* firlNode = __hpvm__createNodeND(2, FIR_left_fxp, decoder0ChannelCount, decoder0FFTBins);
    void* firlNode = __hpvm__createNodeND(0, FIR_left_fxp);

    __hpvm__bindIn(firlNode, 0, 0, 0);
    __hpvm__bindIn(firlNode, 1, 1, 0);
    __hpvm__bindIn(firlNode, 2, 2, 0);
    __hpvm__bindIn(firlNode, 3, 3, 0);

    __hpvm__bindOut(firlNode, 0, 0, 0);
}

void FIR_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ long decoder1ChannelCount, /*3*/ long decoder1FFTBins) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);

    // printf("FIR_right -> IFFT_right: decoder: %zu\n", bytes_decoder);

    // printf("FIR_right starts\n");
    kiss_fft_cpx cpTemp[decoder->m_nFFTBins];

    // This is the non-parallel version
    for (unsigned niChannel = 0; niChannel < decoder->m_nChannelCount; niChannel++) {
        for (int ni = 0; ni < decoder->m_nFFTBins; ni++) {
            cpTemp[ni].r = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[1][niChannel][ni].r - decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[1][niChannel][ni].i;
            cpTemp[ni].i = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[1][niChannel][ni].i + decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[1][niChannel][ni].r;
            decoder->m_pcpScratch[ni] = cpTemp[ni];
        }
    }

    // This is the parallel version
    // void* thisNode = __hpvm__getNode();
    // long niChannel = __hpvm__getNodeInstanceID_x(thisNode);
    // long ni = __hpvm__getNodeInstanceID_y(thisNode);

    // if (niChannel < decoder1ChannelCount && ni < decoder1FFTBins) {
    //     cpTemp[ni].r = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[1][niChannel][ni].r - decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[1][niChannel][ni].i;
    //     cpTemp[ni].i = decoder->m_pcpScratch[ni].r * decoder->m_ppcpFilters[1][niChannel][ni].i + decoder->m_pcpScratch[ni].i * decoder->m_ppcpFilters[1][niChannel][ni].r;
    //     decoder->m_pcpScratch[ni] = cpTemp[ni];
    // }



    // printf("FIR_right ends\n");

    __hpvm__return(1, bytes_decoder);
}

void wrapperFIR_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ long decoder1ChannelCount, /*3*/ long decoder1FFTBins) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);

    // void* firrNode = __hpvm__createNodeND(2, FIR_right_fxp, decoder1ChannelCount, decoder1FFTBins);
    void* firrNode = __hpvm__createNodeND(0, FIR_right_fxp);

    __hpvm__bindIn(firrNode, 0, 0, 0);
    __hpvm__bindIn(firrNode, 1, 1, 0);
    __hpvm__bindIn(firrNode, 2, 2, 0);
    __hpvm__bindIn(firrNode, 3, 3, 0);

    __hpvm__bindOut(firrNode, 0, 0, 0);
}

// IFFT
void IFFT_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);

    // printf("IFFT_left -> overlap_left: decoder: %zu\n", bytes_decoder);

    // printf("IFFT_left starts\n");
    // printf("%d\n", decoder->m_nChannelCount);
    for (unsigned niChannel = 0; niChannel < decoder->m_nChannelCount; niChannel++) {
        kiss_fftri(decoder->m_pIFFT_cfg.get(), decoder->m_pcpScratch.get(), decoder->m_pfScratchBufferB.data());
        // printf("Np with kiss_fftri\n");
        for(unsigned ni = 0; ni < decoder->m_nFFTSize; ni++)
            decoder->m_pfScratchBufferA[ni] += decoder->m_pfScratchBufferB[ni];
    }    
    // printf("IFFT_left ends\n");

    __hpvm__return(1, bytes_decoder);
}

void wrapperIFFT_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);

    void* ifftlNode = __hpvm__createNodeND(0, IFFT_left_fxp);

    __hpvm__bindIn(ifftlNode, 0, 0, 0);
    __hpvm__bindIn(ifftlNode, 1, 1, 0);

    __hpvm__bindOut(ifftlNode, 0, 0, 0);
}

void IFFT_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);

    // printf("IFFT_right -> overlap_right: decoder: %zu\n", bytes_decoder);

    // printf("IFFT_right starts\n");
    for (unsigned niChannel = 0; niChannel < decoder->m_nChannelCount; niChannel++) {
        kiss_fftri(decoder->m_pIFFT_cfg.get(), decoder->m_pcpScratch.get(), decoder->m_pfScratchBufferB.data());
        for(unsigned ni = 0; ni < decoder->m_nFFTSize; ni++)
            decoder->m_pfScratchBufferA[ni] += decoder->m_pfScratchBufferB[ni];        
    }
    // printf("IFFT_right ends\n");

    __hpvm__return(1, bytes_decoder);
}

void wrapperIFFT_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(1, decoder, 1, decoder);

    void* ifftrNode = __hpvm__createNodeND(0, IFFT_right_fxp);

    __hpvm__bindIn(ifftrNode, 0, 0, 0);
    __hpvm__bindIn(ifftrNode, 1, 1, 0);

    __hpvm__bindOut(ifftrNode, 0, 0, 0);
}

// Overlap
void overlap_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ float* resultSample0, /*3*/ size_t bytes_resultSample0) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, resultSample0, 1, resultSample0);

    // printf("overlap_left -> exit: resultSample0: %zu\n", bytes_resultSample0);

    // printf("overlap_left starts\n");
    for(int ni = 0; ni < decoder->m_nFFTSize; ni++) {
        decoder->m_pfScratchBufferA[ni] *= (decoder->m_fFFTScaler);
    }

    memcpy(resultSample0, decoder->m_pfScratchBufferA.data(), (decoder->m_nBlockSize) * sizeof(float));
    for(int ni = 0; ni < decoder->m_nOverlapLength; ni++) {
        resultSample0[ni] += (decoder->m_pfOverlap)[0][ni];
    }
    memcpy((decoder->m_pfOverlap[0]).data(), &(decoder->m_pfScratchBufferA[decoder->m_nBlockSize]), (decoder->m_nOverlapLength) * sizeof(float));
    // printf("overlap_left ends\n");


    __hpvm__return(1, bytes_resultSample0);
    
}

void wrapperOverlap_left_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ float* resultSample0, /*3*/ size_t bytes_resultSample0) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, resultSample0, 1, resultSample0);

    void* overLeftNode = __hpvm__createNodeND(0, overlap_left_fxp);

    __hpvm__bindIn(overLeftNode, 0, 0, 0);
    __hpvm__bindIn(overLeftNode, 1, 1, 0);
    __hpvm__bindIn(overLeftNode, 2, 2, 0);
    __hpvm__bindIn(overLeftNode, 3, 3, 0);

    __hpvm__bindOut(overLeftNode, 0, 0, 0);
}

void overlap_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ float* resultSample1, /*3*/ size_t bytes_resultSample1) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, resultSample1, 1, resultSample1);

    // printf("overlap_right -> exit: resultSample1: %zu\n", bytes_resultSample1);

    // printf("overlap_right starts\n");
    for(int ni = 0; ni < decoder->m_nFFTSize; ni++) {
        decoder->m_pfScratchBufferA[ni] *= (decoder->m_fFFTScaler);
    }

    memcpy(resultSample1, decoder->m_pfScratchBufferA.data(), (decoder->m_nBlockSize) * sizeof(float));
    for(int ni = 0; ni < decoder->m_nOverlapLength; ni++) {
        resultSample1[ni] += (decoder->m_pfOverlap)[0][ni];
    }
    memcpy((decoder->m_pfOverlap[1]).data(), &(decoder->m_pfScratchBufferA[decoder->m_nBlockSize]), (decoder->m_nOverlapLength) * sizeof(float));
    // printf("overlap_right ends\n");
    


    __hpvm__return(1, bytes_resultSample1);
    
}

void wrapperOverlap_right_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder, /*2*/ float* resultSample1, /*3*/ size_t bytes_resultSample1) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(2, decoder, resultSample1, 1, resultSample1);

    void* overRightNode = __hpvm__createNodeND(0, overlap_right_fxp);

    __hpvm__bindIn(overRightNode, 0, 0, 0);
    __hpvm__bindIn(overRightNode, 1, 1, 0);
    __hpvm__bindIn(overRightNode, 2, 2, 0);
    __hpvm__bindIn(overRightNode, 3, 3, 0);

    __hpvm__bindOut(overRightNode, 0, 0, 0);
}

// An internal node for the FFT-FIR-IFFT flow
// void channelWrapper_fxp(/*0*/ CAmbisonicBinauralizer* decoder, /*1*/ size_t bytes_decoder) {
//     __hpvm__hint(hpvm::CPU_TARGET);
//     __hpvm__attributes(1, decoder, 1, decoder);

//     void* FFTNode = __hpvm__createNodeND(0, FFT_fxp);
//     void* FIRNode = __hpvm__createNodeND(0, FIR_fxp);
//     void* IFFTNode = __hpvm__createNodeND(0, IFFT_fxp);

//     __hpvm__bindIn(FFTNode, 0, 0, 0);
//     __hpvm__bindIn(FFTNode, 1, 1, 0);

//     __hpvm__bindIn(FIRNode, 0, 0, 0);
//     __hpvm__edge(FFTNode, FIRNode, 1, 0, 1, 0);

//     __hpvm__bindIn(IFFTNode, 0, 0, 0);
//     __hpvm__edge(FIRNode, IFFTNode, 1, 0, 1, 0);

//     __hpvm__bindOut(IFFTNode, 0, 0, 0);
// }

// The root node
void audioDecoding(/*0*/ CAmbisonicProcessor* rotator, /*1*/ size_t bytes_rotator, /*2*/ CBFormat* sumBF, /*3*/ size_t bytes_sumBF, /*4*/ CAmbisonicZoomer* zoomer, \
                    /*5*/ size_t bytes_zoomer, /*6*/ CAmbisonicBinauralizer* decoder0, /*7*/ size_t bytes_decoder0, /*8*/ float* resultSample0, \
                    /*9*/ size_t bytes_resultSample0, /*10*/ long nSample, /*11*/ float** channelpart1, /*12*/ size_t bytes_channelpart1, /*13*/ float** channelpart2, \
                    /*14*/ size_t bytes_channelpart2, /*15*/ float** channelpart3, /*16*/ size_t bytes_channelpart3, /*17*/ CAmbisonicBinauralizer* decoder1, \
                    /*18*/ size_t bytes_decoder1, /*19*/ float* resultSample1, /*20*/ size_t bytes_resultSample1, /*21*/ long decoder0ChannelCount, /*22*/ long decoder0FFTBins, \
                    /*23*/ long decoder1ChannelCount, /*24*/ long decoder1FFTBins) {
    __hpvm__hint(hpvm::CPU_TARGET);
    __hpvm__attributes(7, rotator, sumBF, zoomer, decoder0, decoder1, resultSample0, resultSample1, 6, sumBF, zoomer, decoder0, decoder1, resultSample0, resultSample1);

    // void* rotatorNode = __hpvm__createNodeND(0, rotator_fxp);
    // void* rotatorSetNode = __hpvm__createNodeND(0, rotatorSet_fxp);
    // void* psychoFilterNode = __hpvm__createNodeND(0, psychoFilter_fxp);
    // void* rotateOrder1Node = __hpvm__createNodeND(0, rotateOrder1_fxp);
    // void* rotateOrder2Node = __hpvm__createNodeND(0, rotateOrder2_fxp);
    // void* rotateOrder3Node = __hpvm__createNodeND(0, rotateOrder3_fxp);
    // void* zoomSetNode = __hpvm__createNodeND(0, zoomSet_fxp);
    // void* zoomProcessNode = __hpvm__createNodeND(0, zoomProcess_fxp);
    // // void* decodeProcessSetNode = __hpvm__createNodeND(0, decodeProcessSet_fxp);
    // // void* bufferBCpyNSetNode = __hpvm__createNodeND(0, bufferBCpyNSet_fxp);
    // // void* channelWrapperNode = __hpvm__createNodeND(channelCount, channelWrapper_fxp);
    // // void* overlapNode = __hpvm__createNodeND(0, overlap_fxp);
    // void* setAndFFT_LeftNode = __hpvm__createNodeND(0, setAndFFT_left_fxp);
    // void* setAndFFT_RightNode = __hpvm__createNodeND(0, setAndFFT_right_fxp);
    // void* FIR_LeftNode= __hpvm__createNodeND(0, FIR_left_fxp);
    // void* FIR_RightNode= __hpvm__createNodeND(0, FIR_right_fxp);
    // void* IFFT_LeftNode= __hpvm__createNodeND(0, IFFT_left_fxp);
    // void* IFFT_RightNode= __hpvm__createNodeND(0, IFFT_right_fxp);
    // void* overlap_LeftNode= __hpvm__createNodeND(0, overlap_left_fxp);
    // void* overlap_RightNode= __hpvm__createNodeND(0, overlap_right_fxp);

    void* rotatorSetNode = __hpvm__createNodeND(0, wrapperRotatorSet_fxp);
    void* psychoFilterNode = __hpvm__createNodeND(0, wrapperPsychoFilter_fxp);
    void* rotateOrder1Node = __hpvm__createNodeND(0, wrapperRotateOrder1_fxp);
    void* rotateOrder2Node = __hpvm__createNodeND(0, wrapperRotateOrder2_fxp);
    void* rotateOrder3Node = __hpvm__createNodeND(0, wrapperRotateOrder3_fxp);
    void* zoomSetNode = __hpvm__createNodeND(0, wrapperZoomSet_fxp);
    void* zoomProcessNode = __hpvm__createNodeND(0, wrapperZoomProcess_fxp);
    // void* decodeProcessSetNode = __hpvm__createNodeND(0, decodeProcessSet_fxp);
    // void* bufferBCpyNSetNode = __hpvm__createNodeND(0, bufferBCpyNSet_fxp);
    // void* channelWrapperNode = __hpvm__createNodeND(channelCount, channelWrapper_fxp);
    // void* overlapNode = __hpvm__createNodeND(0, overlap_fxp);
    void* setAndFFT_LeftNode = __hpvm__createNodeND(0, wrapperSetAndFFT_left_fxp);
    void* setAndFFT_RightNode = __hpvm__createNodeND(0, wrapperSetAndFFT_right_fxp);
    void* FIR_LeftNode= __hpvm__createNodeND(0, wrapperFIR_left_fxp);
    void* FIR_RightNode= __hpvm__createNodeND(0, wrapperFIR_right_fxp);
    void* IFFT_LeftNode= __hpvm__createNodeND(0, wrapperIFFT_left_fxp);
    void* IFFT_RightNode= __hpvm__createNodeND(0, wrapperIFFT_right_fxp);
    void* overlap_LeftNode= __hpvm__createNodeND(0, wrapperOverlap_left_fxp);
    void* overlap_RightNode= __hpvm__createNodeND(0, wrapperOverlap_right_fxp);



    // __hpvm__bindIn(rotatorNode, 0, 0, 0);
    // __hpvm__bindIn(rotatorNode, 1, 1, 0);
    // __hpvm__bindIn(rotatorNode, 2, 2, 0);
    // __hpvm__bindIn(rotatorNode, 3, 3, 0);
    // __hpvm__bindIn(rotatorNode, 10, 4, 0);

    __hpvm__bindIn(rotatorSetNode, 0, 0, 1);
    __hpvm__bindIn(rotatorSetNode, 1, 1, 1);

    __hpvm__bindIn(psychoFilterNode, 0, 0, 1);
    __hpvm__edge(rotatorSetNode, psychoFilterNode, 1, 0, 1, 1);
    __hpvm__bindIn(psychoFilterNode, 2, 2, 1);
    __hpvm__bindIn(psychoFilterNode, 3, 3, 1);
    __hpvm__bindIn(psychoFilterNode, 10, 4, 1);
    __hpvm__bindIn(psychoFilterNode, 11, 5, 1);
    __hpvm__bindIn(psychoFilterNode, 12, 6, 1);
    __hpvm__bindIn(psychoFilterNode, 13, 7, 1);
    __hpvm__bindIn(psychoFilterNode, 14, 8, 1);
    __hpvm__bindIn(psychoFilterNode, 15, 9, 1);
    __hpvm__bindIn(psychoFilterNode, 16, 10, 1);

    // __hpvm__bindIn(rotateOrder1Node, 0, 0, 0);
    // __hpvm__edge(psychoFilterNode, rotateOrder1Node, 1, 0, 1, 0);
    // __hpvm__bindIn(rotateOrder1Node, 2, 2, 0);
    // __hpvm__edge(psychoFilterNode, rotateOrder1Node, 1, 1, 3, 0);
    // __hpvm__bindIn(rotateOrder1Node, 10, 4, 0);

    // __hpvm__bindIn(rotateOrder2Node, 0, 0, 0);
    // __hpvm__edge(rotateOrder1Node, rotateOrder2Node, 1, 0, 1, 0);
    // __hpvm__bindIn(rotateOrder2Node, 2, 2, 0);
    // __hpvm__edge(rotateOrder1Node, rotateOrder2Node, 1, 1, 3, 0);
    // __hpvm__bindIn(rotateOrder2Node, 10, 4, 0);

    // __hpvm__bindIn(rotateOrder3Node, 0, 0, 0);
    // __hpvm__edge(rotateOrder2Node, rotateOrder3Node, 1, 0, 1, 0);
    // __hpvm__bindIn(rotateOrder3Node, 2, 2, 0);
    // __hpvm__edge(rotateOrder2Node, rotateOrder3Node, 1, 1, 3, 0);
    // __hpvm__bindIn(rotateOrder3Node, 10, 4, 0);

    __hpvm__bindIn(rotateOrder1Node, 0, 0, 1);
    __hpvm__edge(psychoFilterNode, rotateOrder1Node, 1, 0, 1, 1);
    __hpvm__bindIn(rotateOrder1Node, 10, 2, 1);
    __hpvm__bindIn(rotateOrder1Node, 11, 3, 1);
    __hpvm__edge(psychoFilterNode, rotateOrder1Node, 1, 4, 4, 1);

    __hpvm__bindIn(rotateOrder2Node, 0, 0, 1);
    __hpvm__edge(psychoFilterNode, rotateOrder2Node, 1, 1, 1, 1);
    __hpvm__bindIn(rotateOrder2Node, 10, 2, 1);
    __hpvm__bindIn(rotateOrder2Node, 13, 3, 1);
    __hpvm__edge(psychoFilterNode, rotateOrder2Node, 1, 5, 4, 1);

    __hpvm__bindIn(rotateOrder3Node, 0, 0, 1);
    __hpvm__edge(psychoFilterNode, rotateOrder3Node, 1, 2, 1, 1);
    __hpvm__bindIn(rotateOrder3Node, 10, 2, 1);
    __hpvm__bindIn(rotateOrder3Node, 15, 3, 1);
    __hpvm__edge(psychoFilterNode, rotateOrder3Node, 1, 6, 4, 1);

    __hpvm__bindIn(zoomSetNode, 4, 0, 1);
    __hpvm__bindIn(zoomSetNode, 5, 1, 1);

    __hpvm__bindIn(zoomProcessNode, 4, 0, 1);
    __hpvm__edge(zoomSetNode, zoomProcessNode, 1, 0, 1, 1);
    __hpvm__bindIn(zoomProcessNode, 2, 2, 1);
    // __hpvm__edge(rotateOrder3Node, zoomProcessNode, 1, 0, 3, 0);
    // __hpvm__bindIn(zoomProcessNode, 10, 4, 0);
    __hpvm__edge(psychoFilterNode, zoomProcessNode, 1, 3, 3, 1);
    __hpvm__bindIn(zoomProcessNode, 10, 4, 1);
    __hpvm__bindIn(zoomProcessNode, 11, 5, 1);
    __hpvm__edge(rotateOrder1Node, zoomProcessNode, 1, 0, 6, 1);
    __hpvm__bindIn(zoomProcessNode, 13, 7, 1);
    __hpvm__edge(rotateOrder2Node, zoomProcessNode, 1, 0, 8, 1);
    __hpvm__bindIn(zoomProcessNode, 15, 9, 1);
    __hpvm__edge(rotateOrder3Node, zoomProcessNode, 1, 0, 10, 1);

    // __hpvm__bindIn(decodeProcessSetNode, 6, 0, 0);
    // __hpvm__bindIn(decodeProcessSetNode, 7, 1, 0);

    __hpvm__bindIn(setAndFFT_LeftNode, 6, 0, 1);
    __hpvm__bindIn(setAndFFT_LeftNode, 7, 1, 1);
    __hpvm__bindIn(setAndFFT_LeftNode, 2, 2, 1);
    __hpvm__edge(zoomProcessNode, setAndFFT_LeftNode, 1, 0, 3, 1);

    __hpvm__bindIn(setAndFFT_RightNode, 17, 0, 1);
    __hpvm__bindIn(setAndFFT_RightNode, 18, 1, 1);
    __hpvm__bindIn(setAndFFT_RightNode, 2, 2, 1);
    __hpvm__edge(zoomProcessNode, setAndFFT_RightNode, 1, 1, 3, 1);
    
    __hpvm__bindIn(FIR_LeftNode, 6, 0, 1);
    __hpvm__edge(setAndFFT_LeftNode, FIR_LeftNode, 1, 0, 1, 1);
    __hpvm__bindIn(FIR_LeftNode, 21, 2, 1);
    __hpvm__bindIn(FIR_LeftNode, 22, 3, 1);
    
    __hpvm__bindIn(FIR_RightNode, 17, 0, 1);
    __hpvm__edge(setAndFFT_RightNode, FIR_RightNode, 1, 0, 1, 1);
    __hpvm__bindIn(FIR_RightNode, 23, 2, 1);
    __hpvm__bindIn(FIR_RightNode, 24, 3, 1);

    __hpvm__bindIn(IFFT_LeftNode, 6, 0, 1);
    __hpvm__edge(FIR_LeftNode, IFFT_LeftNode, 1, 0, 1, 1);

    __hpvm__bindIn(IFFT_RightNode, 17, 0, 1);
    __hpvm__edge(FIR_RightNode, IFFT_RightNode, 1, 0, 1, 1);

    __hpvm__bindIn(overlap_LeftNode, 6, 0, 1);
    __hpvm__edge(IFFT_LeftNode, overlap_LeftNode, 1, 0, 1, 1);
    __hpvm__bindIn(overlap_LeftNode, 8, 2, 1);
    __hpvm__bindIn(overlap_LeftNode, 9, 3, 1);

    __hpvm__bindIn(overlap_RightNode, 17, 0, 1);
    __hpvm__edge(IFFT_RightNode, overlap_RightNode, 1, 0, 1, 1);
    __hpvm__bindIn(overlap_RightNode, 19, 2, 1);
    __hpvm__bindIn(overlap_RightNode, 20, 3, 1);

    __hpvm__bindOut(overlap_LeftNode, 0, 0, 1);
    __hpvm__bindOut(overlap_RightNode, 0, 0, 1);

    // __hpvm__bindIn(bufferBCpyNSetNode, 6, 0, 0);
    // __hpvm__edge(decodeProcessSetNode, bufferBCpyNSetNode, 1, 0, 1, 0);
    // __hpvm__bindIn(bufferBCpyNSetNode, 2, 2, 0);
    // __hpvm__edge(zoomProcessNode, bufferBCpyNSetNode, 1, 0, 3, 0);

    // __hpvm__bindIn(channelWrapperNode, 6, 0, 0);
    // __hpvm__edge(bufferBCpyNSetNode, channelWrapperNode, 1, 0, 1, 0);

    // __hpvm__bindIn(overlapNode, 6, 0, 0);
    // __hpvm__edge(channelWrapperNode, overlapNode, 1, 0, 1, 0);
    // __hpvm__bindIn(overlapNode, 8, 2, 0);
    // __hpvm__bindIn(overlapNode, 9, 3, 0);

    // __hpvm__bindOut(overlapNode, 0, 0, 0);
}
}

int main(int argc, char const *argv[])
{
    using namespace ILLIXR_AUDIO;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <number of size 1024 blocks to process> ";
        std::cout << "<optional: encode/decode>\n";
        std::cout << "Note: If you want to hear the output sound, limit the process sample blocks so that the output is not longer than input!\n";
        return 1;
    }

    const int numBlocks = atoi(argv[1]);
    // ABAudio::ProcessType procType(ABAudio::ProcessType::FULL);
    // if (argc > 2){
    //     if (!strcmp(argv[2], "encode"))
    //         procType = ABAudio::ProcessType::ENCODE;
    //     else
    //         procType = ABAudio::ProcessType::DECODE;
    // }
    
    ABAudio audio("output.wav", ABAudio::ProcessType::DECODE);
    audio.loadSource();

    ABAudio* audioAddr = &audio;

    // float** resultSample = new float*[2];
    // resultSample[0] = new float[BLOCK_SIZE];
    // resultSample[1] = new float[BLOCK_SIZE];
    float* resultSample0 = new float[BLOCK_SIZE];
    float* resultSample1 = new float[BLOCK_SIZE];

    CBFormat* sumBF = new CBFormat;
    sumBF->Configure(NORDER, true, BLOCK_SIZE);

    float** channelpart1 = new float*[3];
    channelpart1[0] = new float[BLOCK_SIZE];
    channelpart1[1] = new float[BLOCK_SIZE];
    channelpart1[2] = new float[BLOCK_SIZE];

    float** channelpart2 = new float*[5];
    channelpart2[0] = new float[BLOCK_SIZE];
    channelpart2[1] = new float[BLOCK_SIZE];
    channelpart2[2] = new float[BLOCK_SIZE];
    channelpart2[3] = new float[BLOCK_SIZE];
    channelpart2[4] = new float[BLOCK_SIZE];

    float** channelpart3 = new float*[7];
    channelpart3[0] = new float[BLOCK_SIZE];
    channelpart3[1] = new float[BLOCK_SIZE];
    channelpart3[2] = new float[BLOCK_SIZE];
    channelpart3[3] = new float[BLOCK_SIZE];
    channelpart3[4] = new float[BLOCK_SIZE];
    channelpart3[5] = new float[BLOCK_SIZE];
    channelpart3[6] = new float[BLOCK_SIZE];

    __hpvm__init();

    size_t bytes_rotator = sizeof(CAmbisonicProcessor);
    size_t bytes_sumBF = sizeof(CBFormat);
    size_t bytes_zoomer = sizeof(CAmbisonicZoomer);
    // size_t bytes_decoder = sizeof(CAmbisonicBinauralizer);
    size_t bytes_decoder0 = sizeof(CAmbisonicBinauralizer);
    size_t bytes_decoder1 = sizeof(CAmbisonicBinauralizer);
    // size_t bytes_resultSample = 2 * BLOCK_SIZE * sizeof(float);
    size_t bytes_resultSample0 = BLOCK_SIZE * sizeof(float);
    size_t bytes_resultSample1 = BLOCK_SIZE * sizeof(float);
    size_t bytes_channelpart1 = 3 * BLOCK_SIZE * sizeof(float);
    size_t bytes_channelpart2 = 5 * BLOCK_SIZE * sizeof(float);
    size_t bytes_channelpart3 = 7 * BLOCK_SIZE * sizeof(float);

    RootArg* arg = (RootArg*)malloc(sizeof(RootArg));

    arg->rotator = audioAddr->rotator;
    arg->bytes_rotator = bytes_rotator;
    arg->sumBF = sumBF;
    arg->bytes_sumBF = bytes_sumBF;
    arg->zoomer = audioAddr->zoomer;
    arg->bytes_zoomer = bytes_zoomer;
    // arg->decoder = audioAddr->decoder;
    // arg->bytes_decoder = bytes_decoder;
    arg->decoder0 = audioAddr->decoder0;
    arg->bytes_decoder0 = bytes_decoder0;
    // arg->resultSample = resultSample;
    // arg->bytes_resultSample = bytes_resultSample;
    arg->resultSample0 = resultSample0;
    arg->bytes_resultSample0 = bytes_resultSample0;
    arg->nSample = BLOCK_SIZE;
    // arg->channelCount = audioAddr->decoder->m_nChannelCount;
    arg->channelpart1 = channelpart1;
    arg->bytes_channelpart1 = bytes_channelpart1;
    arg->channelpart2 = channelpart2;
    arg->bytes_channelpart2 = bytes_channelpart2;
    arg->channelpart3 = channelpart3;
    arg->bytes_channelpart3 = bytes_channelpart3;
    arg->decoder1 = audioAddr->decoder1;
    arg->bytes_decoder1 = bytes_decoder1;
    arg->resultSample1 = resultSample1;
    arg->bytes_resultSample1 = bytes_resultSample1;
    arg->decoder0ChannelCount = audioAddr->decoder0->m_nChannelCount;
    arg->decoder0FFTBins = audioAddr->decoder0->m_nFFTBins;
    arg->decoder1ChannelCount = audioAddr->decoder1->m_nChannelCount;
    arg->decoder1FFTBins = audioAddr->decoder1->m_nFFTBins;

    // // ABAudio* audioAddr = &audio;
    // for (int i = 0; i < numBlocks; ++i){
    //     // audio.processBlock();
    //     // printf("\nTracking!\nrotator\n");
    //     llvm_hpvm_track_mem(audioAddr->rotator, bytes_rotator);
    //     // printf("sumBF\n");
    //     llvm_hpvm_track_mem(sumBF, bytes_sumBF);
    //     // printf("zoomer\n");
    //     llvm_hpvm_track_mem(audioAddr->zoomer, bytes_zoomer);
    //     // printf("decoder\n");
    //     llvm_hpvm_track_mem(audioAddr->decoder0, bytes_decoder0);
    //     llvm_hpvm_track_mem(audioAddr->decoder1, bytes_decoder1);
    //     // printf("resultSample\n");
    //     llvm_hpvm_track_mem(resultSample0, bytes_resultSample0);
    //     llvm_hpvm_track_mem(resultSample1, bytes_resultSample1);
    //     llvm_hpvm_track_mem(channelpart1, bytes_channelpart1);
    //     llvm_hpvm_track_mem(channelpart2, bytes_channelpart2);
    //     llvm_hpvm_track_mem(channelpart3, bytes_channelpart3);
    //     // printf("Done with Tracking\n");

    //     void* DFG = __hpvm__launch(0, audioDecoding, (void*) arg);
    //     __hpvm__wait(DFG);
    //     // printf("Execution complete");

    //     // printf("\nRequesting Memory\n");
    //     llvm_hpvm_request_mem(audioAddr->rotator, bytes_rotator);
    //     llvm_hpvm_request_mem(sumBF, bytes_sumBF);
    //     llvm_hpvm_request_mem(audioAddr->zoomer, bytes_zoomer);
    //     llvm_hpvm_request_mem(audioAddr->decoder0, bytes_decoder0);
    //     llvm_hpvm_request_mem(audioAddr->decoder1, bytes_decoder1);
    //     llvm_hpvm_request_mem(resultSample0, bytes_resultSample0);
    //     llvm_hpvm_request_mem(resultSample1, bytes_resultSample1);
    //     llvm_hpvm_request_mem(channelpart1, bytes_channelpart1);
    //     llvm_hpvm_request_mem(channelpart2, bytes_channelpart2);
    //     llvm_hpvm_request_mem(channelpart3, bytes_channelpart3);
    //     // printf("\nDone requesting Memory\n");

    //     // printf("Untracking\n");
    //     llvm_hpvm_untrack_mem(audioAddr->rotator);
    //     llvm_hpvm_untrack_mem(sumBF);
    //     llvm_hpvm_untrack_mem(audioAddr->zoomer);
    //     llvm_hpvm_untrack_mem(audioAddr->decoder0);
    //     llvm_hpvm_untrack_mem(audioAddr->decoder1);
    //     llvm_hpvm_untrack_mem(resultSample0);
    //     llvm_hpvm_untrack_mem(resultSample1);
    //     llvm_hpvm_untrack_mem(channelpart1);
    //     llvm_hpvm_untrack_mem(channelpart2);
    //     llvm_hpvm_untrack_mem(channelpart3);
    //     // printf("Done with Untracking\n");
    // }

    void* DFG = __hpvm__launch(1, audioDecoding, (void*) arg);

    if (numBlocks >= 2) {
        for (int i = 0; i < numBlocks; ++i) {
            arg->rotator = audioAddr->rotator;
            arg->sumBF = sumBF;
            arg->zoomer = audioAddr->zoomer;
            arg->decoder0 = audioAddr->decoder0;
            arg->resultSample0 = resultSample0;
            arg->channelpart1 = channelpart1;
            arg->channelpart2 = channelpart2;
            arg->channelpart3 = channelpart3;
            arg->decoder1 = audioAddr->decoder1;
            arg->resultSample1 = resultSample1;

            llvm_hpvm_track_mem(audioAddr->rotator, bytes_rotator);
            // printf("sumBF\n");
            llvm_hpvm_track_mem(sumBF, bytes_sumBF);
            // printf("zoomer\n");
            llvm_hpvm_track_mem(audioAddr->zoomer, bytes_zoomer);
            // printf("decoder\n");
            llvm_hpvm_track_mem(audioAddr->decoder0, bytes_decoder0);
            llvm_hpvm_track_mem(audioAddr->decoder1, bytes_decoder1);
            // printf("resultSample\n");
            llvm_hpvm_track_mem(resultSample0, bytes_resultSample0);
            llvm_hpvm_track_mem(resultSample1, bytes_resultSample1);
            llvm_hpvm_track_mem(channelpart1, bytes_channelpart1);
            llvm_hpvm_track_mem(channelpart2, bytes_channelpart2);
            llvm_hpvm_track_mem(channelpart3, bytes_channelpart3);
            // printf("Done with Tracking\n");

            // printf("Launching DFG\n")
            __hpvm__push(DFG, arg);
            void* ret = __hpvm__pop(DFG);
            size_t outSize = ((RootOut*) ret)->result;
            // printf("\n\nPipeline execution completed!\n");
            // printf("\n\nRequesting memory!\n");

            llvm_hpvm_request_mem(audioAddr->rotator, bytes_rotator);
            llvm_hpvm_request_mem(sumBF, bytes_sumBF);
            llvm_hpvm_request_mem(audioAddr->zoomer, bytes_zoomer);
            llvm_hpvm_request_mem(audioAddr->decoder0, bytes_decoder0);
            llvm_hpvm_request_mem(audioAddr->decoder1, bytes_decoder1);
            llvm_hpvm_request_mem(resultSample0, bytes_resultSample0);
            llvm_hpvm_request_mem(resultSample1, bytes_resultSample1);
            llvm_hpvm_request_mem(channelpart1, bytes_channelpart1);
            llvm_hpvm_request_mem(channelpart2, bytes_channelpart2);
            llvm_hpvm_request_mem(channelpart3, bytes_channelpart3);
            // printf("\nDone requesting Memory\n");

            // printf("Untracking\n");
            llvm_hpvm_untrack_mem(audioAddr->rotator);
            llvm_hpvm_untrack_mem(sumBF);
            llvm_hpvm_untrack_mem(audioAddr->zoomer);
            llvm_hpvm_untrack_mem(audioAddr->decoder0);
            llvm_hpvm_untrack_mem(audioAddr->decoder1);
            llvm_hpvm_untrack_mem(resultSample0);
            llvm_hpvm_untrack_mem(resultSample1);
            llvm_hpvm_untrack_mem(channelpart1);
            llvm_hpvm_untrack_mem(channelpart2);
            llvm_hpvm_untrack_mem(channelpart3);
            // printf("Done with Untracking\n");
        }
    }
    else {
        __hpvm__push(DFG, arg);
        __hpvm__pop(DFG);
    }

    __hpvm__wait(DFG);

    __hpvm__cleanup();

    // delete[] resultSample[0];
    // delete[] resultSample[1];
    // delete[] resultSample;

    return 0;
}
