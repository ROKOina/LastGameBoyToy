#pragma once

#include "System\Component.h"
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/Shaders/ConstantBuffer.h"

#define THREAD 512

class GPUParticle :public Component
{
public:
    GPUParticle(const char* filename, size_t maxparticle);
    ~GPUParticle() {};

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render();

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName()const override { return "GPUParticle"; }

private:

    //リセット関数
    void Reset();

    //シリアライズ
    void Serialize();

    //デシリアライズ
    void Desirialize(const char* filename);

    //読み込み
    void LoadDesirialize();

    //パラメータリセット
    void ParameterReset();

public:

    //パーティクルの実体
    struct EntityParticle
    {
        DirectX::XMFLOAT3 position = { 0,0,0 };
        DirectX::XMFLOAT2 scale = { 0.0f,0.0f };
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
        DirectX::XMFLOAT3 velocity = { 0,0,0 };
        DirectX::XMFLOAT3 strechvelocity = { 0,0,0 };
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        float lifetime = 0.0f;
        float age = 0.0f;
        int isalive = {};
        int isstart = 0;
    };

    //コンスタントバッファ
    struct GPUParticleConstants
    {
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
        DirectX::XMFLOAT3 position = { 0,0,0 };
        int isalive = { true };
        int loop = { true };
        int startflag = { true };
        DirectX::XMFLOAT2 Gdummy = {};
    };
    std::unique_ptr<ConstantBuffer<GPUParticleConstants>>m_gpu;

    //保存するコンスタントバッファ
    struct GPUparticleSaveConstants
    {
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        DirectX::XMFLOAT4 startcolor = { 1,1,1,1 };
        DirectX::XMFLOAT4 endcolor = { 1,1,1,1 };
        DirectX::XMFLOAT4 shape = { 0,0.0f,0.0f,0.0f }; //ｘ：生成場所ランダム、ｙ：半径、ｚ：半径のボリューム、ｗ：円形にするパラ
        DirectX::XMFLOAT3 velocity = { 0,0,0 };
        float lifetime = 1.0f;
        DirectX::XMFLOAT3 luminance = { 1,1,1 };
        float speed = 1.0f;
        DirectX::XMFLOAT2 scale = { 0.2f,0.2f };
        float startsize = 1.0f;
        float endsize = 1.0f;
        DirectX::XMFLOAT3 orbitalvelocity = { 0,0,0 };
        float radial = { 0 };
        float startspeed = 1.0f;
        float endspeed = 1.0f;
        float velorandscale = 0.0f;
        int strechflag = 0;
        DirectX::XMFLOAT3 buoyancy = {};
        float startgravity = 0.0f;
        float endgravity = 0.0f;
        DirectX::XMFLOAT3 savedummy = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    GPUparticleSaveConstants m_GSC;

    //保存するパラメータ
    struct SaveParameter
    {
        int m_blend = 2;
        std::string	m_filename;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameter m_p;

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_particlebuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_particlesrv;
    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>m_particleuav;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>m_updatecomputeshader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader>m_initialzecomputeshader;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_colormap;
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_constantbuffer;
    const size_t m_maxparticle = 0;
};