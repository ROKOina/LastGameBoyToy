#include "Scene.h"
#include "Graphics/Graphics.h"

#include "Components/CameraCom.h"
#include "Components/TransformCom.h"

//シーンのコンスタントバッファの初期化
void Scene::ConstantBufferInitialize()
{
    sc = std::make_unique<ConstantBuffer<SceneConstants>>(Graphics::Instance().GetDevice());
}

//シーンのコンスタントバッファの更新
void Scene::ConstantBufferUpdate()
{
    //カメラの情報を持ってくる
    std::shared_ptr<CameraCom> c = GameObjectManager::Instance().Find("camera")->GetComponent<CameraCom>();

    Graphics& graphics = Graphics::Instance();

    //カメラの情報を入れてあげる
    sc->data.view = c->GetView();
    sc->data.projection = c->GetProjection();
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&sc->data.view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&sc->data.projection);
    DirectX::XMStoreFloat4x4(&sc->data.viewprojection, V * P);
    DirectX::XMStoreFloat4x4(&sc->data.inverseview, DirectX::XMMatrixInverse(nullptr, V));
    DirectX::XMStoreFloat4x4(&sc->data.inverseprojection, DirectX::XMMatrixInverse(nullptr, P));
    sc->data.cameraposition = c->GetGameObject()->transform_->GetWorldPosition();

    //更新する
    sc->Activate(Graphics::Instance().GetDeviceContext(), 10, true, true, true, true, true, true);
}