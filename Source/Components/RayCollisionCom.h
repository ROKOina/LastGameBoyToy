#pragma once

#include "System\Component.h"
#include <intsafe.h>

class RayCollisionCom : public Component, public std::enable_shared_from_this<RayCollisionCom>
{
public:
    // ���C�L���X�g�Ŏg�p���钸�_���
    struct CollisionVertex
    {
        DirectX::XMFLOAT3	position = { 0, 0, 0 };
        DirectX::XMFLOAT3	normal = { 0, 0, 0 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    struct Collision
    {
        // ���_�̏��
        struct VertexData
        {
            UINT keyID = -1;
            UINT vertexA = -1; // �|���S�����\������ŏ��̒��_
        };
        // AABB�ŃO���[�v���������|���S��
        struct PolygonGroupe {
            DirectX::XMFLOAT3 aabbCenter = {};
            DirectX::XMFLOAT3 aabbRadius = {};

            // ���̃O���[�v�Ɋ܂܂�钸�_���
            std::vector<VertexData>	      vertexDatas;
        };

        // �|���S���̃O���[�v
        std::vector<PolygonGroupe> polygonGroupes;
        std::vector<std::vector<CollisionVertex>> vertices;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

public:
    RayCollisionCom(const char* filePath);
    ~RayCollisionCom() {}

    // ���O�擾
    const char* GetName() const override { return "RayCollision"; }

    void Start()override;

    void OnDestroy()override;

    const Collision& GetCollision() { return collision; }

private:
    void Deserialize(const char* filePath);

private:
    Collision collision;
};
