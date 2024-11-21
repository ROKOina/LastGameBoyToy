#include "SetNodeWorldPosCom.h"

#include "Component/Renderer/RendererCom.h"
#include "Component/System/TransformCom.h"

void SetNodeWorldPosCom::Start()
{
    //腕オブジェ保存
    obj = GameObjectManager::Instance().Find("armChild");
}

void SetNodeWorldPosCom::Update(float elapsedTime)
{
    const auto& model = obj.lock()->GetComponent<RendererCom>()->GetModel();
    const auto& node = model->FindNode("gun2");

    DirectX::XMFLOAT3 gunPos = { node->worldTransform._41,node->worldTransform._42,node->worldTransform._43 };

    GetGameObject()->transform_->SetWorldPosition(gunPos);
}

void SetNodeWorldPosCom::OnGUI()
{
}
