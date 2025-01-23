#pragma once


#include <xaudio2.h>
#include <xaudio2fx.h>
#include <cassert>
#include <map>
#include <string>

#include "Audio/AudioSource.h"
#include "Audio/AudioResource.h"
#include <X3DAudio.h>
#include <wrl/client.h>
#include <xapofx.h>

#define INPUTCHANNELS 1  // number of source channels
#define OUTPUTCHANNELS 8 // maximum number of destination channels supported in this sample
#define NUM_PRESETS 30

enum class AUDIOID
{
    BGM,
    SE,
    SCENE_GAME1,
    SCENE_GAME2,
    SCENE_TITLE,
    CURSOR,
    ENTER,
    BOSS_JUMPATTACK_START,
    BOSS_JUMPATTACK_END,
    BOSS_JUMPATTACK_GROUND,
    BOSS_SHOT,
    BOSS_POWERSHOT,
    BOSS_CHARGE,
    BOSS_BULLET,
    BOSS_PUNCH,
    BOSS_LARIAT,
    BOSS_WALK,

    //プレイヤー
    PLAYER_SHOOT,
    PLAYER_ATTACKULTSHOOT,
    PLAYER_ATTACKULTBOOM,
    PLAYER_CHARGE,
    PLAYER_DAMAGE,
    PLAYER_DASH,

    //チュートリアルセリフ
    TUTOLINES_01,
    TUTOLINES_02,
    TUTOLINES_03,
    TUTOLINES_04,
    TUTOLINES_05,
    TUTOLINES_06,
    TUTOLINES_07,
    TUTOLINES_08,
    TUTOLINES_09,
    TUTOLINES_10,
    TUTOLINES_11,
    TUTOLINES_12,
    TUTOLINES_13,
    TUTOLINES_14,
    TUTOLINES_15,
    TUTOLINES_16,
    TUTOLINES_17,
    TUTOLINES_18,
    TUTOLINES_19,
    TUTOLINES_20,
    TUTOLINES_21,
    TUTOLINES_22,
    TUTOLINES_23,
    TUTOLINES_24,
    TUTOLINES_25,



    MAX
};

// リスナー情報
struct Listener_
{
    X3DAUDIO_LISTENER x3dListener = {};
    std::shared_ptr<X3DAUDIO_CONE> pCone;

    const X3DAUDIO_CONE Listener_DirectionalCone = { 
        X3DAUDIO_PI * 5.0f / 6.0f, X3DAUDIO_PI * 11.0f / 6.0f, 1.0f, 0.75f, 0.0f, 0.25f, 0.708f, 1.0f 
    };

    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 front = { 0.0f, 0.0f, 1.0f }; // 前方向
    DirectX::XMFLOAT3 top = { 0.0f, 1.0f, 0.0f };   // 上方向

    float listenerAngle;
    bool useListenerCone;
};
// エミッター情報
struct Emitter_
{
    X3DAUDIO_EMITTER x3dEmitter = {};
    X3DAUDIO_CONE emitterCone;

    const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_LFE_CurvePoints[3] = { 0.0f, 1.0f, 0.25f, 0.0f, 1.0f, 0.0f };
    const X3DAUDIO_DISTANCE_CURVE       Emitter_LFE_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_LFE_CurvePoints[0], 3 };

    const X3DAUDIO_DISTANCE_CURVE_POINT Emitter_Reverb_CurvePoints[3] = { 0.0f, 0.5f, 0.75f, 1.0f, 1.0f, 0.0f };
    const X3DAUDIO_DISTANCE_CURVE       Emitter_Reverb_Curve = { (X3DAUDIO_DISTANCE_CURVE_POINT*)&Emitter_Reverb_CurvePoints[0], 3 };

    DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 front = { 0.0f, 0.0f, 1.0f }; // 前方向
    DirectX::XMFLOAT3 top = { 0.0f, 1.0f, 0.0f };   // 上方向
};

// オーディオ
class Audio
{
public:
    Audio();
    ~Audio();

public:
    static void Initialize()
    {
        assert(instance == nullptr);
        instance = std::make_unique<Audio>();
    }
    static void Finalize()
    {
        assert(instance != nullptr);
        instance.reset();
    }
    // インスタンス取得
    static Audio& Instance() {
        assert(instance != nullptr);
        return *instance.get();
    }

    // オーディオソース読み込み
    std::shared_ptr<AudioResource> LoadAudioSource(const char* filename);
    std::shared_ptr<AudioResource> GetAudioResource(AUDIOID id);
    std::shared_ptr<AudioResource> GetAudioResourceID(AUDIOID id);
    // オーディオ登録
    void RegisterAudioSources();
    void RegisterAudioSourcesTest();

    // IDと名前の登録関数
    void RegisterAudioName(AUDIOID id, const std::string& name);
    std::string GetAudioName(AUDIOID id) const;

    IXAudio2* GetXAudio() const { return xaudio_; }
    const X3DAUDIO_HANDLE* GetX3DAudioHandle() const { return &x3dAudioHandle_; }
    IXAudio2MasteringVoice* GetMasteringVoice() const { return masteringVoice_; }

    Listener_& GetListener() { return listener_; }
    Emitter_& GetEmitter()   { return emitter_; }

    std::shared_ptr<X3DAUDIO_DSP_SETTINGS> GetDspSettings() const { return dspSettings; }
    void SetDspSettings(const std::shared_ptr<X3DAUDIO_DSP_SETTINGS>& settings) { dspSettings = settings; }
    
   IXAudio2SubmixVoice* GetSubmixVoice() const { return submixVoice; }
    void SetSubmixVoice(IXAudio2SubmixVoice* submix) { submixVoice = submix; }

    DWORD GetChannelMask() const { return channelMask; }
    void SetChannelMask(DWORD mask) { channelMask = mask; }

    UINT32 GetNChannels() const { return nChannels; }
    void SetNChannels(UINT32 channels) { nChannels = channels; }

    bool GetUseInnerRadius() const { return useInnerRadius; }
    void SetUseInnerRadius(bool useRadius) { useInnerRadius = useRadius; }

    bool GetUseRedirectToLFE() const { return useRedirectToLFE; }
    void SetUseRedirectToLFE(bool redirectToLFE) { useRedirectToLFE = redirectToLFE; }

    const FLOAT32* GetMatrixCoefficients() const { return matrixCoefficients; }
private:
    // 3Dオーディオ関連
    X3DAUDIO_HANDLE x3dAudioHandle_;
    std::shared_ptr<X3DAUDIO_DSP_SETTINGS> dspSettings;
    IXAudio2SubmixVoice* submixVoice;
    Microsoft::WRL::ComPtr<IUnknown> reverbEffect;

    // リスナー情報
    Listener_ listener_;
    // エミッター情報
    Emitter_ emitter_;

    DWORD channelMask;
    UINT32 nChannels;
    bool useInnerRadius;
    bool useRedirectToLFE;

    FLOAT32 matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];

private:
    static inline std::unique_ptr<Audio> instance;

    IXAudio2* xaudio_ = nullptr;
    IXAudio2MasteringVoice* masteringVoice_ = nullptr;

    std::map<AUDIOID, std::shared_ptr<AudioResource>> audioResources;
    std::map<AUDIOID, std::shared_ptr<AudioResource>> audioResourcesTest;
    std::map<AUDIOID, std::string> audioNames;  // IDと名前の紐付け
};