#pragma once

#include <DirectXMath.h>
#include <sstream>
#include <vector>

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



struct NetData
{
    int id;
    float radi;
    DirectX::XMFLOAT3 pos;
};

static std::stringstream& operator<<(std::stringstream& out, const NetData& h)
{
    out << h.id << " " << h.radi << " ";
    out << h.pos;
    return out;
}

static std::stringstream& operator>>(std::stringstream& in, NetData& h)
{
    in >> h.id >> h.radi;
    in >> h.pos;
    return in;
}

