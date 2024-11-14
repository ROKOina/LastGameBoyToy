#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Scene/SceneManager.h"

//������
void SceneLoading::Initialize()
{
    //�X���b�h�J�n
    thread_ = new std::thread(LoadingThread, this);
}

//�I����
void SceneLoading::Finalize()
{
    //�X���b�h�I����
    if (thread_ != nullptr)
    {
        thread_->detach();
        delete thread_;
        thread_ = nullptr;
    }
}

//�X�V����
void SceneLoading::Update(float elapsedTime)
{
    //���̃V�[���̏���������������V�[����؂�ւ���
    if (nextScene_->IsReady())
    {
        SceneManager::Instance().ChangeScene(nextScene_);
    }
}

//�`�揈��
void SceneLoading::Render(float elapsedTime)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    //��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };    //RGBA(0.0~1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //3D�`��
    {
        // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
        rtv = {};
        dsv = {};
        rtv = Graphics::Instance().GetRenderTargetView();
        dsv = Graphics::Instance().GetDepthStencilView();
        dc->ClearRenderTargetView(rtv, color);
        dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        dc->OMSetRenderTargets(1, &rtv, dsv);

        // �r���[�|�[�g�̐ݒ�
        D3D11_VIEWPORT	vp = {};
        vp.Width = static_cast<float>(Graphics::Instance().GetScreenWidth());
        vp.Height = static_cast<float>(Graphics::Instance().GetScreenHeight());
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        dc->RSSetViewports(1, &vp);
    }
}

//���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    //COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
    CoInitialize(nullptr);

    //���̃V�[���̏��������s��
    scene->nextScene_->Initialize();

    //�X���b�h���I���O��COM�֘A�̏I����
    CoUninitialize();

    //���̃V�[���̏��������ݒ�
    scene->nextScene_->SetReady();
}