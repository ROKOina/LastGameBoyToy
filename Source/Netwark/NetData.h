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
static std::stringstream& operator>>(std::stringstream& in, std::array<float,6>& h)
{
    in >> float(h[0]) >> float(h[1]) >> float(h[2]) >> float(h[3]) >> float(h[4]) >> float(h[5]);
    return in;
}
static std::stringstream& operator<<(std::stringstream& out, const std::array<float, 6>& h)
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


struct NetData
{
    int id;
    float radi;
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 velocity;
    DirectX::XMFLOAT3 nonVelocity;
    DirectX::XMFLOAT4 rotato;
    unsigned int inputDown;
    unsigned int input;
    unsigned int inputUp;
    long long nowFrame;
    std::array<float,6> damageData;//キャラに与えたダメージ

    //int pSize;
    //std::vector<int> p;
};
static std::stringstream& operator<<(std::stringstream& out, const NetData& h)
{
    out << h.id << " " << h.radi << " ";
    out << h.pos << " ";
    out << h.velocity << " ";
    out << h.nonVelocity << " ";
    out << h.rotato << " ";
    out << h.input << " "<< h.inputDown << " "<< h.inputUp << " ";
    out << h.nowFrame << " ";
    out << h.damageData << " ";
    //for (auto& i : h.p)
    //{

    //}
    return out;
}
static std::stringstream& operator>>(std::stringstream& in, NetData& h)
{
    in >> h.id >> h.radi;
    in >> h.pos;
    in >> h.velocity;
    in >> h.nonVelocity;
    in >> h.rotato;
    in >> h.input>> h.inputDown >> h.inputUp;
    in >> h.nowFrame;
    in >> h.damageData;
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


