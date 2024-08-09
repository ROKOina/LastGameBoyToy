#pragma once

#include "../RingBuffer.h"

//�N���C�A���g�ɑ��M��������Ǘ�����
class StaticSendDataManager
{
public:
    StaticSendDataManager() 
        : sendData(std::make_unique<RingBuffer<NetSendData>>(100))
    {}
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

        int sendType;   //0:damage 1:heal 2:stan

        int value;
    };

    //�_���[�W�𑗐M
    void SetSendDamage(int myID, int sendID, int damage);

    //�q�[���𑗐M
    void SetSendHead(int myID, int sendID, int damage);

    //���M����S�Ď擾���ď���
    std::vector<NetSendData> GetNetSendDatas() 
    {
        std::vector<NetSendData> netDatas;
        while (1)
        {
            if (sendData->GetSize() > 0)
            {
                auto n = sendData->Dequeue();
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
    std::unique_ptr<RingBuffer<NetSendData>> sendData;
};