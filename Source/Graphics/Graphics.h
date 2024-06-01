#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Shader.h"
#include "DebugRenderer/DebugRenderer.h"
#include "DebugRenderer/LineRenderer.h"

//�u�����h�X�e�[�g
enum class BLENDSTATE
{
    NONE,
    ALPHA,
    ADD,
    SUBTRACT,
    REPLACE,
    MULTIPLY,
    LIGHTEN,
    DARKEN,
    SCREEN,

    MAX
};

// ���X�^���C�U
enum class RASTERIZERSTATE
{
    SOLID_CULL_NONE,
    SOLID_CULL_BACK,
    SOLID_CULL_FRONT,
    WIREFRAME,

    MAX
};

// �[�x�X�e�[�g
enum class DEPTHSTATE
{
    NONE, // �[�x�e�X�g���Ȃ�
    ZT_ON_ZW_ON,    // �e�X�g�L�A�������ݗL
    ZT_ON_ZW_OFF,   // �e�X�g�L�A�������ݖ�
    ZT_OFF_ZW_ON,   // �e�X�g���A�������ݗL
    ZT_OFF_ZW_OFF,  // �e�X�g���A�������ݖ�

    // �ȍ~�X�e���V���}�X�N�p
    MASK,
    APPLY_MASK,
    EXCLUSIVE,

    MAX
};

// �T���v���[�X�e�[�g
enum SAMPLEMODE
{
    WRAP_POINT,
    WRAP_LINEAR,
    WRAP_ANISOTROPIC,

    BORDER_POINT,
    BORDER_LINEAR,
    BORDER_ANISOTROPIC,

    SHADOW,

    MAX
};

class ModelShader;

// �O���t�B�b�N�X
class Graphics
{
public:
    Graphics(HWND hWnd);
    ~Graphics();

    // �C���X�^���X�擾
    static Graphics& Instance() { return *instance; }

    // �f�o�C�X�擾
    ID3D11Device* GetDevice() const { return device.Get(); }

    // �f�o�C�X�R���e�L�X�g�擾
    ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

    // �X���b�v�`�F�[���擾
    IDXGISwapChain* GetSwapChain() const { return swapchain.Get(); }

    // �����_�[�^�[�Q�b�g�r���[�擾
    ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

    // �f�v�X�X�e���V���r���[�擾
    ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

    // �u�����h�X�e�[�g�̎擾
    ID3D11BlendState* GetBlendState(BLENDSTATE index) { return blendStates[static_cast<int>(index)].Get(); }

    // ���X�^���C�U�̎擾
    ID3D11RasterizerState* GetRasterizerState(RASTERIZERSTATE index) { return rasterizerStates[static_cast<int>(index)].Get(); }

    // �[�x�X�e�[�g �̎擾
    ID3D11DepthStencilState* GetDepthStencilState(DEPTHSTATE index) { return depthStencilStates[static_cast<int>(index)].Get(); }

    //�T���v���[�X�e�[�g�̐ݒ�
    void SetSamplerState();

    //�V�F�[�_�[�擾
    ModelShader* GetModelShader(int number)const { return modelshaders[number].get(); }

    // �X�N���[�����擾
    float GetScreenWidth() const { return screenWidth; }

    // �X�N���[�������擾
    float GetScreenHeight() const { return screenHeight; }

    // �f�o�b�O�����_���擾
    DebugRenderer* GetDebugRenderer() const { return debugRenderer.get(); }

    // ���C�������_���擾
    LineRenderer* GetLineRenderer() const { return lineRenderer.get(); }

private:
    static Graphics* instance;
    Microsoft::WRL::ComPtr<ID3D11Device>			device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates[static_cast<int>(BLENDSTATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerStates[static_cast<int>(RASTERIZERSTATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates[static_cast<int>(DEPTHSTATE::MAX)];
    Microsoft::WRL::ComPtr<ID3D11SamplerState>	    samplerStates[static_cast<int>(SAMPLEMODE::MAX)];

    std::unique_ptr<ModelShader>                    modelshaders[2];
    std::unique_ptr<DebugRenderer>					debugRenderer;
    std::unique_ptr<LineRenderer>					lineRenderer;

    float	screenWidth;
    float	screenHeight;
};
