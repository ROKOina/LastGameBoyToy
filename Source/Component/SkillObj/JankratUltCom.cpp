#include "JankratUltCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Particle\CPUParticle.h"

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

    //���Ԍo��
    time += elapsedTime;

    //�n�ʔ���
    if (move->OnGround())
    {
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
            const auto& explosion = GetGameObject()->GetChildFind("explosion");
            explosion->GetComponent<CPUParticle>()->SetActive(true);
        }
    }

    //���ԂɂȂ�΍폜
    if (time > 7.0f)
    {
        GameObjectManager::Instance().Remove(GetGameObject());
    }
}