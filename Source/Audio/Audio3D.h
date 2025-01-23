//--------------------------------------------------------------------------------------
// File: audio.h
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#include <xapofx.h>

#include <wrl/client.h>


#include "AudioResource.h"
#include <memory>

// オーディオソース
class AudioSource3D
{
public:
    AudioSource3D() = default;
    ~AudioSource3D() { }

    // オーディオ呼び出し関数
    void SetAudio();

    IXAudio2SourceVoice* sourceVoice_ = nullptr;
    std::shared_ptr<AudioResource>	resource_;
};


//-----------------------------------------------------------------------------
// Global defines
//-----------------------------------------------------------------------------
#define INPUTCHANNELS 1  // number of source channels
#define OUTPUTCHANNELS 8 // maximum number of destination channels supported in this sample

#define NUM_PRESETS 30

// Constants to define our world space
constexpr INT XMIN = -10;
constexpr INT XMAX = 10;
constexpr INT ZMIN = -10;
constexpr INT ZMAX = 10;

//-----------------------------------------------------------------------------
// Struct to hold audio game state
//-----------------------------------------------------------------------------
struct AUDIO_STATE
{
    bool bInitialized;

    // XAudio2
#ifdef USING_XAUDIO2_7_DIRECTX
    HMODULE mXAudioDLL;
#endif
    Microsoft::WRL::ComPtr<IXAudio2> pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;
    IXAudio2SubmixVoice* pSubmixVoice;
    Microsoft::WRL::ComPtr<IUnknown> pVolumeLimiter;
    Microsoft::WRL::ComPtr<IUnknown> pReverbEffect;

    // 3D
    X3DAUDIO_HANDLE x3DInstance;
    int nFrameToApply3DAudio;

    DWORD dwChannelMask;
    UINT32 nChannels;

    X3DAUDIO_DSP_SETTINGS dspSettings;
    X3DAUDIO_LISTENER listener;
    X3DAUDIO_EMITTER emitter;
    X3DAUDIO_CONE emitterCone;

    DirectX::XMFLOAT3 vListenerPos;
    DirectX::XMFLOAT3 vEmitterPos;
    float fListenerAngle;
    bool  fUseListenerCone;
    bool  fUseInnerRadius;
    bool  fUseRedirectToLFE;

    FLOAT32 matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];

};


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
extern AUDIO_STATE  g_audioState;

static AudioSource3D au3D;

//--------------------------------------------------------------------------------------
// External functions
//--------------------------------------------------------------------------------------
HRESULT InitAudio();
HRESULT PrepareAudio();
HRESULT UpdateAudio(float fElapsedTime);
HRESULT SetReverb(int nReverb);
VOID PauseAudio(bool resume);
VOID CleanupAudio();


