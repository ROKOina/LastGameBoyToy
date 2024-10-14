#pragma once
#include "PxPhysicsAPI.h"
#include <memory>

class PhysXLib
{
private:
    PhysXLib() {};
    ~PhysXLib() {};

public:
    // �C���X�^���X�擾
    static PhysXLib& Instance()
    {
        static PhysXLib instance;
        return instance;
    }

    //������
    void Initialize();

    //�I����
    void Finalize() {};

    //�X�V����
    void Update(float elapsedTime);

private:
    // PhysX���ŗ��p����A���P�[�^�[
    physx::PxDefaultAllocator m_defaultAllocator;
    // �G���[���p�̃R�[���o�b�N�ŃG���[���e�������Ă�
    physx::PxDefaultErrorCallback m_defaultErrorCallback;
    // ��ʃ��x����SDK(PxPhysics�Ȃ�)���C���X�^���X������ۂɕK�v
    std::unique_ptr<physx::PxFoundation> m_pFoundation = nullptr; 
    // ���ۂɕ������Z���s��
    std::unique_ptr<physx::PxPhysics> m_pPhysics = nullptr;
    // �V�~�����[�V�������ǂ��������邩�̐ݒ�Ń}���`�X���b�h�̐ݒ���ł���
    std::unique_ptr<physx::PxDefaultCpuDispatcher> m_pDispatcher = nullptr;
    // �V�~�����[�V���������Ԃ̒P�ʂ�Actor�̒ǉ��Ȃǂ������ōs��
    std::unique_ptr<physx::PxScene> m_pScene = nullptr;
    // PVD�ƒʐM����ۂɕK�v
    std::unique_ptr<physx::PxPvd> m_pPvd = nullptr;
};