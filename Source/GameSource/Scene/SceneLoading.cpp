#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"

//������
void SceneLoading::Initialize()
{
    //�X�v���C�g������
    sprite_ = new Sprite("Data/Sprite/yellow.png");
    
    //�X���b�h�J�n
    thread_ = new std::thread(LoadingThread,this);

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

    //�X�v���C�g�I����
    if (sprite_ != nullptr)
    {
        delete sprite_;
        sprite_ = nullptr;
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
        //�o�b�t�@���
        Graphics::Instance().CacheRenderTargets();

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

        //2D�X�v���C�g�`��
        {
            //��ʉ��Ƀ��[�ŃC���O�A�C�R����`��
            float screenWidth = static_cast<float>(graphics.GetScreenWidth());
            float screenHeight = static_cast<float>(graphics.GetScreenHeight());
            float textureWidth = static_cast<float>(sprite_->GetTextureWidth());
            float textureHeight = static_cast<float>(sprite_->GetTextureHeight());
            float positionX = screenWidth - textureWidth;
            float positionY = screenHeight - textureHeight;

            sprite_->Render(dc,
                0, 0, screenWidth, screenHeight,
                0, 0, textureWidth, textureHeight,
                0,
                1, 1, 1, 1);
        }

        //�o�b�t�@�߂�
        graphics.RestoreRenderTargets();
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