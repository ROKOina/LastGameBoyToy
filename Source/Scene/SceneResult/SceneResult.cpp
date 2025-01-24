#include "SceneResult.h"
#include "Graphics\Graphics.h"
#include "Component\Light\LightCom.h"
#include "Component\Camera\FreeCameraCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Sprite\Sprite.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\UI\Font.h"
#include "Component\UI\UiMove.h"
#include "Component\Renderer\RendererCom.h"
#include "Input\Input.h"

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
        freeCamera->transform_->SetWorldPosition({ 0.275f, -1.563f, -7.571f });
        freeCamera->transform_->SetEulerRotation({ -12.959,359.176f,0.0f });
        std::shared_ptr<FreeCameraCom> camera = freeCamera->AddComponent<FreeCameraCom>();
        camera->SetFocusPos({ 0.363f,-1.042f,-5.307f });
        camera->SetFocus({ 0.372f,-1.192f,-5.967f });
        camera->SetEye({ 0.4f,-1.644f,-7.916f });
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
    MakeResultModel();

    //コンスタントバッファの初期化
    ConstantBufferInitialize();
}

void SceneResult::Finalize()
{
    for (int i = 0; i < 4; ++i)
    {
        resultUI[i].reset();
    }
    for (int i = 0; i < 2; ++i)
    {
        resultModel->reset();
    }
}

void SceneResult::Update(float elapsedTime)
{
    UiMove* modelMove = resultModel[0]->GetComponent<UiMove>().get();
    static bool event1 = true;
    static bool event2 = true;

    //スペースキーでリザルト開始（デバッグ用
    GamePad& gamepad = Input::Instance().GetGamePad();
    if (GamePad::BTN_A & gamepad.GetButtonDown())
    {
        event1 = true;
        event2 = true;

        for (int i = 0; i < 4; ++i)
        {
            resultUI[i]->GetComponent<UiMove>()->SlideAllReset();
        }

        modelMove->SlideStart(UiMove::SlideType::Pos,true, 2.0f);
        modelMove->SlideStart(UiMove::SlideType::Rote,true, 2.0f);
    }

    //モデルのラープが半分になると敗者チームのUIを出す
    if (modelMove->GetSlideProgress(UiMove::SlideType::Pos) > 0.3f && event1)
    {
        UiMove* uiMove1 = resultUI[2]->GetComponent<UiMove>().get();
        uiMove1->SlideStart(UiMove::SlideType::Pos, true, 0.3f);
        UiMove* uiMove2 = resultUI[3]->GetComponent<UiMove>().get();
        uiMove2->SlideStart(UiMove::SlideType::Pos, true, 0.3f);

        event1 = false;
    }

    //モデルのラープが最後になると勝利チームのUIを出す
    if (modelMove->GetSlideProgress(UiMove::SlideType::Pos) > 0.95f && event2)
    {
        UiMove* uiMove1 = resultUI[0]->GetComponent<UiMove>().get();
        uiMove1->SlideStart(UiMove::SlideType::Pos, true, 0.1f);
        UiMove* uiMove2 = resultUI[1]->GetComponent<UiMove>().get();
        uiMove2->SlideStart(UiMove::SlideType::Pos, true, 0.1f);

        event2 = false;
    }

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

#define MAKE_FONT_SHADOW(parentFont, offsetX,offsetY) \
        GameObj fontObj = parentFont->GetGameObject()->AddChildObject();\
        std::string name = parentFont->GetGameObject()->GetName();\
        fontObj->SetName(name.c_str());\
        name += "_shadow";\
        parentFont->GetGameObject()->SetName(name.c_str()); \
        Font* fontCom = fontObj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024).get();\
        fontCom->str = parentFont->str; \
        fontCom->parentPosOffset = { -offsetX, -offsetY }; \
        fontCom->scale = parentFont->scale; \
        fontCom->color = { 0,0,0,1 }; \
        fontCom->isParentMove = true; \
        parentFont->color = { 1,1,1,1 };

void SceneResult::MakeResultUI(GameObj canvas)
{
    

    for (int i = 0; i < 4; ++i)
    {
        float uiOffset = 0;
        if (i >= 2) uiOffset = 60;

        GameObj uiCanvas = GameObjectManager::Instance().Create();
        uiCanvas->transform_->SetWorldPosition({ -400.0f,395.0f + (160 * i) + uiOffset,0.0f });
        UiMove* uiMove = uiCanvas->AddComponent<UiMove>().get();
        DirectX::XMFLOAT3 uiMovePos = uiCanvas->transform_->GetWorldPosition();
        uiMove->SetOriginValue(UiMove::SlideType::Pos,uiMovePos);
        uiMove->SetTargetValue(UiMove::SlideType::Pos,{ 480.0f, uiMovePos.y, uiMovePos.z });
        resultUI[i] = uiCanvas;

        //フレーム
        GameObj frameUiObj = uiCanvas->AddChildObject();
        Sprite* frameUiSprite = frameUiObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_flame.ui", Sprite::SpriteShader::DEFALT, false).get();
        frameUiSprite->SetIsParentMove(true);

        //キル数
        std::shared_ptr<GameObject> killNumObj = uiCanvas->AddChildObject();
        std::string killNum = std::to_string(i) + "st_PlayerKillNum";
        killNumObj->SetName(killNum.c_str());
        Sprite* killNumSpr = killNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_number.ui", Sprite::SpriteShader::DEFALT, false).get();
        killNumSpr->SetIsParentMove(true);
        killNumSpr->SetParentPosOffset({ 305,-10,0 });

        //デス数
        std::shared_ptr<GameObject> deathNumObj = uiCanvas->AddChildObject();
        std::string deathNum = std::to_string(i) + "st_PlayerDeathNum";
        deathNumObj->SetName(killNum.c_str());
        Sprite* deathNumSpr = deathNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_number.ui", Sprite::SpriteShader::DEFALT, false).get();
        deathNumSpr->SetIsParentMove(true);
        deathNumSpr->SetParentPosOffset({ 435,-10,0 });
    }

    //勝敗表示
    GameObj judgeObj = GameObjectManager::Instance().Create();
    Sprite* judgeSpr = nullptr;
    resultDatas[0].isWin ?
        judgeSpr = judgeObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Judge_victory.ui", Sprite::SpriteShader::DEFALT, false).get() :
        judgeSpr = judgeObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Judge_defeat.ui", Sprite::SpriteShader::DEFALT, false).get();
}

void SceneResult::MakeResultModel()
{
    //勝利したキャラを出す
    GameObj winChara = GameObjectManager::Instance().Create();
    winChara->transform_->SetWorldPosition({ 1.0f,1.9f,-5.1f });
    winChara->transform_->SetScale({ 0.13f,0.13f,0.13f });
    auto& winCharaRender = winChara->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    winCharaRender->LoadModel("Data/Model/player_True/player1.mdl");
    UiMove* winCharaMove = winChara->AddComponent<UiMove>().get();
    winCharaMove->SetOriginValue(UiMove::SlideType::Pos,{ 1.0f,-4.0f,-5.1f });
    winCharaMove->SetTargetValue(UiMove::SlideType::Pos,{ 1.0f,-1.9f,-5.1f });
    winCharaMove->SetOriginValue(UiMove::SlideType::Rote,{ 0.0f,0.0f,0.0f });
    winCharaMove->SetTargetValue(UiMove::SlideType::Rote,{ 0.0f,560.0f,0.0f });

    resultModel[0] = winChara;
}
