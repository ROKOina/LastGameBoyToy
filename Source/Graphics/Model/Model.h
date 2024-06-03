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

    // アニメーション
    bool IsPlayAnimation() const { return currentAnimation >= 0; }
    void PlayAnimation(int animationIndex, bool loop = false);
    void StopAnimation();
    void UpdateAnimation(float elapsedTime);
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Node& node);

    // 行列計算
    void UpdateTransform(const DirectX::XMMATRIX& Transform);

    const std::vector<Node>& GetNodes() const { return nodes; }
    std::vector<Node>& GetNodes() { return nodes; }
    const ModelResource* GetResource() const { return resource.get(); }
    ModelResource* GetResource() { return resource.get(); }

    // アニメーション時間
    float GetAnimationSeconds() const { return currentSeconds; }
    void SetAnimationSeconds(float seconds) { currentSeconds = seconds; }

    //シリアライズに適用させる為の処理
    void CopyModel();

    //オープンノードをした時に逆に情報を入れる
    void CopyRefrectModel();

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
    int								currentAnimation = -1;
    float							currentSeconds = 0.0f;
    bool							loopAnimation = false;
    bool							endAnimation = false;
};