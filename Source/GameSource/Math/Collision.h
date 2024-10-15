#pragma once

#include <DirectXMath.h>
#include "Graphics/Model/Model.h"

//ヒット結果
struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 }; //レイとポリゴンの交点
    DirectX::XMFLOAT3 normal = { 0,0,0 };   //衝突したポリゴンの法線ベクトル
    float distance = 0.0f;  //レイの始点から交点までの距離
    int materialIndex = -1; //衝突したポリゴンのマテリアル番号
    DirectX::XMFLOAT3 rotation = { 0,0,0 }; //回転量
};

//コリジョン
class Collision
{
public:
    //球と球の交差判定
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    //円柱と円柱の交差判定
    static bool IntersectCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    //球と円柱の交差判定
    static bool IntersectSphereVsCylider(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    //球と円柱の交差判定(回転あり)
    static bool Collision::IntersectSphereVsCylider(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        const DirectX::XMFLOAT3& cylinderDir,
        float cylinderRadius,
        float cylinderHeight
    );

    //レイとモデルの交差判定
    static bool IntersectRayVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result
    );

    static bool IntersectRayVsSphere(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR& spherePos,
        float radius,
        HitResult& result
    );

    // レイVs円柱
    static bool IntersectRayVsOrientedCylinder(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMVECTOR& startCylinder,
        const DirectX::XMVECTOR& endCylinder,
        float radius,
        HitResult& result,
        DirectX::XMVECTOR* onCenterLinPos = {});

    // レイVsBOX（OBB）
    static bool IntersectRayVsBOX(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// 要正規化
        float rayDist,
        const DirectX::XMMATRIX boxWorldTransform,
        HitResult& result);

    //　球VsOBB
    static bool IntersectSphereVsOBB(
        const DirectX::XMVECTOR spherePos,
        float radius,
        DirectX::XMMATRIX boxWorldTransform);

    //カプセルVs円柱
    static bool IntersectCapsuleVsCylinder(
        const DirectX::XMVECTOR cap1StartPos,
        const DirectX::XMVECTOR cap1EndPos,
        float cap1Radius,
        const DirectX::XMVECTOR cly2StartPos,
        const DirectX::XMVECTOR cly2EndPos,
        float cly2Radius);

    //カプセルVsOBB
    static bool IntersectCapsuleVsOBB(
        const DirectX::XMVECTOR cap1StartPos,
        const DirectX::XMVECTOR cap1EndPos,
        float cap1Radius,
        DirectX::XMMATRIX boxWorldTransform);

};

