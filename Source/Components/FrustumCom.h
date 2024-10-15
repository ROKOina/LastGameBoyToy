#pragma once
#include "CameraCom.h"
#include "Graphics/Graphics.h"
#include <DirectXMath.h>
#include "Components/System/Component.h"
#include"Components/CameraCom.h"

class FrustumCom :public Component
{
public:
    //コンストラクタ
    FrustumCom();

    //初期化
    void Start() override;

    //更新処理
    void Update(float elapsedTime)override;

    // GUI描画
    void OnGUI() override;

private:
    //視錐台生成
    void CalcurateFrustum();

    //描画判定
    void DrawJudgement();

    //視錐台とAABBの当たり判定計算
    bool IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3 radius);



    struct  Plane
    {
        DirectX::XMFLOAT3 normal;
        float dist;
    };

public:
    Plane plane[6];
    DirectX::XMFLOAT3 nearP[4] = {};
    DirectX::XMFLOAT3 farP[4] = {};
    DirectX::XMFLOAT3 outLineNorm[4] = {};
    DirectX::XMFLOAT3 cameraPos = { 0,0,0 };
private:
    std::weak_ptr<CameraCom> cameraCom;
};

