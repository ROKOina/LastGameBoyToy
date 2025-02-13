#pragma once
#include "Component/Camera/CameraCom.h"

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
};