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
static std::stringstream& operator>>(std::stringstream& in, std::array<float, 6>& h)
{
    in >> float(h[0]) >> float(h[1]) >> float(h[2]) >> float(h[3]) >> float(h[4]) >> float(h[5]);
    return in;
}
static std::stringstream& operator<<(std::stringstream& out, const std::array<float, 6>& h)
{
    out << h[0] << " " << h[1] << " " << h[2] << " " << h[3] << " " << h[4] << " " << h[5];
    return out;
}
//int
static std::stringstream& operator>>(std::stringstream& in, std::array<int, 6>& h)
{
    in >> int(h[0]) >> int(h[1]) >> int(h[2]) >> int(h[3]) >> int(h[4]) >> int(h[5]);
    return in;
}
static std::stringstream& operator<<(std::stringstream& out, const std::array<int, 6>& h)
{
    out << h[0] << " " << h[1] << " " << h[2] << " " << h[3] << " " << h[4] << " " << h[5];
    return out;
}


////フレームと入力
//struct InputFrame
//{
//    int progressFrame;  //経過フレーム
//    unsigned int input; //入力
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

    DirectX::XMFLOAT2 leftStick = { 0,0 };    //移動
    DirectX::XMFLOAT3 pos = { 0,0,0 };       //位置
    DirectX::XMFLOAT4 rotato = { 0,0,0,1 };       //回転

    DirectX::XMFLOAT3 fpsDir = { 0,0,1 };       //FPS視点の方向

    DirectX::XMFLOAT3 velo = { 0,0,0 };       //速力
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
static void Vector3Out(std::stringstream& out, std::array<DirectX::XMFLOAT3, 6>& vec)
{
    out << vec[0] << " ";
    out << vec[1] << " ";
    out << vec[2] << " ";
    out << vec[3] << " ";
    out << vec[4] << " ";
    out << vec[5] << " ";
}
static void Vector3In(std::stringstream& in, std::array<DirectX::XMFLOAT3, 6>& vec)
{
    in >> vec[0];
    in >> vec[1];
    in >> vec[2];
    in >> vec[3];
    in >> vec[4];
    in >> vec[5];
}


struct NetData
{
    bool isMasterClient;
    int id;

    std::vector<SaveBuffer> saveInputBuf;

    std::array<int, 6> damageData;//キャラに与えたダメージ
    std::array<int, 6> healData;//キャラに与えたヒール
    std::array<float, 6> stanData;//キャラに与えたスタン
    std::array<DirectX::XMFLOAT3, 6> knockbackData = {};//ノックバックを与える
    std::array<DirectX::XMFLOAT3, 6> movePosData = {};//移動位置を与える
    std::array<int, 6> teamID;//チームのID
    int charaID;    //キャラのID

    //int pSize;
    //std::vector<int> p;
};
static std::stringstream& operator<<(std::stringstream& out, NetData& h)
{
    out << h.isMasterClient << " ";
    out << h.id << " " ;
    out << h.damageData << " ";
    out << h.healData << " ";
    out << h.stanData << " ";

    Vector3Out(out, h.knockbackData);

    Vector3Out(out, h.movePosData);

    out << h.teamID << " ";
    out << h.charaID << " ";
    VectorSaveBufferOut(out, h.saveInputBuf);

    //for (auto& i : h.p)
    //{

    //}
    return out;
}
static std::stringstream& operator>>(std::stringstream& in, NetData& h)
{
    in >> h.isMasterClient;
    in >> h.id;
    in >> h.damageData;
    in >> h.healData;
    in >> h.stanData;

    Vector3In(in, h.knockbackData);

    Vector3In(in, h.movePosData);

    in >> h.teamID;
    in >> h.charaID;
    VectorSaveBufferIn(in, h.saveInputBuf);

    return in;
}

//送信データに変換
static std::stringstream NetDataSendCast(std::vector<NetData>& n)
{
    //送信型に変換してデータを全て送る
    std::stringstream ss = {};
    for (auto& data : n)
    {
        ss << " ";
        ss << data;
    }

    return ss;
}

//受信データを変換
static std::vector<NetData> NetDataRecvCast(std::string& recvData)
{
    NetData n;
    std::vector<NetData> data;
    std::stringstream ss(recvData);
    while (ss >> n)
    {
        data.emplace_back(n);
    }

    return data;
}


