#include "Scene.h"
#include "Graphics/Graphics.h"
#include "Camera/Camera.h"

//viewportの設定
void Scene::ViewPortInitialize()
{
    Graphics& graphics = Graphics::Instance();

    // 画面クリア＆レンダーターゲット設定
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    graphics.GetDeviceContext()->OMSetRenderTargets(1, &rtv, dsv);
}

//シーンのコンスタントバッファの初期化
void Scene::ConstantBufferInitialize()
{
    sc = std::make_unique<constant_buffer<SceneConstants>>(Graphics::Instance().GetDevice());
}

//シーンのコンスタントバッファの更新
void Scene::ConstantBufferUpdate()
{
    //カメラの情報を持ってくる
    Camera& camera = Camera::Instance();

    Graphics& graphics = Graphics::Instance();

    //ビュー＆プロジェクション行列作成
    float screenWidth = graphics.GetScreenWidth();
    float screenHeight = graphics.GetScreenHeight();
    float fovY = DirectX::XMConvertToRadians(30.0f);
    float aspect = screenWidth / screenHeight;
    float nearZ = 1.0f;
    float farZ = 10000.0f;
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&camera.GetEye());
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&camera.GetFocus());
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&camera.GetUp());
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    DirectX::XMFLOAT4X4 view, projection;
    DirectX::XMStoreFloat4x4(&view, View);
    DirectX::XMStoreFloat4x4(&projection, Projection);

    //カメラの情報を入れてあげる
    sc->data.view = view;
    sc->data.projection = projection;
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&sc->data.view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&sc->data.projection);
    DirectX::XMStoreFloat4x4(&sc->data.viewprojection, V * P);
    DirectX::XMStoreFloat4x4(&sc->data.inverseview, DirectX::XMMatrixInverse(nullptr, V));
    DirectX::XMStoreFloat4x4(&sc->data.inverseprojection, DirectX::XMMatrixInverse(nullptr, P));
    sc->data.cameraposition = camera.GetEye();

    //更新する
    sc->activate(Graphics::Instance().GetDeviceContext(), 10, true, true, true, true, true, true);
}