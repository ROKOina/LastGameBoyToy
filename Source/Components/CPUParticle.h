#pragma once

#include "Graphics/Shaders/ConstantBuffer.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/Texture.h"
#include "Components/System/Component.h"

class CPUParticle :public Component
{
public:

    CPUParticle(const char* filename, int num);
    ~CPUParticle() { delete[] m_data; delete[] m_v; }

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render();

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "CPUParticle"; }

private:

    //セット
    void Set(int type, float time, DirectX::XMFLOAT3 p, DirectX::XMFLOAT4 r, DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size);

    //パーティクルの動き
    void ParticleMove(float elapsedTime);

    //シリアライズ
    void Serialize();

    //デシリアライズ
    void Desirialize(const char* filename);

    //読み込み
    void LoadDesirialize();

    //リセット
    void Reset();

private:

    //頂点構造体
    struct VERTEX
    {
        DirectX::XMFLOAT3 Pos;    //位置
        DirectX::XMFLOAT4 Rotate; //回転
        DirectX::XMFLOAT3 Normal; //法線
        DirectX::XMFLOAT2 Tex;    //UV座標
        DirectX::XMFLOAT4 Color;  //色
        DirectX::XMFLOAT4 Param;  //汎用パラメータ
    };
    VERTEX* m_v = {}; //頂点

    //パーティクルの基本情報
    struct ParticleData
    {
        float x, y, z;
        float rx, ry, rz, rw;
        float w, h;
        float vx, vy, vz;
        float ax, ay, az;
        float alpha;
        float timer;
        float anime_timer;
        float type;
    };
    ParticleData* m_data = {}; //パーティクルデータ

    //定数バッファ
    struct CPUParticleConstant
    {
        DirectX::XMFLOAT4 particlecolor = { 1,1,1,1 };
        DirectX::XMFLOAT3 intensity = { 1,1,1 };
        float dummy = {};
    };
    std::unique_ptr<ConstantBuffer<CPUParticleConstant>>m_cc;

public:

    //シリアライズする情報
    struct SerializeCPUParticle
    {
        int m_verticalkoma = 1;
        int m_besidekoma = 1;
        float m_animationspeed = 0.1f;
        int m_max = 1;
        float m_lifetime = 0.0f;
        float m_radius = 0.0f;
        float m_scaleradius = 0.0f;
        int m_arc = 1;
        int m_blend = 2;
        int m_type = 0;
        float m_scalerandom = 0;
        float m_randomrotation = 0.0f;
        float m_speed = 0.0f;
        float m_string = 1.0f;
        float m_scalar = 1.0f;
        DirectX::XMFLOAT2 m_stringlate = { 1.0f,1.0f };
        std::string	m_filename;
        DirectX::XMFLOAT3 m_orbitalvelocity = { 0,0,0 };
        DirectX::XMFLOAT3 m_randombuoyancy = { 0,0,0 };
        DirectX::XMFLOAT3 m_randomvelocity = { 0,0,0 };
        DirectX::XMFLOAT3 m_buoyancy = { 0,0,0 };
        DirectX::XMFLOAT3 m_velocity = { 0,0,0 };
        DirectX::XMFLOAT2 m_scale = { 1,1 };
        DirectX::XMFLOAT2 m_latesize = { 1,1 };
        DirectX::XMFLOAT4 m_particlecolor = { 1,1,1,1 };
        DirectX::XMFLOAT3 m_intensity = { 1,1,1 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    //方向ベクトルを代入
    void SetDirection(const DirectX::XMFLOAT3& direction) { this->m_direction = direction; }

private:

    int m_numparticle = 0;
    bool m_active = true;
    bool m_pause = true;
    SerializeCPUParticle m_scp;
    DirectX::XMFLOAT3 m_direction = { 1.0f,1.0f,1.0f };

private:

    Microsoft::WRL::ComPtr<ID3D11VertexShader>m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_vertexbuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_shaderresourceview;
};