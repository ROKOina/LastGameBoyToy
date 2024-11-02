#pragma once
#include <DirectXMath.h>
#include "Graphics\Model\Model.h"
#include "Graphics/Model/ModelResource.h"

//アニメーション計算
class AnimationCalculation
{
public:
    //唯一のインスタンス取得
    static AnimationCalculation& Instance()
    {
        static AnimationCalculation instance;
        return instance;
    }

public:

    //アニメーション計算
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node& node);
    //アニメーション切り替え時の計算
    void ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node);
    //前回のアニメーションとのブレンド歩き専用
    void ComputeWalkIdleAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, float blendRate, float walkRate, Model::Node& node);
};