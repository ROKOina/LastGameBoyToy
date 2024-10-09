#include "InstanceRendererCom.h"
#include "TransformCom.h"
#include <Graphics/Model/ResourceManager.h>

//コンストラクタ
InstanceRenderer::InstanceRenderer(SHADER_ID_MODEL id, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode, bool shadowrender, bool silhoutterender)
{
    m_blend = blendmode;
    m_instancemodelshader = std::make_unique<InstanceModelShader>(id);
    m_shadow = std::make_unique<InstanceModelShader>(SHADER_ID_MODEL::SHADOW);
    m_silhoutte = std::make_unique<InstanceModelShader>(SHADER_ID_MODEL::SILHOUETTE);
    m_shadowrender = shadowrender;
    m_silhoutterender = silhoutterender;

    shaderID = id;
}

//初期化
void InstanceRenderer::Start()
{
}

//描画
void InstanceRenderer::Render()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //シルエット描画
    SilhoutteRender();

    //セット
    m_instancemodelshader->Begin(dc, m_blend, m_depth, m_rasterizerState);

    //モデルを描画
    for (auto& mesh : model_->GetResource()->GetMeshes())
    {
        for (auto& subset : mesh.subsets)
        {
            //頂点・インデックスバッファ等設定
            m_instancemodelshader->SetBuffer(dc, model_->GetNodes(), mesh);

            //サブセット毎で描画
            m_instancemodelshader->SetSubset(dc, subset);
        }
    }

    //解放
    m_instancemodelshader->End(dc);
}

//更新処理
void InstanceRenderer::Update(float elapsedTime)
{
    // モデルの更新
    if (model_ != nullptr)
    {
        DirectX::XMFLOAT4X4 transform = GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform();
        model_->UpdateTransform(DirectX::XMLoadFloat4x4(&transform));
    }
}

//影描画
void InstanceRenderer::ShadowRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //影を描画するかのフラグ
    if (m_shadowrender)
    {
        //セット
        m_shadow->ShadowBegin(dc, BLENDSTATE::REPLACE, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK);

        //モデルを描画
        for (auto& mesh : model_->GetResource()->GetMeshes())
        {
            for (auto& subset : mesh.subsets)
            {
                //頂点・インデックスバッファ等設定
                m_shadow->SetBuffer(dc, model_->GetNodes(), mesh);

                //サブセット毎で描画
                m_shadow->ShadowSetSubset(dc, subset);
            }
        }

        //解放
        m_shadow->End(dc);
    }
}

//シルエット描画
void InstanceRenderer::SilhoutteRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //シルエット描画をフラグで制御
    if (m_silhoutterender)
    {
        m_silhoutte->Begin(dc, BLENDSTATE::ALPHA, DEPTHSTATE::SILHOUETTE, RASTERIZERSTATE::SOLID_CULL_BACK);

        //シルエットを描画
        for (auto& mesh : model_->GetResource()->GetMeshes())
        {
            for (auto& subset : mesh.subsets)
            {
                //頂点・インデックスバッファ等設定
                m_silhoutte->SetBuffer(dc, model_->GetNodes(), mesh);

                //サブセット毎で描画
                m_silhoutte->SetSubset(dc, subset);
            }
        }

        //解放
        m_silhoutte->End(dc);
    }
}

//GUI
void InstanceRenderer::OnGUI()
{
    ImGui::SameLine();
    ImGui::Checkbox("shadow", &m_shadowrender);
    ImGui::SameLine();
    ImGui::Checkbox("silhoutte", &m_silhoutterender);

    //デバッグ用にブレンドモード設定
    constexpr const char* BlendName[] =
    {
      "NONE",
      "ALPHA",
      "ADD",
      "SUBTRACT",
      "REPLACE",
      "MULTIPLY",
      "LIGHTEN",
      "DARKEN",
      "SCREEN",
      "MULTIPLERENDERTARGETS",
    };
    //ブレンドモード設定リストとのサイズが違うとエラーを出す
    static_assert(ARRAYSIZE(BlendName) != static_cast<int>(BLENDSTATE::MAX) - 1, "BlendName Size Error!");

    //ブレンドモード設定
    int blendMode = static_cast<int>(m_blend);
    ImGui::Combo("BlendMode", &blendMode, BlendName, static_cast<int>(BLENDSTATE::MAX), static_cast<int>(BLENDSTATE::MAX));
    m_blend = static_cast<BLENDSTATE>(blendMode);

    //デバッグ用にデプスモード設定
    constexpr const char* DepthName[] =
    {
      "NONE",
      "ZT_ON_ZW_ON",
      "ZT_ON_ZW_OFF",
      "ZT_OFF_ZW_ON",
      "ZT_OFF_ZW_OFF",
      "SILHOUETTE",
      "MASK",
      "APPLY_MASK",
      "EXCLUSIVE",
    };
    //デプスモード設定リストとのサイズが違うとエラーを出す
    static_assert(ARRAYSIZE(DepthName) != static_cast<int>(DEPTHSTATE::MAX) - 1, "DepthName Size Error!");

    //デプスステンシルモード設定
    int depthMode = static_cast<int>(m_depth);
    ImGui::Combo("DepthMode", &depthMode, DepthName, static_cast<int>(DEPTHSTATE::MAX), static_cast<int>(DEPTHSTATE::MAX));
    m_depth = static_cast<DEPTHSTATE>(depthMode);

    //デバッグ用にラスタライズモード設定
    constexpr const char* RasterizerName[] =
    {
      "SOLID_CULL_NONE",
      "SOLID_CULL_BACK",
      "SOLID_CULL_FRONT",
      "WIREFRAME",
    };
    //ラスタライズモード設定リストとのサイズが違うとエラーを出す
    static_assert(ARRAYSIZE(RasterizerName) != static_cast<int>(RASTERIZERSTATE::MAX) - 1, "RasterizerName Size Error!");

    //ラスタライズモード設定
    int rasMode = static_cast<int>(m_rasterizerState);
    ImGui::Combo("RasterizerMode", &rasMode, RasterizerName, static_cast<int>(RASTERIZERSTATE::MAX), static_cast<int>(RASTERIZERSTATE::MAX));
    m_rasterizerState = static_cast<RASTERIZERSTATE>(rasMode);
}

//モデル読み込み
void InstanceRenderer::LoadModel(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();

    //リソースマネージャーに登録されているか
    if (!ResourceManager::Instance().JudgeModelFilename(filename))
    {
        m->Load(device, filename);
        ResourceManager::Instance().RegisterModel(filename, m);	//リソースマネージャーに追加する
    }
    else
    {
        m = ResourceManager::Instance().LoadModelResource(filename);	//ロードする
    }

    model_ = std::make_unique<Model>(m);
}