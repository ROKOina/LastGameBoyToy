#include "RendererCom.h"
#include "Component/System/TransformCom.h"
#include <imgui.h>
#include <string>
#include "Graphics/Model/ResourceManager.h"

//コンストラクタ
RendererCom::RendererCom(SHADER_ID_MODEL id, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode, bool shadowrender, bool silhoutterender) :m_depth(depthmode), m_rasterizerState(rasterizermode)
{
    m_blend = blendmode;
    m_modelshader = std::make_unique<ModelShader>(id);
    m_shadow = std::make_unique<ModelShader>(SHADER_ID_MODEL::SHADOW);
    m_silhoutte = std::make_unique<ModelShader>(SHADER_ID_MODEL::SILHOUETTE);
    m_shadowrender = shadowrender;
    m_silhoutterender = silhoutterender;

    shaderID = id;
}

// 開始処理
void RendererCom::Start()
{
    if (OnFlag)
    {
        ChangeMaterialParameter();
    }
}

//描画
void RendererCom::Render()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    // 定数バッファをGPUに設定
    if (variousConstant.get() != nullptr)
    {
        variousConstant->UpdateConstantBuffer(dc);
    }

    //シルエット描画
    SilhoutteRender();

    //セット
    m_modelshader->Begin(dc, m_blend, m_depth, m_rasterizerState);

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

        // 定数バッファの更新
        if (variousConstant.get() != nullptr) {
            variousConstant->Update(elapsedTime);
        }
    }

    BoundsMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    BoundsMin = { FLT_MAX, FLT_MAX, FLT_MAX };

    auto& mehses = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource()->GetMeshesEdit();

    for (const ModelResource::Mesh& mesh : mehses)
    {
        // メッシュのローカル座標系での境界
        DirectX::XMFLOAT3 localBoundsMin = mesh.boundsMin;
        DirectX::XMFLOAT3 localBoundsMax = mesh.boundsMax;

        // メッシュのワールド行列（仮にworldMatrixというメンバ変数があると仮定）
        DirectX::XMMATRIX worldMatrix = DirectX::XMLoadFloat4x4(&GetGameObject()->GetComponent<TransformCom>()->GetWorldTransform());

        // boundsMin と boundsMax をワールド座標に変換
        DirectX::XMVECTOR minVec = DirectX::XMVector3Transform(XMLoadFloat3(&localBoundsMin), worldMatrix);
        DirectX::XMVECTOR maxVec = DirectX::XMVector3Transform(XMLoadFloat3(&localBoundsMax), worldMatrix);

        // ワールド座標に変換された結果をXMFLOAT3に保存
        DirectX::XMFLOAT3 worldBoundsMin, worldBoundsMax;
        DirectX::XMStoreFloat3(&worldBoundsMin, minVec);
        DirectX::XMStoreFloat3(&worldBoundsMax, maxVec);

        // シーン全体の最小座標と最大座標を更新
        BoundsMin.x = (std::min)(BoundsMin.x, worldBoundsMin.x);
        BoundsMin.y = (std::min)(BoundsMin.y, worldBoundsMin.y);
        BoundsMin.z = (std::min)(BoundsMin.z, worldBoundsMin.z);

        BoundsMax.x = (std::max)(BoundsMax.x, worldBoundsMax.x);
        BoundsMax.y = (std::max)(BoundsMax.y, worldBoundsMax.y);
        BoundsMax.z = (std::max)(BoundsMax.z, worldBoundsMax.z);

        if (std::string(GetGameObject()->GetName()) == "tes")
        {
            auto s = GetGameObject()->transform_->GetScale();
            DirectX::XMFLOAT3 a = (mesh.boundsMax - mesh.boundsMin) * s / 2;
            DirectX::XMFLOAT3 b = { 0,0,0 };
            b = (((mesh.boundsMax - mesh.boundsMin) / 2) + mesh.boundsMin) * s;
            DirectX::XMFLOAT3 wPos = b + GetGameObject()->transform_->GetWorldPosition();
            Graphics::Instance().GetDebugRenderer()->DrawBox(wPos, a, { 1.0f,1.0f,0.0f,1.0f });
        }
    }

    DirectX::XMFLOAT3 boundsMax = BoundsMax;
    DirectX::XMFLOAT3 boundsMin = BoundsMin;

    bounds.x = (boundsMax.x - boundsMin.x) / 2.0f;  // GetGameObject()->transform_->GetScale().x;
    bounds.y = (boundsMax.y - boundsMin.y) / 2.0f;                           //GetGameObject()->transform_->GetScale().y;
    bounds.z = (boundsMax.z - boundsMin.z) / 2.0f;                           //GetGameObject()->transform_->GetScale().z;

    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();

    pos += boundsMin + bounds;

    if (std::string(GetGameObject()->GetName()) == "test")
        Graphics::Instance().GetDebugRenderer()->DrawBox(pos, bounds, { 1.0f,0.0f,0.0f,1.0f }, GetGameObject()->transform_->GetRotation());
}

//影描画
void RendererCom::ShadowRender()
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
void RendererCom::SilhoutteRender()
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

//ボーンのguizmo
void RendererCom::BoneGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    model_->BoneGuizmo(view, projection);
}

// GUI描画
void RendererCom::OnGUI()
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

#ifdef _DEBUG
    MaterialSelector();

#endif // _DEBUG

    if (variousConstant.get() != nullptr) {
        ImGui::Separator();
        ImGui::Text("ConstantBuffer");

        variousConstant->DrawGui();
    }
}

// モデルの読み込み
void RendererCom::LoadModel(const char* filename)
{
    future = Graphics::Instance().GetThreadPool()->submit([&](auto filename) { return ModelInitialize(filename); }, filename);
}

void RendererCom::LoadMaterial(const char* filename)
{
    assert(model_.get() != nullptr && "モデルを読み込む前に関数を呼び出している");

    ID3D11Device* device = Graphics::Instance().GetDevice();
    model_->GetResource()->LoadMaterial(device, filename);
}

//マテリアルの名前を取得して値を変更する処理
void RendererCom::ChangeMaterialParameter()
{
    // 全マテリアルを取得
    std::vector<ModelResource::Material*> materials = GetAllMaterials();

    // ループでマテリアルの色を変更
    for (size_t i = 0; i < materials.size(); i++)
    {
        if (materials[i] != nullptr)
        {
            materials[i]->outlineColor = { 0.000f, 0.282f, 1.000f };
            materials[i]->outlineintensity = { 5.0f };
        }
    }
}

#include "SystemStruct\Framework.h"
#include <shlwapi.h>
#include "Graphics/Texture.h"
#include "SystemStruct\Dialog.h"

void RendererCom::ModelInitialize(const char* filename)
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

#ifdef _DEBUG
    modelFilePath = filename;

#endif // _DEBUG
}

void RendererCom::MaterialSelector()
{
    ImGui::Separator();

    std::vector<const char*> materialNames = {};

    ImGui::Checkbox("flag", &OnFlag);

    //マテリアル情報の表示(パターン1)
    ImGui::Text("Materials");
    if (model_->GetResource() != nullptr)
    {
        int materialIndex = 0;
        for (const ModelResource::Material& material : model_->GetResource()->GetMaterials())
        {
            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

            if (selectionMaterialIndex == materialIndex)
            {
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            }

            materialNames.push_back(material.name.c_str());

            ImGui::TreeNodeEx(&material, nodeFlags, material.name.c_str());

            if (ImGui::IsItemClicked())
            {
                selectionMaterialIndex = materialIndex;
            }

            ImGui::TreePop();

            ++materialIndex;
        }
    }

    //マテリアル情報の表示(パターン2)
    ImGui::Combo("Materials", &selectionMaterialIndex, &materialNames[0], materialNames.size(), 5);

    ImGui::Separator();

    // マテリアルのプロパティを表示
    ImGui::Text("Property");
    ModelResource::Material* material = GetSelectionMaterial();
    if (material != nullptr)
    {
        if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ラベル名が重複してしまうと起きる問題の対応
            ImGui::PushID(0);
            {
                char name[256];
                ::strncpy_s(name, sizeof(name), material->name.c_str(), sizeof(name));
                if (ImGui::InputText("Name", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    material->name = name;
                }
            }
            ImGui::PopID();

            // テクスチャ編集
            TextureGui(material);

            ImGui::ColorEdit4("Color", &material->color.x, ImGuiColorEditFlags_None);
            ImGui::DragFloat("Roughness", &material->Roughness, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &material->Metalness, 0.01f, 0.0f, 1.0f);
            ImGui::ColorEdit3("EmissiveColor", &material->emissivecolor.x, ImGuiColorEditFlags_None);
            ImGui::DragFloat("EmissivePower", &material->emissiveintensity, 0.1f, 0.0f, 100.0f);
            ImGui::ColorEdit3("outlineColor", &material->outlineColor.x);
            ImGui::DragFloat("outlineintensity", &material->outlineintensity, 0.1f, 0.0f, 10.0f);
            ImGui::DragFloat("dissolveThreshold", &material->dissolveThreshold, 0.1f, 0.0f, 1.0f);
            ImGui::DragFloat("dissolveEdgeWidth", &material->dissolveEdgeWidth, 0.001f, 0.01f, 0.1f);
            ImGui::ColorEdit3("dissolveEdgeColor", &material->dissolveEdgeColor.x);
            ImGui::DragFloat("alpha", &material->alpha, 0.1f, 0.0f, 1.0f);

            // マテリアルファイルを上書き
            if (ImGui::Button("Save")) {
                ExportMaterialFile();
            }
        }
    }
}

ModelResource::Material* RendererCom::GetSelectionMaterial()
{
    if (model_.get() != nullptr)
    {
        const std::vector<ModelResource::Material>& materials = model_->GetResource()->GetMaterials();
        if (selectionMaterialIndex >= 0 && selectionMaterialIndex < static_cast<int>(materials.size()))
        {
            return const_cast<ModelResource::Material*>(&materials.at(selectionMaterialIndex));
        }
    }
    return nullptr;
}

//全マテリアル取得
std::vector<ModelResource::Material*> RendererCom::GetAllMaterials()
{
    std::vector<ModelResource::Material*> materialPtrs;

    if (model_.get() != nullptr)
    {
        const std::vector<ModelResource::Material>& materials = model_->GetResource()->GetMaterials();
        materialPtrs.reserve(materials.size());

        for (const auto& material : materials)
        {
            // const_castを使用してポインタを格納
            materialPtrs.push_back(const_cast<ModelResource::Material*>(&material));
        }
    }

    return materialPtrs;
}

void RendererCom::TextureGui(ModelResource::Material* material)
{
    ImGui::Separator();
    ImGui::Text("TextureEdit");

    char id[12] = "ModelFile";
    ImGui::PushID(id);
    ImGui::Text("Model");
    ImGui::SameLine();
    if (ImGui::Button("...")) {
        char modelFile[256];
        ::strncpy_s(modelFile, sizeof(modelFile), material->textureFilename[0].c_str(), sizeof(modelFile));

        const char* filter = "Texture Files(*.mdl)\0*.mdl;\0All Files(*.*)\0*.*;\0\0";
        DialogResult result = Dialog::OpenFileName(modelFile, sizeof(modelFile), filter, nullptr, Framework::GetInstance()->GetHWND());
        if (result == DialogResult::OK)
        {
            filePathDriveToModel = modelFile;
        }
    }
    ImGui::PopID();

    if (filePathDriveToModel.empty())return;

    // 適用するテクスチャを選択
    char textureFile[256];
    const char* fileKinds[6] = { "Diffuse","Normal","Metallic","Roughness","Ao","Emissive" };
    for (int i = 0; i < 6; ++i)
    {
        ::strncpy_s(textureFile, sizeof(textureFile), material->textureFilename[i].c_str(), sizeof(textureFile));

        ImGui::PushID(&material->textureFilename[i]);
        if (ImGui::Button("..."))
        {
            const char* filter = "Texture Files(*.DDS;*.png;*.tga;*.jpg;*.tif)\0*.DDS;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
            DialogResult result = Dialog::OpenFileName(textureFile, sizeof(textureFile), filter, nullptr, Framework::GetInstance()->GetHWND());
            if (result == DialogResult::OK)
            {
                char drive[32], dir[256], dirname[256];
                ::_splitpath_s(filePathDriveToModel.c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
                ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);
                dirname[strlen(dirname) - 1] = '\0';
                char relativeTextureFile[MAX_PATH];
                PathRelativePathToA(relativeTextureFile, dirname, FILE_ATTRIBUTE_DIRECTORY, textureFile, FILE_ATTRIBUTE_ARCHIVE);

                // 読み込み
                material->textureFilename[i] = relativeTextureFile;
                material->LoadTexture(Graphics::Instance().GetDevice(), textureFile, i);
            }
        }
        ImGui::SameLine();

        ::strncpy_s(textureFile, sizeof(textureFile), material->textureFilename[i].c_str(), sizeof(textureFile));
        if (ImGui::InputText(fileKinds[i], textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            material->textureFilename[i] = textureFile;

            char drive[32], dir[256], fullPath[256];
            ::_splitpath_s(filePathDriveToModel.c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
            ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
            material->LoadTexture(Graphics::Instance().GetDevice(), fullPath, i);
        }
        ImGui::Text("TextureResource");
        ImGui::Image(material->shaderResourceView[i].Get(), { 156, 156 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        ImGui::PopID();
    }
}

void RendererCom::ExportMaterialFile()
{
    static const char* filter = "Material Files(*.Material)\0*.Material;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "mdl", Framework::GetInstance()->GetHWND());
    if (result == DialogResult::OK)
    {
        model_->GetResource()->MaterialSerialize(filename);
    }
}