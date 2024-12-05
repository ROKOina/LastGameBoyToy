#pragma once

#include "../RingBuffer.h"
#include <DirectXMath.h>

//�N���C�A���g�ɑ��M��������Ǘ�����
class StaticSendDataManager
{
public:
    StaticSendDataManager()
        : sendGameData(std::make_unique<RingBuffer<NetSendData>>(100))
    {
    }
    ~StaticSendDataManager() {}

    // �C���X�^���X�擾
    static StaticSendDataManager& Instance()
    {
        static StaticSendDataManager instance;
        return instance;
    }

    //���M���
    struct NetSendData
    {
        int id; //���M����

        int sendType;   //0:damage 1:heal 2:stan 3:knockback 4:movePos

        int valueI;
        float valueF;
        DirectX::XMFLOAT3 valueF3;
    };

    //�_���[�W�𑗐M
    void SetSendDamage(int myID, int sendID, float damage);

    //�q�[���𑗐M
    void SetSendHeal(int myID, int sendID, float heal);

    //�X�^���𑗐M
    void SetSendStan(int myID, int sendID, float stanSec);

    //�m�b�N�o�b�N�𑗐M
    void SetSendKnockback(int myID, int sendID, DirectX::XMFLOAT3 knockbackVec);

    //�ړ��ʒu�𑗐M
    void SetSendMovePos(int myID, int sendID, DirectX::XMFLOAT3 movePos);

    //���M����S�Ď擾���ď���
    std::vector<NetSendData> GetNetSendDatas()
    {
        std::vector<NetSendData> netDatas;
        while (1)
        {
            if (sendGameData->GetSize() > 0)
            {
                auto n = sendGameData->Dequeue();
                netDatas.emplace_back(n);
            }
            else
            {
                break;
            }
        }
        return netDatas;
    }

private:
    std::unique_ptr<RingBuffer<NetSendData>> sendGameData;
};