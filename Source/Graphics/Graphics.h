#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include "Graphics/Shaders/Shader.h"
#include "Graphics/DebugRenderer/DebugRenderer.h"
#include "Graphics/DebugRenderer/LineRenderer.h"
#include "Graphics\Dx11StateLib.h"


#include "ThreadPool/ThreadPool.h"

#include <mutex>

#define DEBUG_GUI_ true

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


class CameraCom;
class ModelShader;

// �O���t�B�b�N�X
class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	// �C���X�^���X�擾
	static Graphics& Instance() { return *instance_; }

	// �f�o�C�X�擾
	ID3D11Device* GetDevice() const { return device_.Get(); }

	// �f�o�C�X�R���e�L�X�g�擾
	ID3D11DeviceContext* GetDeviceContext() const { return immediateContext_.Get(); }

	// �X���b�v�`�F�[���擾
	IDXGISwapChain* GetSwapChain() const { return swapchain_.Get(); }

	// �����_�[�^�[�Q�b�g�r���[�擾
	ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView_.Get(); }

	// �f�v�X�X�e���V���r���[�擾
	ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView_.Get(); }

	//�V�F�[�_�[�擾
	ModelShader* GetModelShader(int number)const { return modelshaders[number].get(); }

	// �X�N���[�����擾
	float GetScreenWidth() const { return screenWidth_; }

	// �X�N���[�������擾
	float GetScreenHeight() const { return screenHeight_; }

	// �f�o�b�O�����_���擾
	DebugRenderer* GetDebugRenderer() const { return debugRenderer_.get(); }

	// ���C�������_���擾
	LineRenderer* GetLineRenderer() const { return lineRenderer_.get(); }

	//�`�����ݒ�Ăяo��
	const std::unique_ptr<Dx11StateLib>& GetDx11State() { return dx11State_; }

	// �`��^�[�Q�b�g�̑ޔ�
	void CacheRenderTargets();

	// �`��^�[�Q�b�g��߂�
	void RestoreRenderTargets();


	// �u�����h�X�e�[�g�̎擾
	ID3D11BlendState* GetBlendState(BLENDSTATE index) { return blendStates[static_cast<int>(index)].Get(); }

	// ���X�^���C�U�̎擾
	ID3D11RasterizerState* GetRasterizerState(RASTERIZERSTATE index) { return rasterizerStates[static_cast<int>(index)].Get(); }

	// �[�x�X�e�[�g �̎擾
	ID3D11DepthStencilState* GetDepthStencilState(DEPTHSTATE index) { return depthStencilStates[static_cast<int>(index)].Get(); }

	//�T���v���[�X�e�[�g�̐ݒ�
	void SetSamplerState();


	//���[���h���W����X�N���[�����W�ɂ���
	DirectX::XMFLOAT3 WorldToScreenPos(DirectX::XMFLOAT3 worldPos, std::shared_ptr<CameraCom> camera);

	//FPS(�Z�b�g�̓t���[�����[�N�ł����g��Ȃ��\��)
	void SetFPSFramework(float fps) { this->fps_ = fps; }
	float GetFPS() { return fps_; }

	//�~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex_; }

	//�f�o�b�O�\�����邩
	bool IsDebugGUI() { return isDebugGui_; }

	//�X���b�h�v�[���擾
	ThreadPool* GetThreadPool() { return threadPool_.get(); }

	//�n���h���Q�b�g
	HWND GetHwnd() { return hWnd_; }


private:
	static Graphics* instance_;

	Microsoft::WRL::ComPtr<ID3D11Device>			device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext_;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapchain_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;

	std::unique_ptr<DebugRenderer>					debugRenderer_;
	std::unique_ptr<LineRenderer>					lineRenderer_;

	//�`�����ꊇ������
	std::unique_ptr<Dx11StateLib> dx11State_;

	//�`��^�[�Q�b�g���p
	UINT			cachedViewportCount_{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT	cachedViewports_[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView_;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates[static_cast<int>(BLENDSTATE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerStates[static_cast<int>(RASTERIZERSTATE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilStates[static_cast<int>(DEPTHSTATE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11SamplerState>	    samplerStates[static_cast<int>(SAMPLEMODE::MAX)];

	std::unique_ptr<ModelShader>                    modelshaders[2];


private:
	float	screenWidth_;
	float	screenHeight_;

	float fps_;
	std::mutex	mutex_;

	//�f�o�b�O�\��
	bool isDebugGui_ = DEBUG_GUI_;

	//�X���b�h�v�[��
	std::unique_ptr<ThreadPool> threadPool_;

	const HWND				hWnd_;

	//���E�̑��x
public:
	float GetWorldSpeed() { return worldSpeed_; }
	void SetWorldSpeed(float speed) { worldSpeed_ = speed; }

private:
	float worldSpeed_ = 1.0f;
};

