#include "Graphics/Graphics.h"
#include "Input\Input.h"
#include "Input\GamePad.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneLoading/SceneLoading.h"
#include "imgui.h"
#include "Component\System\GameObject.h"
#include "Component\Renderer\RendererCom.h"
#include "Component/System/TransformCom.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Animation\AnimationCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include "Component\Collsion\RayCollisionCom.h"
#include "Component/Camera/FreeCameraCom.h"
#include "SceneResult.h"
#include "Scene\SceneTitle\SceneTitle.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Light\LightCom.h"

void SceneResult::Initialize()
{
    Graphics& graphics = Graphics::Instance();

    //�|�X�g�G�t�F�N�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        obj->AddComponent<PostEffect>();
    }

    //���C�g
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>(nullptr);
    }

    //�t���[�J����
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    //�L�����o�X
    {
        auto& obj = GameObjectManager::Instance().Create();
        obj->SetName("Canvas");

        {
            auto& modoru = obj->AddChildObject();
            modoru->SetName("modoru");
            modoru->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/modoru.ui", Sprite::SpriteShader::DEFALT, true);
        }
    }
}

void SceneResult::Finalize()
{
}

void SceneResult::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    UIUpdate(elapsedTime);

    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
}

void SceneResult::Render(float elapsedTime)
{
    // ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //�R���X�^���g�o�b�t�@�̍X�V
    ConstantBufferUpdate(elapsedTime);

    //�T���v���[�X�e�[�g�̐ݒ�
    Graphics::Instance().SetSamplerState();

    //�I�u�W�F�N�g�`��
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());
}

void SceneResult::UIUpdate(float elapsedTime)
{
    auto& canvas = GameObjectManager::Instance().Find("Canvas");
    if (!canvas)return;

    //�߂�
    {
        auto& modoru = canvas->GetChildFind("modoru");
        auto& sprite = modoru->GetComponent<Sprite>();
        if (sprite->GetHitSprite())
        {
            GamePad& gamePad = Input::Instance().GetGamePad();
            if (GamePad::BTN_RIGHT_TRIGGER & gamePad.GetButtonDown())
            {
                SceneManager::Instance().ChangeScene(new SceneTitle);
            }
        }
    }
}