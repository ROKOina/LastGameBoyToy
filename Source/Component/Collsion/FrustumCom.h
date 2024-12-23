#pragma once
#include "Component/Camera/CameraCom.h"
#include "Graphics/Graphics.h"
#include <DirectXMath.h>
#include "Component/System/Component.h"

#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();

class FrustumCom :public Component
{
public:

    //ú»
    void Start() override;

    //XV
    void Update(float elapsedTime)override;

    // GUI`æ
    void OnGUI() override;

    //¼OÝè
    const char* GetName() const override { return "Frustum"; }
private:
    //ä¶¬
    void CalcurateFrustum();

    //`æ»è
    void DrawJudgement();

    //äÆAABBÌ½è»èvZ
    bool IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3 radius);

    bool check = false;

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
