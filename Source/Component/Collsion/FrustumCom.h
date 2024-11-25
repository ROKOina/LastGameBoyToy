#pragma once
#include "Component/Camera/CameraCom.h"
#include "Graphics/Graphics.h"
#include <DirectXMath.h>
#include "Component/System/Component.h"

#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();

class FrustumCom :public Component
{
public:

    //‰Šú‰»
    void Start() override {};

    //XVˆ—
    void Update(float elapsedTime)override;

    // GUI•`‰æ
    void OnGUI() override;

    //–¼‘Oİ’è
    const char* GetName() const override { return "Frustum"; }

private:

    //‹‘ä¶¬
    void CalculateFrustum();

    //•`‰æ”»’è
    void PerformDrawJudgement();

    //‹‘ä‚ÆAABB‚Ì“–‚½‚è”»’èŒvZ
    bool IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3& radius);

    // •½–ÊŒvZ•â•ŠÖ”
    void CalculatePlane(float a, float b, float c, float d, int index);

private:

    struct  Plane
    {
        DirectX::XMFLOAT3 normal;
        float dist;
    };

    Plane plane[6];
    DirectX::XMFLOAT3 nearP[4] = {};
    DirectX::XMFLOAT3 farP[4] = {};
    DirectX::XMFLOAT3 outLineNorm[4] = {};
    DirectX::XMFLOAT3 cameraPos = { 0,0,0 };
    std::weak_ptr<CameraCom> cameraCom;
    bool check = false;
};