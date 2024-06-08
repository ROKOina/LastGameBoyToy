#include "RendererCom.h"

#include "TransformCom.h"
#include <imgui.h>
#include <string>

//コンストラクタ
RendererCom::RendererCom(int shaderslot, int blendmode)
{
    m_blend = blendmode;
    m_modelshader = std::make_unique<ModelShader>(shaderslot);
}

// 開始処理
void RendererCom::Start()
{
}

//描画
void RendererCom::Render()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //セット
    m_modelshader->Begin(dc, m_blend);

    //モデルを描画
    for (auto& mesh : model_->GetResource()->GetMeshes())
    {
        for (auto& subset : mesh.subsets)
        {
            //頂点・インデックスバッファ等設定
            m_modelshader->SetBuffer(dc, model_->GetNodes(), mesh);

            //サブセット毎で描画
            m_modelshader->SetSubset(dc, subset);
        }
    }

    //解放
    m_modelshader->End(dc);
}

// 更新処理
void RendererCom::Update(float elapsedTime)
{
    // モデルの更新
    if (model_ != nullptr)
    {
        DirectX::XMFLOAT4X4 transform = GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform();
        model_->UpdateTransform(DirectX::XMLoadFloat4x4(&transform));
    }
}

// GUI描画
void RendererCom::OnGUI()
{
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
    ImGui::Combo("BlendMode", &m_blend, BlendName, static_cast<int>(BLENDSTATE::MAX), 10);
}

// モデルの読み込み
void RendererCom::LoadModel(const char* filename)
{
    ID3D11Device* device = Graphics::Instance().GetDevice();
    std::shared_ptr<ModelResource> m = std::make_shared<ModelResource>();
    m->Load(device, filename);

    model_ = std::make_unique<Model>(m);
}