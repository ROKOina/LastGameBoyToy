#pragma once
#include <DirectXMath.h>
#include "Graphics\Model\Model.h"
#include "Graphics/Model/ModelResource.h"

//�A�j���[�V�����v�Z
class AnimationCalculation
{
public:
    //�B��̃C���X�^���X�擾
    static AnimationCalculation& Instance()
    {
        static AnimationCalculation instance;
        return instance;
    }

public:

    //�A�j���[�V�����v�Z
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node& node);
    //�A�j���[�V�����؂�ւ����̌v�Z
    void ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node);
    //�O��̃A�j���[�V�����Ƃ̃u�����h������p
    void ComputeWalkIdleAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, float blendRate, float walkRate, Model::Node& node);
};