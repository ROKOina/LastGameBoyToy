#pragma once
#include "Component/System/Component.h"
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

class Font :  public Component
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT3			position;
		DirectX::XMFLOAT4			color;
		DirectX::XMFLOAT4			mask;
		DirectX::XMFLOAT2			texcoord;
	};

	struct Header
	{
		int							fontWidth;
		int							fontHeight;
		int							textureWidth;
		int							textureHeight;
		int							textureNum;
		int							characterInfoNum;
	};
	struct CharacterInfo
	{
		static const WORD			NonCode = 0;			// �R�[�h�Ȃ�
		static const WORD			EndCode = 0xFFFF;		// �I���R�[�h
		static const WORD			ReturnCode = 0xFFFE;	// ���s�R�[�h
		static const WORD			TabCode = 0xFFFD;		// �^�u�R�[�h
		static const WORD			SpaceCode = 0xFFFC;		// �󔒃R�[�h

		float						left;					// �e�N�X�`�������W
		float						top;					// �e�N�X�`������W
		float						right;					// �e�N�X�`���E���W
		float						bottom;					// �e�N�X�`�������W
		float						xoffset;				// �I�t�Z�b�gX
		float						yoffset;				// �I�t�Z�b�gY
		float						xadvance;				// �v���|�[�V���i����
		float						width;					// �摜��
		float						height;					// �摜����
		int							page;					// �e�N�X�`���ԍ�
		int							channel;				// �`�����l��
		bool						ascii;					// �A�X�L�[����
	};

public:
	Font(const char* filename, int maxSpriteCount);
	~Font() {}

	// ���O�擾
	const char* GetName() const override { return "Font"; }

	// �J�n����
	void Start() override {};

	// �X�V����
	void Update(float elapsedTime) override {};
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	void OnGUI()override;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>						vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>						pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>						inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>							vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>							indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState>						blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>					rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>					depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>						samplerState;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	shaderResourceViews;

	float							fontWidth;
	float							fontHeight;
	int								textureCount;
	int								characterCount;
	std::vector<CharacterInfo>		characterInfos;
	std::vector<UINT16>				characterIndices;

	struct Subset
	{
		ID3D11ShaderResourceView* shaderResourceView;
		UINT						startIndex;
		UINT						indexCount;
	};
	std::vector<Subset>				subsets;
	Vertex* currentVertex = nullptr;
	UINT							currentIndexCount;
	int								currentPage;

	float							screenWidth;
	float							screenHeight;

public:
	DirectX::XMFLOAT2				position = { 0,0 };
	 DirectX::XMFLOAT2			    originalPos = {0,0};
	float							scale = 0;
   std::string			            str;
};
