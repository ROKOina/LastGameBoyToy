#pragma once

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#include <xapofx.h>

#include <wrl/client.h>

#include "AudioResource.h"
#include <memory>
#include <map>
#include "Component/System/Component.h"

    // Global defines
#define INPUTCHANNELS 1  // number of source channels
#define OUTPUTCHANNELS 8 // maximum number of destination channels supported in this sample

#define NUM_PRESETS 30

// Constants to define our world space
    constexpr INT XMIN = -10;
    constexpr INT XMAX = 10;
    constexpr INT ZMIN = -10;
    constexpr INT ZMAX = 10;

    static const X3DAUDIO_CONE Listener_DirectionalCone = { X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f };

    static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
    static const X3DAUDIO_DISTANCE_CURVE       Emitter_LFE_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_LFE_CurvePoints[0], 3 };

    static const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
    static const X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0], 3 };

enum AUDIOIDTEST
{
    BGM,
    SE,
    TEST,

   MAX_
};
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

// オーディオソース
class AudioSource3D : public Component
{
public:
    AudioSource3D();
    ~AudioSource3D() override;

    void Start() override;
    void Update(float elapsedTime) override;

    const char* GetName() const override { return "Audio"; }
    void OnGUI() override {}

    void UpdateAudio3d(float elapsedTime);

    // オーディオ呼び出し関数
    void SetAudio(AUDIOIDTEST id);

    // 再生
    void AudioPlay();

    void SetListenerPos(DirectX::XMFLOAT3 pos) { listenerPos = pos; }
    void SetEmitterPos(DirectX::XMFLOAT3 pos) { emitterPos = pos; }

private:
    // オーディオ登録
    void RegisterAudio();

public:
    IXAudio2SourceVoice* sourceVoice_ = nullptr;
    std::shared_ptr<AudioResource>	resource_;

private:
    std::map<AUDIOIDTEST, std::shared_ptr<AudioResource>> audioResources;

    DirectX::XMFLOAT3 listenerPos;
    DirectX::XMFLOAT3 emitterPos;
    AUDIO_STATE  g_audioState;
};


//// Global variables
//extern AUDIO_STATE  g_audioState;