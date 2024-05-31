#include "RendererCom.h"

#include "TransformCom.h"
#include <imgui.h>
#include <string>

// �J�n����
void RendererCom::Start()
{
	
}

// �X�V����
void RendererCom::Update(float elapsedTime)
{
	// ���f���̍X�V
	if (model_ != nullptr)
	{
		DirectX::XMFLOAT4X4 transform = GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform();
		model_->UpdateTransform(DirectX::XMLoadFloat4x4(&transform));
	}
}

// GUI�`��
void RendererCom::OnGUI()
{

}

// ���f���̓ǂݍ���
void RendererCom::LoadModel(const char* filename)
{
	//model_ = std::make_unique<Model>(filename);
}
