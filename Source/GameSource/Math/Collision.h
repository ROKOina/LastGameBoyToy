#pragma once

#include <DirectXMath.h>
#include "Graphics/Model/Model.h"

//�q�b�g����
struct HitResult
{
    DirectX::XMFLOAT3 position = { 0,0,0 }; //���C�ƃ|���S���̌�_
    DirectX::XMFLOAT3 normal = { 0,0,0 };   //�Փ˂����|���S���̖@���x�N�g��
    float distance = 0.0f;  //���C�̎n�_�����_�܂ł̋���
    int materialIndex = -1; //�Փ˂����|���S���̃}�e���A���ԍ�
    DirectX::XMFLOAT3 rotation = { 0,0,0 }; //��]��
};

//�R���W����
class Collision
{
public:
    //���Ƌ��̌�������
    static bool IntersectSphereVsSphere(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        DirectX::XMFLOAT3& outPositionB
    );

    //�~���Ɖ~���̌�������
    static bool IntersectCylinder(
        const DirectX::XMFLOAT3& positionA,
        float radiusA,
        float heightA,
        const DirectX::XMFLOAT3& positionB,
        float radiusB,
        float heightB,
        DirectX::XMFLOAT3& outPositionB
    );

    //���Ɖ~���̌�������
    static bool IntersectSphereVsCylider(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        float cylinderRadius,
        float cylinderHeight,
        DirectX::XMFLOAT3& outCylinderPosition
    );

    //���Ɖ~���̌�������(��]����)
    static bool Collision::IntersectSphereVsCylider(
        const DirectX::XMFLOAT3& spherePosition,
        float sphereRadius,
        const DirectX::XMFLOAT3& cylinderPosition,
        const DirectX::XMFLOAT3& cylinderDir,
        float cylinderRadius,
        float cylinderHeight
    );

    //���C�ƃ��f���̌�������
    static bool IntersectRayVsModel(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        const Model* model,
        HitResult& result
    );

    static bool IntersectRayVsSphere(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// �v���K��
        float rayDist,
        const DirectX::XMVECTOR& spherePos,
        float radius,
        HitResult& result
    );

    // ���CVs�~��
    static bool IntersectRayVsOrientedCylinder(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// �v���K��
        float rayDist,
        const DirectX::XMVECTOR& startCylinder,
        const DirectX::XMVECTOR& endCylinder,
        float radius,
        HitResult& result,
        DirectX::XMVECTOR* onCenterLinPos = {});

    // ���CVsBOX�iOBB�j
    static bool IntersectRayVsBOX(
        const DirectX::XMVECTOR& rayStart,
        const DirectX::XMVECTOR& rayDirection,		// �v���K��
        float rayDist,
        const DirectX::XMMATRIX boxWorldTransform,
        HitResult& result);

    //�@��VsOBB
    static bool IntersectSphereVsOBB(
        const DirectX::XMVECTOR spherePos,
        float radius,
        DirectX::XMMATRIX boxWorldTransform);

    //�J�v�Z��Vs�~��
    static bool IntersectCapsuleVsCylinder(
        const DirectX::XMVECTOR cap1StartPos,
        const DirectX::XMVECTOR cap1EndPos,
        float cap1Radius,
        const DirectX::XMVECTOR cly2StartPos,
        const DirectX::XMVECTOR cly2EndPos,
        float cly2Radius);

    //�J�v�Z��VsOBB
    static bool IntersectCapsuleVsOBB(
        const DirectX::XMVECTOR cap1StartPos,
        const DirectX::XMVECTOR cap1EndPos,
        float cap1Radius,
        DirectX::XMMATRIX boxWorldTransform);

};

