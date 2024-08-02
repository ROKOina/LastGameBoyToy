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
        int id;
        int damage;
    };

    //���M����ǉ�
    void SetNetSendData(NetSendData data) { sendData->Enqueue(data); }
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