#include "JankratUltCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Collsion\ColliderCom.h"

//�X�V����
void JankratUltCom::Update(float elapsedTime)
{
    //�N��
    Fire(elapsedTime);
}

//�N��
void JankratUltCom::Fire(float elapsedTime)
{
    // MovementCom ���擾
    const auto& move = GetGameObject()->GetComponent<MovementCom>();
    const auto& explosion = GetGameObject()->GetChildFind("explosion");

    //�n�ʔ���
    if (move->OnGround())
    {
        plustime = true;

        //���n�����u�Ԃ̏���
        if (move->JustLanded())
        {
            move->SetIsRaycast(false);
            move->ZeroNonMaxSpeedVelocity();
            move->ZeroVelocity();
            move->SetGravity(0.0f);

            //�����Ńp�[�e�B�N�����~
            GetGameObject()->GetComponent<CPUParticle>()->SetActive(false);

            //�΍ЃG�t�F�N�g�Đ�
            explosion->GetComponent<CPUParticle>()->SetActive(true);
        }
    }

    //���Ԃ��X�V����
    if (plustime)
    {
        //���Ԍo��
        time += elapsedTime;
    }

    //�΍ЃG�t�F�N�g��~
    if (time > 7.6f)
    {
        GetGameObject()->GetComponent<Collider>()->SetEnabled(false);
        explosion->GetComponent<CPUParticle>()->SetActive(false);
    }

    //���ԂɂȂ�΍폜
    if (time > 9.0f)
    {
        GameObjectManager::Instance().Remove(GetGameObject());
    }
}