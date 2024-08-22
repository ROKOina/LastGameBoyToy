#pragma once
#include "System\Component.h"
#include "Components\RendererCom.h"
class FootIKCom : public Component
{
public:
    enum class LegNodes{
        RIGHT_WAIST,   //�� 
        RIGHT_KNEES,   //�G
        RIGHT_ANKLE,   //����
        LEFT_WAIST,         
        LEFT_KNEES,
        LEFT_ANKLE,
        Max,
    };

    enum class Legs {
        RIGHT,    //�m�[�h�̃��C���[���Q�Ƃ���̂�3����
        LEFT,
        Max,
    };

public:
    //�R���|�[�l���g�I�[�o�[���C�h
    FootIKCom() {}
    ~FootIKCom() {}

    // ���O�擾
    const char* GetName() const override { return "FootIK"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    

public:
    //�^�[�Q�b�g�|�W�V�����擾
    bool GetTargetPosition(Legs leg, DirectX::XMFLOAT3& resultPos);
    //�Z�b�^�[
    void SetLegNodes() {};

    //�{�[���̌v�Z
    void MoveBone(Legs leg);

    //�{�[���̃��[���h�s��X�V
    void UpdateWorldTransform(Model::Node* legNode);
private:
    //���m�[�h��ێ�
    Model::Node* legNodes[(int)LegNodes::Max] = {};
    DirectX::XMFLOAT3* rotate[(int)LegNodes::Max] = {};

    //�^�[�Q�b�g�|�W�V����
    DirectX::XMFLOAT3 targetPos[(int)Legs::Max] = {};

    //�^�[�Q�b�g�|�[��
    DirectX::XMFLOAT4X4 poleLocalTransform[(int)Legs::Max] = {};
    DirectX::XMFLOAT4X4 poleWorldTransform[(int)Legs::Max] = {};
    
    //StageModel
    Model*  stageModel;
};