#include "Scene.h"
#include "Graphics/Graphics.h"
#include "Camera/Camera.h"

//viewport�̐ݒ�
void Scene::ViewPortInitialize()
{
    Graphics& graphics = Graphics::Instance();

    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    graphics.GetDeviceContext()->OMSetRenderTargets(1, &rtv, dsv);
}

//�V�[���̃R���X�^���g�o�b�t�@�̏�����
void Scene::ConstantBufferInitialize()
{
    sc = std::make_unique<constant_buffer<SceneConstants>>(Graphics::Instance().GetDevice());
}

//�V�[���̃R���X�^���g�o�b�t�@�̍X�V
void Scene::ConstantBufferUpdate()
{
    //�J�����̏��������Ă���
    Camera& camera = Camera::Instance();

    Graphics& graphics = Graphics::Instance();

    //�r���[���v���W�F�N�V�����s��쐬
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

    //�J�����̏������Ă�����
    sc->data.view = view;
    sc->data.projection = projection;
    DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&sc->data.view);
    DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&sc->data.projection);
    DirectX::XMStoreFloat4x4(&sc->data.viewprojection, V * P);
    DirectX::XMStoreFloat4x4(&sc->data.inverseview, DirectX::XMMatrixInverse(nullptr, V));
    DirectX::XMStoreFloat4x4(&sc->data.inverseprojection, DirectX::XMMatrixInverse(nullptr, P));
    sc->data.cameraposition = camera.GetEye();

    //�X�V����
    sc->activate(Graphics::Instance().GetDeviceContext(), 10, true, true, true, true, true, true);
}