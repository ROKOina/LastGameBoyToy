#pragma once
#include "Component\UI\UiSystem.h"
#include "Component\UI\UiGauge.h"
#include "Component\Character\RegisterChara.h"
#include "Component\Character\CharacterCom.h"
#include "PVPGameSystem\PVPGameSystem.h"
#include <map>

class UI_Skill : public UiSystem
{
    //コンポーネントオーバーライド
public:
    UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max);
    ~UI_Skill() {}

    // 名前取得
    const char* GetName() const override { return "Ui_Skill"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

private:
    float changePosValue;
    DirectX::XMFLOAT2 originalPos = {};
    DirectX::XMFLOAT2 maxPos = {};
};

class UI_HPEffect : public UiSystem
{
    //コンポーネントオーバーライド
public:
    UI_HPEffect(const char* filename, SpriteShader spriteshader, bool collsion, int gaugeTexSize, std::weak_ptr<GameObject> obj, int num);
    ~UI_HPEffect() {}

    // 名前取得
    const char* GetName() const override { return "Ui_Hpeffect"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    void OnGUI()override;

private:
    int divideTexSize = 0; //分割した時のTexSize
    int gaugeTexSize = 0;  //ゲージ本体のテクスチャサイズ
    float maxHp;
    int memoryId; //何番目のメモリなのか
    std::weak_ptr<GameObject> character;

    bool easingFLG = false;
    bool onceFLG = false;

    bool isDebug = false;
};

class UI_PlayerHpUI :public Component
{
    //コンポーネントオーバーライド
public:
    UI_PlayerHpUI();
    ~UI_PlayerHpUI() {}

    // 名前取得
    const char* GetName() const override { return "UI_PlayerHp"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

private:
    float* hp;
    std::weak_ptr<GameObject> player;
};

class UI_BoosGauge : public Component
{
    //コンポーネントオーバーライド
public:
    UI_BoosGauge();
    ~UI_BoosGauge() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

private:
    std::shared_ptr<GameObject> frame;  //外枠
    std::shared_ptr<GameObject> gauge;  //ゲージ
    std::shared_ptr<GameObject> mask;   //マスク

    int minAngle = 0;
    int maxAngle = 90;

    int  num;
    float maxDashGauge;                 //ダッシュゲージの総量を保持
    float* value;                       //ダッシュゲージの現在の値
    float separateValue;                //区切りの値
    DirectX::XMFLOAT2 originlTexSize;   //元のテクスチャーサイズ
    std::weak_ptr<GameObject> player;
};

class UI_LockOn : public Component
{
    //コンポーネントオーバーライド
public:
    UI_LockOn(int num, float min, float max);
    ~UI_LockOn() {}

    // 名前取得
    const char* GetName() const override { return "UI_LockOn"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    void OnGUI()override;

    //視線の先のオブジェクトを入手
    std::shared_ptr<GameObject> SearchObjct();

    void UpdateGauge(float elapsedTime, std::shared_ptr<GameObject> obj);

    void LockIn(float elapsedTime);
    void LockOut(float elapsedTime);

private:
    std::vector<std::shared_ptr<GameObject>> reacters;

    std::weak_ptr<GameObject> camera;   //カメラ

    std::shared_ptr<GameObject> lockOn;
    std::shared_ptr<GameObject> lockOn2;
    std::shared_ptr<GameObject> gaugeFrame;
    std::shared_ptr<GameObject> gaugeFrame2;
    std::shared_ptr<GameObject> gauge;
    std::shared_ptr<GameObject> gaugeMask;

    std::shared_ptr<UiSystem> lockOnUi;         //lockOn
    std::shared_ptr<UiSystem> lockOn2Ui;        //lockOn
    std::shared_ptr<UiSystem> gaugeFrameUi;     //GaugeFrame
    std::shared_ptr<UiSystem> gaugeUi;          //Gauge
    std::shared_ptr<UiSystem> gaugeMaskUi;      //GaugeMask

    float minAngle = 0.0f;
    float maxAngle = 0.0f;
    std::vector<float> similarity;
};

class UI_E_SkillCount : public Component
{
    //コンポーネントオーバーライド
public:
    UI_E_SkillCount(int num);
    ~UI_E_SkillCount() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;
    void UpdateGauge(float elapsedTime);
    void UpdateCore(float elapsedTime);

    void OnGUI()override;

private:
    struct SkillCore {
        std::shared_ptr<UiSystem> coreFrameUi;
        std::shared_ptr<UiSystem> coreUi;
    };
    std::vector<SkillCore> coresUi;
    std::shared_ptr<UiSystem> gaugeUi;
    std::shared_ptr<UiSystem> gaugeFrameUi;

    std::vector<std::shared_ptr<GameObject>> cores;
    std::vector<std::shared_ptr<GameObject>> coreFrames;
    std::shared_ptr<GameObject> gauge;
    std::shared_ptr<GameObject> gaugeFrame;

    int num = 0;
    int* arrowCount = 0;
    float* skillTimer = nullptr;
    float skillTime = 3.0f;
    float spacing = 30.0f;
    bool* isShot;
    DirectX::XMFLOAT2 originalTexSize = {};
    std::weak_ptr<GameObject> player;
};

class UI_Ult_Count : public Component
{
    //コンポーネントオーバーライド
public:
    UI_Ult_Count(int num);
    ~UI_Ult_Count() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;
    void UpdateCore(float elapsedTime);

private:
    struct SkillCore {
        std::shared_ptr<UiSystem> coreFrameUi;
        std::shared_ptr<UiSystem> coreUi;
    };
    std::vector<SkillCore> coresUi;

    std::vector<std::shared_ptr<GameObject>> cores;
    std::vector<std::shared_ptr<GameObject>> coreFrames;

    int num = 0;
    int* ultCount;
    bool* isUlt;
    float spacing = 30.0f;
    std::weak_ptr<GameObject> player;
};

class UI_EnemyHp : public Component
{
public:
    UI_EnemyHp() {};
    ~UI_EnemyHp() {};

    // 名前取得
    const char* GetName() const override { return "UI_EnemyHp"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //ゲージ更新
    void GaugeUpdate(float elapsedTime);

    //登録
    void Register(std::weak_ptr<GameObject> obj);

    void OnGUI()override {};
private:
    bool enemyFLG = false;
    bool displayFLG = false;

    float oldHp = 0.0f;
    float timer = 0.0f;
    const float time = 0.5f;

    std::shared_ptr<GameObject> enemyHp;

    float* hp;
};

class UI_GameJudge : public Component
{
    //コンポーネントオーバーライド
public:
    UI_GameJudge(PVPGameSystem::TEAM_KIND victryTeam);
    ~UI_GameJudge() {}

    // 名前取得
    const char* GetName() const override { return "UI_GameJudge"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    void OnGUI()override;
private:
    std::vector<std::weak_ptr<GameObject>> circles;
    int state = 0;
    float fadeTimr = 0.0f;
};

class UI_UltNum : public Component
{
public:
    UI_UltNum();
    ~UI_UltNum() {};

    // 名前取得
    const char* GetName() const override { return "UI_UltNum"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    void OnGUI()override {};

private:
};

class UI_SkillNum : public Component
{
public:
    UI_SkillNum(CharacterCom::SkillCoolID skillid, const char* objnamekunn, const char* name, DirectX::XMFLOAT2 pos);
    ~UI_SkillNum() {};

    // 名前取得
    const char* GetName() const override { return "UI_SkillNum"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //gui
    void OnGUI()override {};

private:
    CharacterCom::SkillCoolID skill = CharacterCom::SkillCoolID::E;
    const char* objname = {};
};

class UI_HpNum : public Component
{
public:
    UI_HpNum();
    ~UI_HpNum() {};

    // 名前取得
    const char* GetName() const override { return "UI_HpNum"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //gui
    void OnGUI()override {};
};

class UI_BulletNum : public Component
{
public:
    UI_BulletNum();
    ~UI_BulletNum() {};

    // 名前取得
    const char* GetName() const override { return "UI_BulletNum"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //gui
    void OnGUI()override {};
};

class UI_SkillComp :public Component
{
public:
    UI_SkillComp(CharacterCom::SkillCoolID skillid);
    ~UI_SkillComp() {};

    // 名前取得
    const char* GetName() const override { return "UI_SkillComp"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //gui
    void OnGUI()override {};

private:
    CharacterCom::SkillCoolID skill = CharacterCom::SkillCoolID::E;
};

class UI_DeathComp :public Component
{
public:
    UI_DeathComp() {};
    ~UI_DeathComp() {};

    // 名前取得
    const char* GetName() const override { return "UI_DeathComp"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //gui
    void OnGUI()override {};
};

class UI_KillEffect : public Component
{
public:
    UI_KillEffect();
    ~UI_KillEffect() {};

    // 名前取得
    const char* GetName() const override { return "KillEffect"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    //エフェクトの更新
    void EffectUpdat(float elapsedTime);

    //gui
    void OnGUI()override {};

private:
    bool  effectFLG;
    float  effectFLGTimer = 0;
};

//PlayerUIのマネージャー
class PlayerUIManager
{
    //コンポーネントオーバーライド

public:
    PlayerUIManager() {};
    ~PlayerUIManager() {};

    // インスタンス取得
    static PlayerUIManager& Instance()
    {
        static PlayerUIManager instance;
        return instance;
    }

    void Register();

    void UIUpdate(float elapsedTime);

    //スキルUI
    void CreateSkillUI(USE_SKILL use_skill, int count);

    //レティクルUI
    void CreateReticleUI();

    //ウルトUI
    void CreateUltUI();
    //HPUI
    void CreateHpUI();
    //ブーストUI
    void CreateBoostUI();

    //敵HP
    void CreateEnemyHpUI() {};

    //キャラアイコン
    void CreatePlayerIcon();

    //銃のアイコンとか
    void CreateGunIcon();

    //ヒットエフェクト
    void CreateHitEffect();

    //キルエフェクト
    void CreateKillEffect();

    //キルログ
    void KillLogUpdate(float elapsedTime);
    void CreateKillLog();   //オブジェ生成

    //全員の使用キャラUI
    void CreateNetUseCharaUI();
    //使用キャラ更新
    void NetUseCharaUIUpdate(int chara[4], int photonid[4]);

    //味方HPUI
    void CreateNetTeamUI(std::weak_ptr<GameObject> netPlayer);
    bool GetAllyHp() { return allyHp; }
    void ResetAllyHp() { allyHp = false; }

    //勝敗表示UI
    void CreateGameJudgeUI(PVPGameSystem::TEAM_KIND victryTeam);
    bool GetIsEndFLG() { return isEndFLG; }

    void BookingRegistrationUI(std::shared_ptr<GameObject> obj);

private:
    bool bookingRegister = false;

    std::weak_ptr<GameObject> player;

    bool isEndFLG = false;

    bool allyHp = false;    //味方HP表示済み

    float  kilogTimer[4] = {};

    //要素がキルキャラID
    struct DeathData
    {
        int charaID;
        bool isEnemy;   //ですしたキャラは敵か
        //自分がいる場合
        int myID = -1;  //-1:なし　0:キル　1:デス

        //動き
        struct MoveData
        {
            bool startFlg = false;
            int id = 0;

            float timer = 0;
            int underNum = 0;   //追加できた数
        }moveData;
    };
    std::map<int, DeathData> saveCharaKilog;
};