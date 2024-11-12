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

private:

    //�V���A���C�Y
    void Serialize();

    // �f�V���A���C�Y
    void Deserialize(const char* filename);

    // �f�V���A���C�Y�̓ǂݍ���
    void LoadDeserialize();

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

public:

    //static�R���W�����̃p�����[�^
    struct StaticCollsionParameter
    {
        int collsiontype = {};
        float radius = 0.0f;
        DirectX::XMFLOAT3 scale = { 0,0,0 };
        DirectX::XMFLOAT3 offsetpos = { 0,0,0 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    std::unordered_map<int, std::vector<StaticCollsionParameter>>scp;

private:

    const char* m_filename = {};
    Model* model = nullptr;
    bool debugrender = false;
    CollsionType m_collsiontype = CollsionType::SPHER;
};