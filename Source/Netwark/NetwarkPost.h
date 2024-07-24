#pragma once

#include <string>
#include "NetData.h"
#include "RingBuffer.h"

//���S�����ݒ�
//#define PerfectSyn

//�ȈՃt���[������
//#define EasyFrameSyn

//�x������
#define DelaySystem

class NetwarkPost
{
public:
    __fastcall NetwarkPost() {}
    virtual __fastcall ~NetwarkPost();

    virtual void __fastcall Initialize() = 0;

    virtual void __fastcall Update() = 0;

    virtual void ImGui() = 0;

    virtual void RenderUpdate();

public:
     std::vector<NetData>& GetNetDatas() { return clientDatas; }
    const int GetNetId() { return id; }

    const bool IsNextFrame() { return isNextFrame; }

protected:
    //�t���[�������ł��Ă��邩�i�o���Ă�Ȃ�true�j
    bool IsSynchroFrame(bool );

protected:

    unsigned __int64 sock;
    unsigned __int64 multicastSock;

    std::string recvData = "ABC";
    std::vector<NetData> clientDatas;
    int id;
    std::vector<int> newLoginID;

    unsigned int input;
    unsigned int inputUp;
    unsigned int inputDown;

    //�e�N���C�A���g�̃t���[���ۑ�
    long long nowFrame;
    

    //�����O�o�b�t�@
    std::unique_ptr<RingBuffer<SaveBuffer>> bufRing;

    //���S�����p
    bool isEndJoin = false; //�����I���t���O
    bool isNextFrame = false;   //���̃t���[���ɍs���邩
    int clientNum;

    //�f�B���C�����p
    const float delaySec = 6/60;
    int delayNow;
    long long playFrame;    //���ݎ��s�t���[��

    //�C���v�b�g�ۑ��i�T�[�o�[�p�j
    struct SaveInput
    {
        int id;
        std::unique_ptr<RingBuffer<SaveBuffer>> inputBuf;
        SaveInput()
        {
            inputBuf = std::make_unique<RingBuffer<SaveBuffer>>(500);
        }
    };
    std::vector<SaveInput> saveInput;
};