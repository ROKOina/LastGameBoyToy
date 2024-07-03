#include "Scene.h"
#include "Graphics/Graphics.h"

#include "Components/CameraCom.h"
#include "Components/TransformCom.h"
#include "GameSource/Scene/SceneManager.h"

//シーンのコンスタントバッファの初期化
void Scene::ConstantBufferInitialize()
{
    sc = std::make_unique<ConstantBuffer<SceneConstants>>(Graphics::Instance().GetDevice());
}

//シーンのコンスタントバッファの更新
void Scene::ConstantBufferUpdate(float elapsedTime)
{
    //カメラの情報を持ってくる
    std::shared_ptr<CameraCom> c = SceneManager::Instance().GetActiveCamera()->GetComponent<CameraCom>();

    Graphics& graphics = Graphics::Instance();

    //コンスタントバッファに入れる時間の更新
    sc->data.time += elapsedTime;
    sc->data.deltatime = elapsedTime;

    //カメラの情報を入れてあげる
    sc->data.view = c->GetView();
    sc->data.projection = c->GetProjection();
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&sc->data.view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&sc->data.projection);
    DirectX::XMStoreFloat4x4(&sc->data.viewprojection, V * P);
    DirectX::XMStoreFloat4x4(&sc->data.inverseview, DirectX::XMMatrixInverse(nullptr, V));
    DirectX::XMStoreFloat4x4(&sc->data.inverseprojection, DirectX::XMMatrixInverse(nullptr, P));
    sc->data.cameraposition = c->GetGameObject()->transform_->GetWorldPosition();
    sc->data.cameraScope = c->GetScope();
    sc->data.screenResolution.x = Graphics::Instance().GetScreenWidth();
    sc->data.screenResolution.y = Graphics::Instance().GetScreenHeight();

    //更新する
    sc->Activate(Graphics::Instance().GetDeviceContext(), (int)CB_INDEX::SCENE, true, true, true, true, true, true);
}