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
#include "Component\Particle\GPUParticle.h"
#include "Component\MoveSystem\EasingMoveCom.h"
#include <Component\Camera\EventCameraCom.h>
#include <Component\Camera\EventCameraManager.h>
#include "Scene\SceneTitle\SceneTitle.h"
#include "SystemStruct\TimeManager.h"

std::wstring stringconvert(const std::string& str)
{
    std::wstring result;
    size_t i = 0;
    while (i < str.size()) {
        unsigned char c = str[i];
        if (c <= 0x7F) {
            result.push_back(c);
            ++i;
        }
        else if ((c & 0xE0) == 0xC0) {
            wchar_t wc = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
            result.push_back(wc);
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            wchar_t wc = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
            result.push_back(wc);
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0) {
            wchar_t wc = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) | ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
            result.push_back(wc);
            i += 4;
        }
        else {
            // 不正なUTF-8データを無視する
            ++i;
        }
    }
    return result;
}

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

    //ライト
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("directionallight");
        obj->AddComponent<Light>("Data/SerializeData/LightData/result.light");
    }

    //フリーカメラ
    {
        std::shared_ptr<GameObject> freeCamera = GameObjectManager::Instance().Create();
        freeCamera->SetName("freecamera");
        freeCamera->AddComponent<FreeCameraCom>();
        freeCamera->transform_->SetWorldPosition({ 0, 5, -10 });
    }
    GameObjectManager::Instance().Find("freecamera")->GetComponent<CameraCom>()->ActiveCameraChange();

#pragma endregion

    //データ受け渡し
    for (int i = 0; i < 4; ++i)
    {
        resultDatas[i] = DelivertResultData::Instance().GetResultData(i);
    }
    isMyWin = DelivertResultData::Instance().GetIsMyWin();

    auto& obj = GameObjectManager::Instance().Create();
    obj->SetName("Canvas");

    //オブジェクト設定
    MakeResultUI(obj);
    MakeResultModel();

    TimeManager::Instance().SetTimeEffect(1.0f, 0.0f);

    //イベント用カメラ
    {
        std::shared_ptr<GameObject> eventCamera = GameObjectManager::Instance().Create();
        eventCamera->SetName("eventcamera");
        std::shared_ptr<EventCameraCom>e = eventCamera->AddComponent<EventCameraCom>();
        e->ActiveCameraChange();
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/result_firstcamera.eventcamera");
    }

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //暗転からはじまるように
    std::vector<PostEffect::PostEffectParameter> parameters = { PostEffect::PostEffectParameter::Exposure };
    auto& post = GameObjectManager::Instance().Find("posteffect")->GetComponent<PostEffect>();
    post->SetExposureZero();    //暗転
    post->SetParameter(1.4f, 1.0f, parameters); //明転

    // BGM
    {
        if (isMyWin)
            Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::SCENE_RESULT_VICTORY, 1.0f, true);
        else
            Audio2DMagaer::Instance().Audio2DPlay(AUDIOID2D::SCENE_RESULT_DEFEAT, 1.0f, true);
    }
}

//終了処理
void SceneResult::Finalize()
{
    for (int i = 0; i < std::size(resultUI); ++i)
    {
        if (resultUI[i].lock())
        {
            resultUI[i].reset();
        }
    }

    for (int i = 0; i < std::size(resultModel); ++i)
    {
        if (resultModel[i].lock())
        {
            resultModel[i].reset();
        }
    }

    Audio2DMagaer::Instance().Audio2DStopAll();
}

//更新処理
void SceneResult::Update(float elapsedTime)
{
    //イベントカメラ用
    EventCameraManager::Instance().EventUpdate(elapsedTime);

    //イベントカメラ
    EventCamera(elapsedTime);

    //行列更新や更新処理
    GameObjectManager::Instance().UpdateTransform();
    GameObjectManager::Instance().Update(elapsedTime);
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

    //イベントカメラ用
    EventCameraManager::Instance().EventCameraImGui();
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
        std::shared_ptr<GameObject> uiCanvas = canvas->AddChildObject();
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
        if (i >= 2)frameUiSprite->spc.easingcolor = { 1,45.0f / 255.0f,45.0f / 255.0f,1 };

        // キル数
        {   //一桁目
            GameObj killNumObj = uiCanvas->AddChildObject();
            std::string killNumName = std::to_string(i) + "st_PlayerKillNum1";
            killNumObj->SetName(killNumName.c_str());
            Sprite* killNumSpr = killNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_Kill_number.ui", Sprite::SpriteShader::DEFALTUV, false).get();
            killNumSpr->SetIsParentMove(true);
            killNumSpr->SetEasingPosition({ 650.0f,395.0f + (160 * i) + uiOffset });
            //キル数分スクロール
            float kill = float(resultDatas[i].killNum % 10) * 0.1f;
            killNumSpr->numUVScroll.x = kill;
        }
        {   //二桁目
            GameObj killNumObj = uiCanvas->AddChildObject();
            std::string killNumName = std::to_string(i) + "st_PlayerKillNum2";
            killNumObj->SetName(killNumName.c_str());
            Sprite* killNumSpr = killNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_Kill_number.ui", Sprite::SpriteShader::DEFALTUV, false).get();
            killNumSpr->SetIsParentMove(true);
            killNumSpr->SetEasingPosition({ 619.0f,395.0f + (160 * i) + uiOffset });
            //キル数分スクロール
            float kill = float(resultDatas[i].killNum / 10) * 0.1f;
            killNumSpr->numUVScroll.x = kill;
        }

        // デス数
        { //一桁目
            GameObj deathNumObj = uiCanvas->AddChildObject();
            std::string deathNumName = std::to_string(i) + "st_PlayerDeathNum1";
            deathNumObj->SetName(deathNumName.c_str());
            Sprite* deathNumSpr = deathNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_Death_number.ui", Sprite::SpriteShader::DEFALTUV, false).get();
            deathNumSpr->SetIsParentMove(true);
            deathNumSpr->SetEasingPosition({ 747.0f,395.0f + (160 * i) + uiOffset });
            //デス数分スクロール
            float death = float(resultDatas[i].deathNum % 10) * 0.1f;
            deathNumSpr->numUVScroll.x = death;
        }
        { //二桁目
            GameObj deathNumObj = uiCanvas->AddChildObject();
            std::string deathNumName = std::to_string(i) + "st_PlayerDeathNum2";
            deathNumObj->SetName(deathNumName.c_str());
            Sprite* deathNumSpr = deathNumObj->AddComponent<Sprite>("Data/SerializeData/UIData/resultScene/Result_Death_number.ui", Sprite::SpriteShader::DEFALTUV, false).get();
            deathNumSpr->SetIsParentMove(true);
            deathNumSpr->SetEasingPosition({ 721.0f,395.0f + (160 * i) + uiOffset });
            //デス数分スクロール
            float death = float(resultDatas[i].deathNum / 10) * 0.1f;
            deathNumSpr->numUVScroll.x = death;
        }

        // キャラアイコン
        GameObj IconObj = uiCanvas->AddChildObject();
        std::string IconName = std::to_string(i) + "st_PlayerIcon";
        IconObj->SetName(IconName.c_str());

        UiSystem* IconSprite = IconObj->AddComponent<UiSystem>("Data/SerializeData/UIData/resultScene/Result_CharaIcon.ui", Sprite::SpriteShader::DEFALT, false).get();
        IconSprite->SetIsParentMove(true);
        IconSprite->SetEasingPosition({ 93.0f,380.0f + (160 * i) + uiOffset });

        switch (resultDatas[i].charaID)
        {
        case Chara::INAZO:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/InazawaCharacter.png");
            break;
        case Chara::FARAHC:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/FarahCom.png");
            break;
        case Chara::SANTORAT:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/JankratCharacter.png");
            break;
        case Chara::MATHYA:
            IconSprite->LoadTexture("Data/Texture/PlayerUI/CharaIcon/SoldierCom.png");
            break;
        }

        //キャラの名前
        GameObj charaname = uiCanvas->AddChildObject();
        std::string name = "charaname" + std::to_string(i + 1);
        charaname->SetName(name.c_str());
        std::shared_ptr<Font>f = charaname->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        f->position = { 154.0f, 365.0f + (160 * i) + uiOffset };
        f->str = stringconvert(resultDatas[i].playerName);
        f->scale = { 0.9f };
        f->color = { 1,1,1,0.0f };
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
    //親ステージ
    GameObj ResultStage = GameObjectManager::Instance().Create();
    ResultStage->SetName("ResultStage");
    ResultStage->transform_->SetScale({ 0.005f,0.005f,0.005f });
    auto& stage = ResultStage->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    stage->LoadModel("Data/Model/AbeStage/ResultStage.mdl");

    //エフェクト
    {
        GameObj BoostParent = ResultStage->AddChildObject();
        BoostParent->SetName("BoostParent");

        //ブーストが四つあるからそれに付随して付ける
        for (int i = 0; i < 4; ++i)
        {
            GameObj boosteffect = BoostParent->AddChildObject();
            std::string effectname = "boosteffect" + std::to_string(i + 1);
            boosteffect->SetName(effectname.c_str());
            boosteffect->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/result_boost.gpuparticle", 1000);

            //初期値をこのように制御する
            if (i == 0)
            {
                boosteffect->transform_->SetWorldPosition({ 134.347f,-62.699f,101.556f });
            }
            if (i == 1)
            {
                boosteffect->transform_->SetWorldPosition({ -132.713f,-63.360f,102.403f });
            }
            if (i == 2)
            {
                boosteffect->transform_->SetWorldPosition({ 132.212f,-62.527f,-102.284f });
            }
            if (i == 3)
            {
                boosteffect->transform_->SetWorldPosition({ -133.341f,-62.337f,-102.013f });
            }
        }
    }

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
        GameObj winChara = ResultStage->AddChildObject();
        winChara->transform_->SetScale({ 7.0f, 7.0f, 7.0f });
        std::string charaName = "winChara" + std::to_string(i + 1);
        winChara->SetName(charaName.c_str());
        auto& winCharaRender = winChara->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);

        switch (winCharaID[i])
        {
        case Chara::INAZO:
            winCharaRender->LoadModel("Data/Model/player_True/player1.mdl");
            break;
        case Chara::FARAHC:
            winCharaRender->LoadModel("Data/Model/player_True/player2.mdl");
            break;
        case Chara::SANTORAT:
            winCharaRender->LoadModel("Data/Model/player_True/player3.mdl");
            break;
        case Chara::MATHYA:
            winCharaRender->LoadModel("Data/Model/player_True/player4.mdl");
            break;
        }

        std::shared_ptr<AnimationCom>anim = winChara->AddComponent<AnimationCom>();
        anim->PlayAnimation(5, true);

        // 結果モデルに格納
        resultModel[i] = winChara;
    }

    //値を代入
    if (resultModel[0].lock() && resultModel[1].lock())
    {
        resultModel[0].lock()->transform_->SetWorldPosition({ 31.532f, -6.528f, 43.868f });
        resultModel[1].lock()->transform_->SetWorldPosition({ -35.722f, -7.606f, -14.812f });
    }

    //王冠
    {
        GameObj crawn1 = ResultStage->AddChildObject();
        crawn1->SetName("crawn1");
        crawn1->transform_->SetScale({ 1.7f,1.7f ,1.7f });
        crawn1->transform_->SetWorldPosition({ 34.960f,95.590f ,42.450f });
        crawn1->AddComponent<EasingMoveCom>("Data/SerializeData/3DEasingData/crawn.easingmove");
        auto& crawn1render = crawn1->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_NONE, true, false);
        crawn1render->LoadModel("Data/Model/Crawn/crawn.mdl");
        crawn1render->SetDissolveThreshold(1.0f);
        crawn1render->SetOutlineIntensity(10.0f);
        crawn1render->SetOutlineColor({ 0.899f, 1.000f, 0.000f });

        GameObj crawn2 = ResultStage->AddChildObject();
        crawn2->SetName("crawn2");
        crawn2->transform_->SetScale({ 1.7f,1.7f ,1.7f });
        crawn2->transform_->SetWorldPosition({ -32.329f,94.703f ,-15.492f });
        crawn2->AddComponent<EasingMoveCom>("Data/SerializeData/3DEasingData/crawn.easingmove");
        auto& crawn2render = crawn2->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_NONE, true, false);
        crawn2render->LoadModel("Data/Model/Crawn/crawn.mdl");
        crawn2render->SetDissolveThreshold(1.0f);
        crawn2render->SetOutlineIntensity(10.0f);
        crawn2render->SetOutlineColor({ 0.899f, 1.000f, 0.000f });
    }

    //王冠エフェクト１
    {
        GameObj crawneffect1 = GameObjectManager::Instance().Create();
        crawneffect1->SetName("crawneffect1");
        crawneffect1->transform_->SetWorldPosition({ 0.173f,0.536f,0.208f });
        crawneffect1->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/result_crawneffect.gpuparticle", 700);
        crawneffect1->SetEnabled(false);
    }

    //王冠エフェクト2
    {
        GameObj crawneffect2 = GameObjectManager::Instance().Create();
        crawneffect2->SetName("crawneffect2");
        crawneffect2->transform_->SetWorldPosition({ -0.162f,0.537f,-0.079f });
        crawneffect2->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/result_crawneffect.gpuparticle", 700);
        crawneffect2->SetEnabled(false);
    }

    //skipフォント
    {
        std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
        obj->SetName("skipfont");
        std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024, Font::FontShader::COOL);
        font->position = { 768.0f,993.0f };
        font->str = L"Enterでタイトルへ";
        font->scale = 0.8f;
        font->color.w = 1.0f;
    }
}

//イベントカメラの更新更新処理
void SceneResult::EventCamera(float elapsedTime)
{
    auto& stage = GameObjectManager::Instance().Find("ResultStage");

    if (!EventCameraManager::Instance().GetIsPlayEvent())
    {
        timer -= elapsedTime / 3;
        stage->GetChildFind("crawn1")->GetComponent<RendererCom>()->SetDissolveThreshold(timer);
        stage->GetChildFind("crawn2")->GetComponent<RendererCom>()->SetDissolveThreshold(timer);
        GameObjectManager::Instance().Find("crawneffect1")->SetEnabled(true);
        GameObjectManager::Instance().Find("crawneffect2")->SetEnabled(true);

        //経過時間
        limittimer += elapsedTime;
    }

    //イージング
    if (limittimer >= 2.0f && limittimer <= 2.1f)
    {
        auto& canvas = GameObjectManager::Instance().Find("Canvas");
        //リザルトのUI君
        for (int i = 0; i < 4; ++i)
        {
            std::string canvasName = "Player" + std::to_string(i) + "_UICanvas";
            resultUI[i].lock()->GetChildFind((canvasName + "_Frame").c_str())->GetComponent<Sprite>()->EasingPlay();
            std::string killNumName = std::to_string(i) + "st_PlayerKillNum1";
            resultUI[i].lock()->GetChildFind(killNumName.c_str())->GetComponent<Sprite>()->EasingPlay();
            std::string killNumNam2 = std::to_string(i) + "st_PlayerKillNum2";
            resultUI[i].lock()->GetChildFind(killNumNam2.c_str())->GetComponent<Sprite>()->EasingPlay();
            std::string deathNumName = std::to_string(i) + "st_PlayerDeathNum1";
            resultUI[i].lock()->GetChildFind(deathNumName.c_str())->GetComponent<Sprite>()->EasingPlay();
            std::string deathNumName2 = std::to_string(i) + "st_PlayerDeathNum2";
            resultUI[i].lock()->GetChildFind(deathNumName2.c_str())->GetComponent<Sprite>()->EasingPlay();
            std::string icon = std::to_string(i) + "st_PlayerIcon";
            resultUI[i].lock()->GetChildFind(icon.c_str())->GetComponent<Sprite>()->EasingPlay();
        }

        //勝敗君
        canvas->GetChildFind("Judge")->GetComponent<Sprite>()->EasingPlay();
    }

    // フォントの alpha 値を徐々に上げる処理
    if (limittimer >= 2.3f)
    {
        // 経過時間に応じて alpha を増加
        fontalpha += elapsedTime / 9;

        //フォント取得
        for (int i = 0; i < 4; ++i)
        {
            std::string name = "charaname" + std::to_string(i + 1);
            resultUI[i].lock()->GetChildFind(name.c_str())->GetComponent<Font>()->color = { 1,1,1 ,fontalpha };
        }

        // alpha 値を最大 1.0 に制限
        fontalpha = (std::min)(fontalpha, 1.0f);
    }

    //チェンジシーン
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (GamePad::ENTER & gamePad.GetButtonDown())
    {
        SceneManager::Instance().ChangeScene(new SceneTitle);
    }
}