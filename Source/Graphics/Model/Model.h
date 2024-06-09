#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "ModelResource.h"

class Model
{
public:
    Model(std::shared_ptr<ModelResource> resource);
    ~Model() {}

    struct Node
    {
        const char* name;
        Node* parent;
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotate;
        DirectX::XMFLOAT3	translate;
        DirectX::XMFLOAT4X4	localTransform;
        DirectX::XMFLOAT4X4	worldTransform;
        int layer;

        std::vector<Node*>	children;
    };

   
    // 行列計算
    void UpdateTransform(const DirectX::XMMATRIX& Transform);

    
    const std::vector<Node>& GetNodes() const { return nodes; }
    std::vector<Node>& GetNodes() { return nodes; }
    const ModelResource* GetResource() const { return resource.get(); }
    ModelResource* GetResource() { return resource.get(); }

   

    //シリアライズに適用させる為の処理
    void CopyModel();

    //オープンノードをした時に逆に情報を入れる
    void CopyRefrectModel();

public:
    //ノード検索　上半身分別
    Node* FindNode(const char* name);

    //特定のノードの番号検索
    int FindNodeIndex(const char* name);


private:

    enum Layer
    {
        ALLBODY,
        UPPER,
        LOWER,
        LAYERMAX
    };

private:
    std::shared_ptr<ModelResource>	resource;
    std::vector<Node>				nodes;
    
};