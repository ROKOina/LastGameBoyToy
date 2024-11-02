#pragma once

#include "Component\System\Component.h"
#include "Graphics\Model\Model.h"

//�A�j���[�V����
class NodeCollsionCom : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    NodeCollsionCom(const char* filename);
    ~NodeCollsionCom() {}

    // ���O�擾
    const char* GetName() const override { return "NodeCollsion"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

public:

    //�f�o�b�O�v���~�e�B�u�̌`��
    enum class CollsionType
    {
        SPHER,
        CYLINDER,
        BOX,
        MAX
    };

    //���ʖ���enumclass
    enum class BosyPartType
    {
        NONE,
        BODY,
        FACE,
        RIGHTHAND,
        LEFTHAND,
        RIGHTLEG,
        LEFTLEG,
        MAX
    };

    CollsionType GetColiisionType() { return m_collsiontype; }

private:

    CollsionType m_collsiontype = CollsionType::SPHER;

private:

    const char* m_filename = {};
    Model* model = nullptr;
    bool debugrender = false;
};