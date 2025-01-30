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
#include "SystemStruct/Misc.h"

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

enum class AUDIOID2D
{
    SCENE_TITLE,
    SCENE_LOBBY,
    SCENE_BOSS_START,
    SCENE_BOSS_CLYMAX,
    SCENE_RESULT_VICTORY,
    SCENE_RESULT_DEFEAT,
    SCENE_RESULT_VICTORY_SE,
    SCENE_RESULT_DEFEAT_SE,

    CURSOR,
    ENTER,

    //プレイヤー
    PLAYER_DAMAGE,
    PLAYER_CANON,
    PLAYER_CANON2,

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
    TUTOLINES_26,
    TUTOLINES_27,
    TUTOLINES_28,
    TUTOLINES_29,
    TUTOLINES_30,
    TUTOLINES_31,
    TUTOLINES_32,

    MAX_
};

class Audio2DMagaer
{
public:
    // インスタンス取得
    static Audio2DMagaer& Instance()
    {
        static Audio2DMagaer instance;
        return instance;
    }

    Audio2DMagaer() {
        HRESULT hr;
        // COMの初期化
        hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        //createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif
    // XAudio初期化
        hr = XAudio2Create(&xaudio2D, createFlags);
        // マスタリングボイス生成
        hr = xaudio2D->CreateMasteringVoice(&masteringVoice);

        Register2DAudio(xaudio2D.Get());
    };
    ~Audio2DMagaer() {
        //ソースボイス削除
        for (auto& r2d : audio2DResources)
        {
            if (r2d.second.sourceVoice_)
            {
                r2d.second.sourceVoice_->DestroyVoice();
                r2d.second.sourceVoice_ = nullptr;
            }
        }

        // マスタリングボイス破棄
        if (masteringVoice != nullptr)
        {
            masteringVoice->DestroyVoice();
            masteringVoice = nullptr;
        }

        xaudio2D->StopEngine();
        xaudio2D.Reset();

        // COM終了化
        CoUninitialize();
    };

    void Audio2DPlay(AUDIOID2D id, float volume = 1, bool loop = false);
    void Audio2DStop(AUDIOID2D id);
    void Audio2DStopAll();
    void Audio2DFeed(AUDIOID2D id, float start, float end) {}

private:
    void Register2DAudio(IXAudio2* x2d)
    {
        //2Dオーディオ登録
        audio2DResources[AUDIOID2D::SCENE_TITLE] = AudioResource2DStr(x2d, "Data/AudioData/BGM/TitleBGM.wav");
        audio2DResources[AUDIOID2D::SCENE_LOBBY] = AudioResource2DStr(x2d, "Data/AudioData/BGM/LobbyBGM.wav");
        audio2DResources[AUDIOID2D::SCENE_BOSS_START] = AudioResource2DStr(x2d, "Data/AudioData/BGM/BossBattle_start.wav");
        audio2DResources[AUDIOID2D::SCENE_BOSS_CLYMAX] = AudioResource2DStr(x2d, "Data/AudioData/BGM/BossBattle_clymax.wav");
        audio2DResources[AUDIOID2D::SCENE_RESULT_VICTORY] = AudioResource2DStr(x2d, "Data/AudioData/BGM/Result_victoryBGM.wav");
        audio2DResources[AUDIOID2D::SCENE_RESULT_DEFEAT] = AudioResource2DStr(x2d, "Data/AudioData/BGM/Result_defeatBGM.wav");
        audio2DResources[AUDIOID2D::SCENE_RESULT_VICTORY_SE] = AudioResource2DStr(x2d, "Data/AudioData/SE/result/victorySE.wav");
        audio2DResources[AUDIOID2D::SCENE_RESULT_DEFEAT_SE] = AudioResource2DStr(x2d, "Data/AudioData/SE/result/defeatSE.wav");
        audio2DResources[AUDIOID2D::CURSOR] = AudioResource2DStr(x2d, "Data/AudioData/SE/cursorMove.wav");
        audio2DResources[AUDIOID2D::ENTER] = AudioResource2DStr(x2d, "Data/AudioData/SE/enter.wav");

        //プレイヤー
        audio2DResources[AUDIOID2D::PLAYER_DAMAGE] = AudioResource2DStr(x2d, "Data/AudioData/SE/player/player_damage.wav");

        //チュートリアル
        audio2DResources[AUDIOID2D::TUTOLINES_01] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/001_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_02] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/002_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_03] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/003_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_04] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/004_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_05] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/005_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_06] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/006_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_07] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/007_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_08] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/008_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_09] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/009_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_10] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/010_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_11] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/011_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_12] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/012_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_13] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/013_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_14] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/014_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_15] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/015_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_16] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/016_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_17] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/017_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_18] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/018_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_19] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/019_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_20] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/020_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_21] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/021_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_22] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/022_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_23] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/023_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_24] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/024_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_25] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/025_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_26] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/026_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_27] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/027_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_28] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/028_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_29] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/029_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_30] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/030_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_31] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/031_L.wav");
        audio2DResources[AUDIOID2D::TUTOLINES_32] = AudioResource2DStr(x2d, "Data/AudioData/SE/Tutorial/TutorialLines/032_L.wav");
    }

        //2D音源構造体
    struct AudioResource2DStr
    {
        friend Audio2DMagaer;

        // デフォルトコンストラクタを明示的に定義
        AudioResource2DStr() : sourceVoice_(nullptr) {}
        //初期化
        AudioResource2DStr(IXAudio2* x2d, std::string filename)
        {
            resource2D = std::make_shared<AudioResource>(filename.c_str());

            if (sourceVoice_)
            {
                sourceVoice_->Stop(0);
                sourceVoice_->DestroyVoice();
                sourceVoice_ = 0;
            }

            HRESULT hr = x2d->CreateSourceVoice(&sourceVoice_, &resource2D->GetWaveFormat());
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }

        std::shared_ptr<AudioResource> resource2D;
        IXAudio2SourceVoice* sourceVoice_ = nullptr;
    };

    //変数
    std::map<AUDIOID2D, AudioResource2DStr> audio2DResources;

    Microsoft::WRL::ComPtr<IXAudio2> xaudio2D;
    IXAudio2MasteringVoice* masteringVoice = nullptr;
};

enum AUDIOID3D
{
    PLAYER_WAKL,

    //ハンゾー
    HANZO_ATK,
    HANZO_DASH,
    HANZO_CHARGE,
    HANZO_ULT,
    HANZO_ULT_BOOM,

    //ファラ
    FARAH_JET,
    FARAH_ATK,
    FARAH_SKILL1,
    FARAH_SKILL2,
    FARAH_ULT,

    //ジャンクラ
    JANKRA_ATK1,
    JANKRA_ATK2,
    JANKRA_ULT,
    JANKRA_ULT2,
    JANKRA_MINE,
    JANKRA_MINE_FIRE,

    //ソルジャー
    SOLDIER_ATK,
    SOLDIER_SKILL1,
    SOLDIER_SKILL2,
    SOLDIER_ULT,

    //ボス
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

    MAX_
};
class Audio3DResourceMagaer
{
public:
    Audio3DResourceMagaer() {
        Register3DAudio();
    };
    ~Audio3DResourceMagaer() {};

    // インスタンス取得
    static Audio3DResourceMagaer& Instance()
    {
        static Audio3DResourceMagaer instance;
        return instance;
    }

    //リソース検索
    std::shared_ptr<AudioResource> GetAudio3DResouce(AUDIOID3D audioENUM)
    {
        auto& it = audio3DResources.find(audioENUM);
        if (it != audio3DResources.end()) { // 見つかった
            return it->second;
        }
        //見つからない
        return nullptr;
    }
private:
    void Register3DAudio()
    {
        //3Dオーディオ登録
        audio3DResources[AUDIOID3D::PLAYER_WAKL] = std::make_shared<AudioResource>("Data/AudioData/SE/player/player_walk.wav");

        audio3DResources[AUDIOID3D::HANZO_ATK] = std::make_shared<AudioResource>("Data/AudioData/SE/player/mono/kanizo/player_shoot.wav");
        audio3DResources[AUDIOID3D::HANZO_CHARGE] = std::make_shared<AudioResource>("Data/AudioData/SE/player/mono/kanizo/player_charge.wav");
        audio3DResources[AUDIOID3D::HANZO_DASH] = std::make_shared<AudioResource>("Data/AudioData/SE/player/mono/kanizo/player_dash2.wav");
        audio3DResources[AUDIOID3D::HANZO_ULT] = std::make_shared<AudioResource>("Data/AudioData/SE/player/mono/kanizo/player_attackUltShoot.wav");
        audio3DResources[AUDIOID3D::HANZO_ULT_BOOM] = std::make_shared<AudioResource>("Data/AudioData/SE/player/mono/kanizo/player_Boom.wav");

        audio3DResources[AUDIOID3D::FARAH_JET] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/jeat.wav");
        audio3DResources[AUDIOID3D::FARAH_ATK] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/Fara_MainAttack.wav");
        audio3DResources[AUDIOID3D::FARAH_SKILL1] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/FaraSkill_E.wav");
        audio3DResources[AUDIOID3D::FARAH_SKILL2] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/Fara_RightClick.wav");

        audio3DResources[AUDIOID3D::JANKRA_ATK1] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/se_small_bomb01.wav");
        audio3DResources[AUDIOID3D::JANKRA_ATK2] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/se_small_bomb02.wav");
        audio3DResources[AUDIOID3D::JANKRA_ULT] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/canon.wav");
        audio3DResources[AUDIOID3D::JANKRA_ULT2] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/canon2.wav");
        audio3DResources[AUDIOID3D::JANKRA_MINE] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/mine.wav");
        audio3DResources[AUDIOID3D::JANKRA_MINE_FIRE] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/mineBomb.wav");

        audio3DResources[AUDIOID3D::SOLDIER_ATK] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/gun.wav");
        audio3DResources[AUDIOID3D::SOLDIER_SKILL1] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/stunBullet.wav");
        audio3DResources[AUDIOID3D::SOLDIER_SKILL2] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/nockback.wav");
        audio3DResources[AUDIOID3D::SOLDIER_ULT] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/stunUlt.wav");

        audio3DResources[AUDIOID3D::BOSS_JUMPATTACK_START] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_jumpAttack_start.wav");
        audio3DResources[AUDIOID3D::BOSS_JUMPATTACK_END] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_jumpAttack_end.wav");
        audio3DResources[AUDIOID3D::BOSS_JUMPATTACK_GROUND] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_jumpAttack_ground3.wav");
        audio3DResources[AUDIOID3D::BOSS_SHOT] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_shot.wav");
        audio3DResources[AUDIOID3D::BOSS_POWERSHOT] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_powerShot.wav");
        audio3DResources[AUDIOID3D::BOSS_CHARGE] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_charge.wav");
        audio3DResources[AUDIOID3D::BOSS_BULLET] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_fire.wav");
        audio3DResources[AUDIOID3D::BOSS_PUNCH] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_punch.wav");
        audio3DResources[AUDIOID3D::BOSS_LARIAT] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_lariat.wav");
        audio3DResources[AUDIOID3D::BOSS_WALK] = std::make_shared<AudioResource>("Data/AudioData/SE/mono/boss_jumpAttack_ground2.wav");

    }

    std::map<AUDIOID3D, std::shared_ptr<AudioResource>> audio3DResources;
};

// オーディオソース
class AudioSource3D : public Component
{
public:
    AudioSource3D(AUDIOID3D id);
    ~AudioSource3D() override;

    void Start() override;
    void Update(float elapsedTime) override;

    const char* GetName() const override { return "Audio3D"; }
    void OnGUI() override;

    void UpdateAudio3d(float elapsedTime);

    // 再生
    void AudioPlay(bool loop, float pitch = 1.0f);
    void AudioPlay();
    // 停止
    void Audio3DStop();

    void SetVolume(float volume) { volume = this->volume; }

    void SetListenerPos(DirectX::XMFLOAT3 pos) { listenerPos = pos; }
    void SetEmitterPos(DirectX::XMFLOAT3 pos) { emitterPos = pos; }

    //効果
    void SetReverb(int nReverb);

    //聞こえる範囲
    void SetCurveDistanceScaler(float dis) { CurveDistanceScaler = dis; }

private:
    // オーディオ登録関数
    void SetAudio(AUDIOID3D id);

public:
    IXAudio2SourceVoice* sourceVoice_ = nullptr;
    std::shared_ptr<AudioResource>	resource_;

private:
    AUDIOID3D myId = AUDIOID3D::MAX_;

    DirectX::XMFLOAT3 listenerPos;
    DirectX::XMFLOAT3 emitterPos;

    float volume = 1;

    float CurveDistanceScaler = 14.0f;

    bool frameUpdate = false;

    AUDIO_STATE  g_audioState;
};