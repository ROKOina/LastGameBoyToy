#pragma once

#include "System\Component.h"
#include <intsafe.h>

class RayCollisionCom : public Component, public std::enable_shared_from_this<RayCollisionCom>
{
public:
    // レイキャストで使用する頂点情報
    struct CollisionVertex
    {
        DirectX::XMFLOAT3	position = { 0, 0, 0 };
        DirectX::XMFLOAT3	normal = { 0, 0, 0 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    struct Collision
    {
        // 頂点の情報
        struct VertexData
        {
            UINT keyID = -1;
            UINT vertexA = -1; // ポリゴンを構成する最初の頂点
        };
        // AABBでグループ分けしたポリゴン
        struct PolygonGroupe {
            DirectX::XMFLOAT3 aabbCenter = {};
            DirectX::XMFLOAT3 aabbRadius = {};

            // このグループに含まれる頂点情報
            std::vector<VertexData>	      vertexDatas;
        };

        // ポリゴンのグループ
        std::vector<PolygonGroupe> polygonGroupes;
        std::vector<std::vector<CollisionVertex>> vertices;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

public:
    RayCollisionCom(const char* filePath);
    ~RayCollisionCom() {}

    // 名前取得
    const char* GetName() const override { return "RayCollision"; }

    void Start()override;

    void OnDestroy()override;

    const Collision& GetCollision() { return collision; }

private:
    void Deserialize(const char* filePath);

private:
    Collision collision;
};
