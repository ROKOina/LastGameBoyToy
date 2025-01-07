#pragma once

#include <xaudio2.h>
#include <cassert>
#include <map>

#include "Audio/AudioSource.h"
#include "Audio/AudioResource.h"
#include <X3DAudio.h>

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

    // オーディオ登録
    void RegisterAudioSources();
    void RegisterAudioSources(AUDIOID id, const char* filename);

    IXAudio2* GetXAudio() const { return xaudio_; }
    const X3DAUDIO_HANDLE* GetX3DAudioHandle() const { return &x3dAudioHandle_; }

private:
    static inline std::unique_ptr<Audio> instance;

    IXAudio2* xaudio_ = nullptr;
    IXAudio2MasteringVoice* masteringVoice_ = nullptr;

    // 3Dオーディオ関連
    X3DAUDIO_HANDLE x3dAudioHandle_;

    std::map<AUDIOID, std::shared_ptr<AudioResource>> audioResources;
};