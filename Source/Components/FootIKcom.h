#pragma once
#include "System\Component.h"
#include "Components\RendererCom.h"
class FootIKCom : public Component
{
public:
    enum class LegNodes{
        RIGHT_WAIST,   //�� �m�[�h�̃��C���[���Q�Ƃ���̂�3����
        RIGHT_KNEES,       //�G
        RIGHT_ANKLE,       //����
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
    DirectX::XMFLOAT3 GetTargetPosition(Legs leg);
    //�Z�b�^�[
    void SetLegNodes() {};
private:
    //���m�[�h��ێ�
    Model::Node* legNodes[(int)LegNodes::Max] = {};
    DirectX::XMFLOAT3* rotate[(int)LegNodes::Max] = {};

    //�^�[�Q�b�g�|�W�V����
    DirectX::XMFLOAT3 targetPos[(int)Legs::Max] = {};

    //StageModel
    Model*  stageModel;
};