#pragma once
#include "Component/Camera/CameraCom.h"
#include "Graphics/Graphics.h"
#include <DirectXMath.h>
#include "Component/System/Component.h"

class FrustumCom :public Component
{
public:

    //ú»
    void Start() override {};

    //XV
    void Update(float elapsedTime)override;

    // GUI`æ
    void OnGUI() override;

    //¼OÝè
    const char* GetName() const override { return "Frustum"; }

private:

    //ä¶¬
    void CalculateFrustum();

    //`æ»è
    void PerformDrawJudgement();

    //äÆAABBÌ½è»èvZ
    bool IntersectFrustumVsAABB(const DirectX::XMFLOAT3& aabbPosition, const DirectX::XMFLOAT3& radius);

    // ½ÊvZâÖ
    void CalculatePlane(float a, float b, float c, float d, int index);

private:

    struct  Plane
    {
        DirectX::XMFLOAT3 normal;
        float dist;
    };

    Plane plane[6];
};