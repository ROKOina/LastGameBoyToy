#include "RendererCom.h"

#include "TransformCom.h"
#include <imgui.h>
#include <string>

// 開始処理
void RendererCom::Start()
{
	
}

// 更新処理
void RendererCom::Update(float elapsedTime)
{
	// モデルの更新
	if (model_ != nullptr)
	{
		DirectX::XMFLOAT4X4 transform = GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform();
		model_->UpdateTransform(DirectX::XMLoadFloat4x4(&transform));
	}
}

// GUI描画
void RendererCom::OnGUI()
{

}

// モデルの読み込み
void RendererCom::LoadModel(const char* filename)
{
	ID3D11Device* device = Graphics::Instance().GetDevice();
	std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();
	m->Load(device, filename);

	model_ = std::make_unique<Model>(m);
}
