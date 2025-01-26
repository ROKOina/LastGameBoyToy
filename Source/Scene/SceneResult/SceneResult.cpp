#include "SceneResult.h"
#include "Graphics\Graphics.h"
#include "Component\Light\LightCom.h"
#include "Component\Camera\FreeCameraCom.h"
#include "Component\System\TransformCom.h"
#include "Component\Sprite\Sprite.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\UI\Font.h"
#include "Component\Renderer\RendererCom.h"
#include "Input\Input.h"
#include "DeliveryResultData.h"
#include <Component\UI\PlayerUI.h>
#include <Component\Animation\AnimationCom.h>

//コンストラクタ
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
        obj->AddComponent<Light>("Data/SerializeData/LightData/result.light");
    }

#pragma endregion

    //データ受け渡し
    for (int i = 0; i < 4; ++i) {
        resultDatas[i] = DelivertResultData::Instance().GetResultData(i);
    }
    isMyWin = DelivertResultData::Instance().GetIsMyWin();

    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("Canvas");

    //オブジェクト設定
    MakeResultUI(obj);
    MakeResultModel();

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //暗転からはじまるように
    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
    auto& post = GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>();
    post->SetExposureZero();    //暗転
    post->SetParameter(1.4f, 1.0f, parameters); //明転
}

//終了処理
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

//更新処理
void SceneResult::Update(float elapsedTime)
{
    //スペースキーでリザルト開始（デバッグ用)
    {
        GamePad& gamepad = Input::Instance().GetGamePad();
        auto& canvas = GameObjectManager::Instance().Find("Canvas");
        if (GamePad::BTN_A & gamepad.GetButtonDown())
        {
            //リザルトのUI君
            for (int i = 0; i < 4; ++i)
            {
                std::string canvasName = "Player" + std::to_string(i) + "_UICanvas";
                resultUI[i]->GetChildFind((canvasName + "_Frame").c_str())->GetComponent<Sprite>()->EasingPlay();
                std::string killNumName = std::to_string(i) + "st_PlayerKillNum";
                resultUI[i]->GetChildFind(killNumName.c_str())->GetComponent<Sprite>()->EasingPlay();
                std::string deathNumName = std::to_string(i) + "st_PlayerDeathNum";
                resultUI[i]->GetChildFind(deathNumName.c_str())->GetComponent<Sprite>()->EasingPlay();
                std::string icon = std::to_string(i) + "st_PlayerIcon";
                resultUI[i]->GetChildFind(icon.c_str())->GetComponent<Sprite>()->EasingPlay();
            }

            //勝敗君
            canvas->GetChildFind("Judge")->GetComponent<Sprite>()->EasingPlay();
        }
        if (GamePad::NAKA_BUTTON & gamepad.GetButtonDown())
        {
            //リザルトのUI君
            for (int i = 0; i < 4; ++i)
            {
                std::string canvasName = "Player" + std::to_string(i) + "_UICanvas";
                resultUI[i]->GetChildFind((canvasName + "_Frame").c_str())->GetComponent<Sprite>()->StopEasing();
                std::string killNumName = std::to_string(i) + "st_PlayerKillNum";
                resultUI[i]->GetChildFind(killNumName.c_str())->GetComponent<Sprite>()->StopEasing();
                std::string deathNumName = std::to_string(i) + "st_PlayerDeathNum";
                resultUI[i]->GetChildFind(deathNumName.c_str())->GetComponent<Sprite>()->StopEasing();
                std::string icon = std::to_string(i) + "st_PlayerIcon";
                resultUI[i]->GetChildFind(icon.c_str())->GetComponent<Sprite>()->StopEasing();
            }

            //勝敗君
            canvas->GetChildFind("Judge")->GetComponent<Sprite>()->StopEasing();
        }
    }

    //行列更新や更新処理
    GameObjectManager::Instance().Update(elapsedTime);
    GameObjectManager::Instance().UpdateTransform();
}

//描画処理
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

//UI関係をここで生成
void SceneResult::MakeResultUI(GameObj canvas)
{
    //複製したいものを増やしていく
    for (int i = 0; i < 4; ++i)
    {
        float uiOffset = 0;
        if (i >= 2) uiOffset = 60;

        // 子オブジェクト (UI Canvas) を生成し名前を付ける
        GameObj uiCanvas = canvas->AddChildObject();
        std::string canvasName = "Player" + std::to_string(i) + "_UICanvas";
        uiCanvas->SetName(canvasName.c_str());
        uiCanvas->transform_->SetWorldPosition({ -400.0f, 395.0f + (160 * i) + uiOffset, 0.0f });
        resultUI[i] = uiCanvas;

        // フレーム
        GameObj frameUiObj = uiCanvas->AddChildObject();
        frameUiObj->SetName((canvasName + "_Frame").c_str());
        Sprite* frameUiSprite = frameUiObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_flame.ui", Sprite::SpriteShader::DEFALT, false).get();
        frameUiSprite->SetIsParentMove(true);
        frameUiSprite->SetEasingPosition({ 400.0f,395.0f + (160 * i) + uiOffset });
        //敵チームは少し色を変える
        if (i >= 2)
            frameUiSprite->spc.easingcolor = { 1,45.0f / 255.0f,45.0f / 255.0f,1 };

        // キル数
        GameObj killNumObj = uiCanvas->AddChildObject();
        std::string killNumName = std::to_string(i) + "st_PlayerKillNum";
        killNumObj->SetName(killNumName.c_str());
        Sprite* killNumSpr = killNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_Kill_number.ui", Sprite::SpriteShader::DEFALT, false).get();
        killNumSpr->SetIsParentMove(true);
        killNumSpr->SetEasingPosition({ 650.0f,395.0f + (160 * i) + uiOffset });
        //キル数分スクロール
        float kill = float(resultDatas[i].killNum) * 0.1f;
        killNumSpr->numUVScroll.x = kill;


        // デス数
        GameObj deathNumObj = uiCanvas->AddChildObject();
        std::string deathNumName = std::to_string(i) + "st_PlayerDeathNum";
        deathNumObj->SetName(deathNumName.c_str());
        Sprite* deathNumSpr = deathNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_Death_number.ui", Sprite::SpriteShader::DEFALT, false).get();
        deathNumSpr->SetIsParentMove(true);
        deathNumSpr->SetEasingPosition({ 766.0f,395.0f + (160 * i) + uiOffset });
        //デス数分スクロール
        float death = float(resultDatas[i].deathNum) * 0.1f;
        deathNumSpr->numUVScroll.x = death;

        // キャラアイコン
        GameObj IconObj = uiCanvas->AddChildObject();
        std::string IconName = std::to_string(i) + "st_PlayerIcon";
        IconObj->SetName(IconName.c_str());

        UiSystem* IconSprite = IconObj->AddComponent<UiSystem>("Data/SerializeData/UIData/resultScene/Result_CharaIcon.ui", Sprite::SpriteShader::DEFALT, false).get();
        IconSprite->SetIsParentMove(true);
        IconSprite->SetEasingPosition({ 93.0f,380.0f + (160 * i) + uiOffset });

        switch (resultDatas[i].charaID)
        {
        case 0:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/InazawaCharacter.png");
            break;
        case 1:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/FarahCom.png");
            break;
        case 2:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/JankratCharacter.png");
            break;
        case 3:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/SoldierCom.png");
            break;
        }


    }

    // 勝敗表示
    GameObj judgeObj = canvas->AddChildObject();
    judgeObj->SetName("Judge");
    Sprite* judgeSpr = nullptr;
    isMyWin ?
        judgeSpr = judgeObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Judge_victory.ui", Sprite::SpriteShader::DEFALT, false).get() :
        judgeSpr = judgeObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Judge_defeat.ui", Sprite::SpriteShader::DEFALT, false).get();
}

//ここでModel関係を作成
void SceneResult::MakeResultModel()
{
    int winCharaID[2] = { 0,0 };
    int c = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (resultDatas[i].isWin)
        {
            winCharaID[c] = resultDatas[i].charaID;
            c++;
            if (c > 1)break;
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        // 勝利したキャラを出す
        GameObj winChara = GameObjectManager::Instance().Create();

        // キャラの位置をずらす
        float offsetX = static_cast<float>(i) * 1.5f; // X方向に1.5ずつずらす
        winChara->transform_->SetWorldPosition({ 1.0f + offsetX, 1.9f, 0.0f });
        winChara->transform_->SetScale({ 0.13f, 0.13f, 0.13f });
        std::string charaName = "winChara" + std::to_string(i + 1);
        winChara->SetName(charaName.c_str());
        auto& winCharaRender = winChara->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);

        switch (winCharaID[i])
        {
        case 0:
            winCharaRender->LoadModel("Data/Model/player_True/player1.mdl");
            break;
        case 1:
            winCharaRender->LoadModel("Data/Model/player_True/player2.mdl");
            break;
        case 2:
            winCharaRender->LoadModel("Data/Model/player_True/player3.mdl");
            break;
        case 3:
            winCharaRender->LoadModel("Data/Model/player_True/player4.mdl");
            break;
        }

        std::shared_ptr<AnimationCom>anim = winChara->AddComponent<AnimationCom>();
        anim->PlayAnimation(5, true);

        // 結果モデルに格納
        resultModel[i] = winChara;
    }
}