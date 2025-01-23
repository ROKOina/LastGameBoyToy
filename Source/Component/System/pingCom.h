#pragma once

#include "Component/System/Component.h"
#include "Component/System/TransformCom.h"
#include "Component/Particle/GPUParticle.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/UI/UiSystem.h"

class PingCom : public Component
{
public:
    PingCom() {}
    ~PingCom() override {}

    // 名前取得
    const char* GetName() const override { return "Ping"; }

    // 開始処理
    void Start() override {}

    // 更新処理
    void Update(float elapsedTime) override
    {
        //停止
        time -= elapsedTime;
        if (time < 0)
        {
            GetGameObject()->SetEnabled(false);
            return;
        }

        //判定済みなら戻す
        if (rayJudge)return;

        //キャラを選択しているか判別
        std::weak_ptr<CharacterCom> chara;
        float dist = 1000;
        for (auto& obj : GetGameObject()->GetComponent<RayColliderCom>()->OnHitGameObject())
        {
            auto& pos = obj.gameObject.lock()->transform_->GetWorldPosition();
            float d = Mathf::Length(start - pos);
            if (d > dist)continue;  //一番近い敵を保存
            dist = d;
            chara = obj.gameObject.lock()->GetParent()->GetComponent<CharacterCom>();
        }

        //キャラが選択されている場合
        if (chara.lock())
        {
            //キャラの選択関数を呼ぶ
            std::string Nname = "netPlayer" + std::to_string(chara.lock()->GetNetCharaData().GetNetPhotonID());
            GameObj netPlayer = GameObjectManager::Instance().Find(Nname.c_str());

            if (netPlayer)
            {
                //ターゲットUI設定
                auto& ui = GetGameObject()->GetComponent<UiSystem>();
                ui->spc.objectname = Nname;
                ui->SetEnabled(true);

                //ネットに送る
                StaticSendDataManager::PinSendData data;
                data.isPos = false;
                data.photonid = chara.lock()->GetNetCharaData().GetNetPhotonID();

                StaticSendDataManager::Instance().SendNetPing(data);
            }

            rayJudge = true;
            return;
        }

        //判定用フレームを確保
        if (!oneFrame)
        {
            oneFrame = true;
            return;
        }

        rayJudge = true;

        //最後にオブジェと判定
        PxRaycastBuffer buffer;
        if (PhysXLib::Instance().RayCast_PhysX(start, Mathf::Normalize(end - start), Mathf::Length(end - start), buffer, PhysXLib::CollisionLayer::Stage))
        {
            DirectX::XMFLOAT3 pinPos;

            // レイキャストが当たった位置と法線を保存
            pinPos.x = buffer.block.position.x;
            pinPos.y = buffer.block.position.y;
            pinPos.z = buffer.block.position.z;

            //ピンを置く
            GetGameObject()->transform_->SetWorldPosition(pinPos);
            auto& per = GetGameObject()->GetComponent<GPUParticle>();
            per->SetEnabled(true);
            per->Play();

            //ネットに送る
            StaticSendDataManager::PinSendData data;
            data.isPos = true;
            data.pinPos = pinPos;

            StaticSendDataManager::Instance().SendNetPing(data);

            return;
        }

        //最後まで来たらピンを置けない箇所
        GetGameObject()->SetEnabled(false);
    }

    // GUI描画
    void OnGUI() override {}

    void SetPing()
    {
        auto& player = GameObjectManager::Instance().Find("player");
        if (!player)return;
        auto& camera = player->GetChildFind("cameraPostPlayer");
        if (!camera)return;

        //レイキャスト位置決定
        start = camera->transform_->GetWorldPosition();
        end = start + camera->transform_->GetWorldFront() * 1000;

        GetGameObject()->GetComponent<RayColliderCom>()->SetStart(start);
        GetGameObject()->GetComponent<RayColliderCom>()->SetEnd(end);

        rayJudge = false;
        oneFrame = false;

        GetGameObject()->SetEnabled(true);
        GetGameObject()->GetComponent<GPUParticle>()->SetEnabled(false);
        GetGameObject()->GetComponent<UiSystem>()->SetEnabled(false);
        time = 3;
    }

    void SetTargetPing(int photonID)
    {
        GetGameObject()->SetEnabled(true);
        GetGameObject()->GetComponent<GPUParticle>()->SetEnabled(false);

        //キャラの選択関数を呼ぶ
        std::string Nname = "netPlayer" + std::to_string(photonID);
        GameObj netPlayer = GameObjectManager::Instance().Find(Nname.c_str());

        if (netPlayer)
        {
            //ターゲットUI設定
            auto& ui = GetGameObject()->GetComponent<UiSystem>();
            ui->spc.objectname = Nname;
            ui->SetEnabled(true);
        }
        time = 3;
    }

    void SetPosPing(DirectX::XMFLOAT3 pos)
    {
        GetGameObject()->SetEnabled(true);
        GetGameObject()->GetComponent<UiSystem>()->SetEnabled(false);

        //ピンを置く
        GetGameObject()->transform_->SetWorldPosition(pos);
        auto& per = GetGameObject()->GetComponent<GPUParticle>();
        per->SetEnabled(true);
        per->Play();
        time = 3;
    }

private:
    float time = 0;
    bool rayJudge = false;
    bool oneFrame = false;
    DirectX::XMFLOAT3 start;
    DirectX::XMFLOAT3 end;
};