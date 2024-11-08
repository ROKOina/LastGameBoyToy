#pragma once

#include "Component/System/Component.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Collsion\ColliderCom.h"

class SpawnCom :public Component
{
public:

    SpawnCom(const char* filename);
    ~SpawnCom() {};

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Spawn"; }

    //複製機能
    void SpawnGameObject();

private:

    //シリアライズ
    void Serialize();

    // デシリアライズ
    void Deserialize(const char* filename);

    // デシリアライズの読み込み
    void LoadDeserialize();

    //ランダム位置
    DirectX::XMFLOAT3 SpawnCom::GenerateRandomPosition();

    //個別で名前を付ける
    void AssignUniqueName(const std::shared_ptr<GameObject>& obj);

    //エネミー生成関数
    void SetupEnemy(const std::shared_ptr<GameObject>& obj);

    //ミサイル生成関数
    void SetupMissile(const std::shared_ptr<GameObject>& obj);

    //爆発生成関数
    void SetupExplosion(const std::shared_ptr<GameObject>& obj);

    //ビーム生成関数
    void SetupBeam(const std::shared_ptr<GameObject>& obj);
    void CreateBeamSegment(const std::shared_ptr<GameObject>& origin, const char* easingMovePath);

    //当たり判定
    void HitObject();

public:

    //更新するか否か
    bool OnTrigger() const { return spwntrigger; }
    void SetOnTrigger(bool flag) { spwntrigger = flag; }

    //生成された瞬間のフラグ
    bool GetSpawnFlag()const { return spawnflag; }
    void SetSpawnFlag(bool flag) { spawnflag = flag; }

public:

    //オブジェクトのenum
    enum class ObjectType
    {
        ENEMY,
        MISSILE,
        EXPLOSION,
        BEEM,
        MAX
    };
    ObjectType objtype = ObjectType::ENEMY;

    //生成時のパラメータ
    struct SpawnParameter
    {
        float spawnInterval = 0.0f;//生成間隔
        float spawnRadius = 0.0f;  //生成半径
        int spawnCount = 0;        //生成個数
        int objecttype = 0;        //オブジェクトのタイプ
        float Yoffset = 0.0f;      //Y軸のオフセット
        DirectX::XMFLOAT2 distanceXY = { 0.0f ,0.0f }; //距離

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SpawnParameter sp;

private:

    // 最後に生成した時間
    float lastSpawnTime = 0.0f;
    // 現在生成されたオブジェクトのカウント
    int currentSpawnedCount = 0; // 生成されたオブジェクトの数を追跡
    //生成フラグ
    bool spwntrigger = false;
    bool spawnflag = false;
    std::string filepath = {};

    std::weak_ptr<GameObject>sharedobj;
};