#pragma once

#include "System\Component.h"
#include <wrl.h>
#include <d3d11.h>
#include "Graphics/Shaders/ConstantBuffer.h"

#define THREAD 1024

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

    // エフェクト再生関数
    void Play();

    //自身を消す関数
    void DeleteMe(float elapsedTime);

private:
    //シリアライズ
    void Serialize();

    //デシリアライズ
    void Desirialize(const char* filename);

    //読み込み
    void LoadDesirialize();

    //パラメータリセット
    void ParameterReset();

    // 設定などパラメーター以外を扱うGUI
    void SystemGUI();

    // パラメーター関係のGUI
    void ParameterGUI();

    void ColorGUI();
    void ScaleGUI();
    void SpeedGUI();
    void EmitGUI();

public:

    //パーティクルの実体
    struct EntityParticle
    {
        DirectX::XMFLOAT3 position = { 0,0,0 };
        DirectX::XMFLOAT2 scale = { 0.0f,0.0f };
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
        DirectX::XMFLOAT3 velocity = { 0,0,0 };
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        float lifetime = 0.0f;
        float age = 0.0f;
        int isalive = {};
    };

    //コンスタントバッファ
    struct GPUParticleConstants
    {
        DirectX::XMFLOAT4 rotation = { 0,0,0,1 };

        DirectX::XMFLOAT3 position = { 0,0,0 };
        int isalive = { true };

        DirectX::XMFLOAT3 currentEmitVec;
        int isEmitFlg = true;
    };
    std::unique_ptr<ConstantBuffer<GPUParticleConstants>>m_gpu;

    //アクティブ化
    void SetLoop(const bool& loop) { m_GSC.isLoopFlg = loop; }

    //保存するコンスタントバッファ
    struct GPUparticleSaveConstants
    {
        float emitTime = 1.0f;
        float lifeTime = 1.0f;
        int stretchFlag = 0;
        int isLoopFlg = 0;

        DirectX::XMFLOAT4 shape = { 0,0.0f,0.0f,0.0f }; //ｘ：生成場所ランダム、ｙ：半径、ｚ：半径のボリューム、ｗ：円形にするパラ

        DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };      // ベースとなる色

        DirectX::XMFLOAT4 lifeStartColor = { 1,1,1,1 }; // パーティクルの生成時の色
        DirectX::XMFLOAT4 lifeEndColor = { 1,1,1,1 };   // パーティクルの消滅時の色
        // -------------------  ↑↓ どちらかのみ  ----------------------
        DirectX::XMFLOAT4 emitStartColor = { 1,1,1,1 }; // エフェクトの再生開始時の色
        DirectX::XMFLOAT4 emitEndColor = { 1,1,1,1 };   // エフェクト再生後の最終的な色

        int colorVariateByLife = 0;               // 色の変化の基準を管理 ( TRUE : 寿命によって変化 )
        DirectX::XMFLOAT3 colorScale = { 1,1,1 }; // 色を更に明るくするなどで使用

        DirectX::XMFLOAT3 emitVec = { 0,0,0 };
        float spiralSpeed = 0;

        DirectX::XMFLOAT3 orbitalVelocity = { 0,0,0 };
        float spiralstrong = 0;

        float veloRandScale = 0.0f;
        float speed = 0.0f;
        float emitStartSpeed = 1.0f;    //  エフェクトの再生開始時の速度
        float emitEndSpeed = 1.0f;      //  エフェクト再生後の最終的な速度

        DirectX::XMFLOAT2 scale = { 0.2f,0.2f };
        int scaleVariateByLife = 0;               // 大きさの変化の基準を管理 ( TRUE : 寿命によって変化 )
        float padding6 = 0;               // 大きさの変化の基準を管理 ( TRUE : 寿命によって変化 )

        float lifeStartSize = 1.0f;     // パーティクルの生成時の速度
        float lifeEndSize = 1.0f;       // パーティクルの消滅時の速度
        // -------------------  ↑↓ どちらかのみ  ----------------------
        float emitStartSize = 1.0f;     // エフェクトの再生開始時の速度
        float emitEndSize = 1.0f;       // エフェクト再生後の最終的な速度

        float radial = { 0 };
        float buoyancy = {};
        float emitStartGravity = 0.0f;
        float emitEndGravity = 0.0f;

        float strechscale = { 1.0f };    //ストレッチビルボードの強度(伸びる時の大きさ)
        DirectX::XMFLOAT3 padding = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    GPUparticleSaveConstants m_GSC;

    //保存するパラメータ
    struct SaveParameter
    {
        int m_blend = 2;
        int m_depthS = 2;
        std::string	m_textureName;

        //ここから別バージョン
        bool m_deleteflag = false;
        float deletetime = 0.0f;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameter m_p;

private:
    float emitTimer = 0.0f;
    bool stopFlg = false;

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
    float time = 0.0f;
};