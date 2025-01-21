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

    void Test3DUpdate();

    // IDと名前の登録関数
    void RegisterAudioName(AUDIOID id, const std::string& name);
    std::string GetAudioName(AUDIOID id) const;

    IXAudio2* GetXAudio() const { return xaudio_; }
    const X3DAUDIO_HANDLE* GetX3DAudioHandle() const { return &x3dAudioHandle_; }
    IXAudio2MasteringVoice* GetMasterVoice() { return masteringVoice_; }
    IXAudio2SubmixVoice* GetSubmixVoice() { return submixVoice_; }
    unsigned long GetChannelMask() { return speakerChannelMask; }
    float GetInputChannel() { return InputChannels; }

private:
    static inline std::unique_ptr<Audio> instance;

    IXAudio2* xaudio_ = nullptr;
    IXAudio2MasteringVoice* masteringVoice_ = nullptr;

    Microsoft::WRL::ComPtr<IUnknown> pVolumeLimiter;
    Microsoft::WRL::ComPtr<IUnknown> pReverbEffect;
    IXAudio2SubmixVoice* submixVoice_ = nullptr;

    // 3Dオーディオ関連
    X3DAUDIO_HANDLE x3dAudioHandle_;

    std::map<AUDIOID, std::shared_ptr<AudioResource>> audioResources;

    std::map<AUDIOID, std::shared_ptr<AudioResource>> audioResourcesTest;
    std::map<AUDIOID, std::string> audioNames;  // IDと名前の紐付け

    unsigned long speakerChannelMask;
    float InputChannels;

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

    FLOAT32 matrixCoefficients[8];
};