#pragma once

#include <DirectXMath.h>
#include <sstream>
#include <vector>
#include <array>

#define MAX_BUFFER_NET 65536

//XMFLOAT4
static std::stringstream& operator<<(std::stringstream& out, const DirectX::XMFLOAT4& h)
{
    out << h.x << " " << h.y << " " << h.z << " " << h.w;
    return out;
}
static std::stringstream& operator>>(std::stringstream& in, DirectX::XMFLOAT4& h)
{
    in >> float(h.x) >> float(h.y) >> float(h.z) >> float(h.w);
    return in;
}
//XMFLOAT3
static std::stringstream& operator<<(std::stringstream& out, const DirectX::XMFLOAT3& h)
{
    out << h.x << " " << h.y << " " << h.z;
    return out;
}
static std::stringstream& operator>>(std::stringstream& in, DirectX::XMFLOAT3& h)
{
    in >> float(h.x) >> float(h.y) >> float(h.z);
    return in;
}
//XMFLOAT2
static std::stringstream& operator<<(std::stringstream& out, const DirectX::XMFLOAT2& h)
{
    out << h.x << " " << h.y;
    return out;
}
static std::stringstream& operator>>(std::stringstream& in, DirectX::XMFLOAT2& h)
{
    in >> float(h.x) >> float(h.y);
    return in;
}

//array
//float
static std::stringstream& operator>>(std::stringstream& in, std::array<float, 4>& h)
{
    in >> float(h[0]) >> float(h[1]) >> float(h[2]) >> float(h[3]);
    return in;
}
static std::stringstream& operator<<(std::stringstream& out, const std::array<float, 4>& h)
{
    out << h[0] << " " << h[1] << " " << h[2] << " " << h[3];
    return out;
}
//int
static std::stringstream& operator>>(std::stringstream& in, std::array<int, 4>& h)
{
    in >> int(h[0]) >> int(h[1]) >> int(h[2]) >> int(h[3]);
    return in;
}
static std::stringstream& operator<<(std::stringstream& out, const std::array<int, 4>& h)
{
    out << h[0] << " " << h[1] << " " << h[2] << " " << h[3];
    return out;
}

////�t���[���Ɠ���
//struct InputFrame
//{
//    int progressFrame;  //�o�߃t���[��
//    unsigned int input; //����
//};

//struct InputData
//{
//    std::vector<unsigned int> input;
//    std::vector<unsigned int> inputDown;
//    std::vector<unsigned int> inputUp;
//};
//

struct SaveBuffer
{
    int frame;
    unsigned int inputDown = 0;
    unsigned int input = 0;
    unsigned int inputUp = 0;

    DirectX::XMFLOAT2 leftStick = { 0,0 };    //�ړ�
    DirectX::XMFLOAT3 pos = { 0,0,0 };       //�ʒu
    DirectX::XMFLOAT4 rotato = { 0,0,0,1 };       //��]

    DirectX::XMFLOAT3 fpsDir = { 0,0,1 };       //FPS���_�̕���

    DirectX::XMFLOAT3 velo = { 0,0,0 };       //����
};
//SaveBuffer
static void VectorSaveBufferOut(std::stringstream& out, std::vector<SaveBuffer>& vec)
{
    int size = vec.size();
    out << size << " ";
    for (auto& v : vec)
    {
        out << v.frame << " " << v.input << " " << v.inputDown << " " << v.inputUp << " ";
        out << v.leftStick << " ";
        out << v.pos << " ";
        out << v.rotato << " ";
        out << v.fpsDir << " ";
        out << v.velo << " ";
    }
}
static void VectorSaveBufferIn(std::stringstream& in, std::vector<SaveBuffer>& vec)
{
    int size;
    in >> size;
    for (int i = 0; i < size; ++i)
    {
        SaveBuffer s;
        in >> s.frame >> s.input >> s.inputDown >> s.inputUp;
        in >> s.leftStick;
        in >> s.pos;
        in >> s.rotato;
        in >> s.fpsDir;
        in >> s.velo;
        vec.emplace_back(s);
    }
}
//Vector3
static void Vector3Out(std::stringstream& out, std::array<DirectX::XMFLOAT3, 4>& vec)
{
    out << vec[0] << " ";
    out << vec[1] << " ";
    out << vec[2] << " ";
    out << vec[3] << " ";
}
static void Vector3In(std::stringstream& in, std::array<DirectX::XMFLOAT3, 4>& vec)
{
    in >> vec[0];
    in >> vec[1];
    in >> vec[2];
    in >> vec[3];
}


struct NetData
{
    //�f�[�^���
    enum DATA_KIND {
        GAME,
        JOIN,
    };
    int dataKind;
    bool isMasterClient;
    int id;
    char name[50];

    //�Q�[����
    struct GameData //0
    {
        int startTime;

        std::vector<SaveBuffer> saveInputBuf;

        std::array<int, 4> damageData;//�L�����ɗ^�����_���[�W
        std::array<int, 4> healData;//�L�����ɗ^�����q�[��
        std::array<float, 4> stanData;//�L�����ɗ^�����X�^��
        std::array<DirectX::XMFLOAT3, 4> knockbackData = {};//�m�b�N�o�b�N��^����
        std::array<DirectX::XMFLOAT3, 4> movePosData = {};//�ړ��ʒu��^����
        std::array<int, 4> teamID;//�`�[����ID
        int charaID;    //�L������ID
    }gameData;

    //��������
    int joinNum;    //����JoinData���ǂꂾ�������邩
    struct JoinData //1
    {
        //�����\��
        bool joinRequest;

        //��������(�z�X�g�̂�)
        bool joinPermission;
        int id;
    };
    std::vector<JoinData> joinData;

    //int pSize;
    //std::vector<int> p;
};
static std::stringstream& operator<<(std::stringstream& out, NetData& h)
{
    out << h.dataKind << " ";
    out << h.isMasterClient << " ";
    out << h.id << " ";
    out << h.name << " ";

    if (h.dataKind == NetData::DATA_KIND::GAME)
    {
        out << h.gameData.startTime << " ";
        out << h.gameData.damageData << " ";
        out << h.gameData.healData << " ";
        out << h.gameData.stanData << " ";

        Vector3Out(out, h.gameData.knockbackData);

        Vector3Out(out, h.gameData.movePosData);

        out << h.gameData.teamID << " ";
        out << h.gameData.charaID << " ";
        VectorSaveBufferOut(out, h.gameData.saveInputBuf);
    }

    out << (int)h.joinData.size() << " ";
    if (h.dataKind == NetData::DATA_KIND::JOIN)
    {
        for (auto& j : h.joinData) {
            out << j.joinRequest << " ";
            out << j.joinPermission << " ";
            out << j.id << " ";
        }
    }

    //for (auto& i : h.p)
    //{
    //}
    return out;
}
static std::stringstream& operator>>(std::stringstream& in, NetData& h)
{
    in >> h.dataKind;
    in >> h.isMasterClient;
    in >> h.id;
    in >> h.name;

    if (h.dataKind == NetData::DATA_KIND::GAME)
    {
        in >> h.gameData.startTime;
        in >> h.gameData.damageData;
        in >> h.gameData.healData;
        in >> h.gameData.stanData;

        Vector3In(in, h.gameData.knockbackData);

        Vector3In(in, h.gameData.movePosData);

        in >> h.gameData.teamID;
        in >> h.gameData.charaID;
        VectorSaveBufferIn(in, h.gameData.saveInputBuf);
    }

    in >> h.joinNum;
    if (h.dataKind == NetData::DATA_KIND::JOIN)
    {
        for (int j = 0; j < h.joinNum; ++j) {
            auto& join = h.joinData.emplace_back();
            in >> join.joinRequest;
            in >> join.joinPermission;
            in >> join.id;
        }
    }

    return in;
}

//���M�f�[�^�ɕϊ�
static std::stringstream NetDataSendCast(std::vector<NetData>& n)
{
    //���M�^�ɕϊ����ăf�[�^��S�đ���
    std::stringstream ss = {};
    for (auto& data : n)
    {
        ss << " ";
        ss << data;
    }

    return ss;
}

//��M�f�[�^��ϊ�
static std::vector<NetData> NetDataRecvCast(std::string& recvData)
{
    NetData n;
    std::vector<NetData> data;
    std::stringstream ss(recvData);
    //while (ss)
    //{
    //    ss >> n;
    //    data.emplace_back(n);
    //}
    while (ss >> n)
    {
        data.emplace_back(n);
    }

    return data;
}
