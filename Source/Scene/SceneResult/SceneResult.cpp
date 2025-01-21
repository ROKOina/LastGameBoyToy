#include "SceneResult.h"
#include "Graphics\Graphics.h"
#include "Component\Light\LightCom.h"
#include "Component\Camera\FreeCameraCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Sprite\Sprite.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\UI\Font.h"

void SceneResult::Initialize()
{
    #pragma region 描画の基本系

    //ポストエフェクト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("posteffect");
        std::shared_ptr<PostEffect>posteffect = obj->AddComponent<PostEffect>();
    }

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->transform_->SetWorldPosition({ -2.394f, 0.644f, -2.916f });
        freeCamera->transform_->SetEulerRotation({ -12.959f,359.176f,0.0f });
        std::shared_ptr<FreeCameraCom> camera = freeCamera->AddComponent<FreeCameraCom>();
        camera->SetFocusPos({ -2.436f,1.322f,0.033f });
        camera->SetFocus({ -2.422f,1.092f,-0.967f });
        camera->SetEye({ -2.394f,0.644f,-2.916f });
        camera->SetDistance(3.026f);
        camera->SetUpdate(false);
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>("Data/SerializeData/LightData/title.light");
    }

#pragma endregion

    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("Canvas");

    MakeResultUI(obj);

    //タイトル
    {
        auto& title = obj->AddChildObject();
        title->SetName("title");
        title->AddComponent<Sprite>("Data/SerializeData/UIData/titleScene/title.ui", Sprite::SpriteShader::CHROMATICABERRATION, false);
    }

    //PVE
    {
        auto& next = obj->AddChildObject();
        next->SetName("PVE");
        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/PVE.ui", Sprite::SpriteShader::GLITCH, true);
    }
    //PVP
    {
        auto& next = obj->AddChildObject();
        next->SetName("PVP");
        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/PVP.ui", Sprite::SpriteShader::GLITCH, true);
    }
    //トレーニング
    {
        auto& next = obj->AddChildObject();
        next->SetName("Training");
        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/Training.ui", Sprite::SpriteShader::GLITCH, true);
    }
    //ゲーム終了
    {
        auto& next = obj->AddChildObject();
        next->SetName("endgame");
        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/endgame.ui", Sprite::SpriteShader::GLITCH, true);
    }

    //セレクト棒
    {
        auto& next = obj->AddChildObject();
        next->SetName("selectBow");
        next->AddComponent<Sprite>("Data/SerializeData/UIData/selectScene/selectBow.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();
}

void SceneResult::Update(float elapsedTime)
{
    GameObjectManager::Instance().Update(elapsedTime);
    GameObjectManager::Instance().UpdateTransform();
}

void SceneResult::Render(float elapsedTime)
{
    // 画面クリア＆レンダーターゲット設定
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    //コンスタントバッファの更新
    ConstantBufferUpdate(elapsedTime);

    //サンプラーステートの設定
    Graphics::Instance().SetSamplerState();

    //オブジェクト描画
    GameObjectManager::Instance().Render(sc->data.view, sc->data.projection, GameObjectManager::Instance().Find("directionallight")->GetComponent<Light>()->GetDirection());
}

//#define void MAKE_SHADOW_UI

void SceneResult::MakeResultUI(GameObj canvas)
{
    for (int i = 0; i < 4; ++i)
    {
        std::shared_ptr<GameObject> obj = canvas->AddChildObject();
        std::string name = std::to_string(i) + "st_Player_shadow";
        obj->SetName(name.c_str());
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->str = L"000";
        font->position = { 450.0f + 5,300.0f + (150 * i) + 2 };
        font->scale = 1.5f;
        font->color = { 0,0,0,1 };

        std::shared_ptr<GameObject> obj1 = canvas->AddChildObject();
        std::string name1 = std::to_string(i) + "st_Player";
        obj1->SetName(name1.c_str());
        std::shared_ptr<Font> font1 = obj1->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font1->str = L"000";
        font1->position = { 450.0f,300.0f + (150 * i)};
        font1->scale = 1.5f;
        font1->color = { 1,1,1,1 };
    }
}