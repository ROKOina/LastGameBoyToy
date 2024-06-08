#include <imgui.h>
#include "../../Graphics/Graphics.h"
#include "Component.h"
#include "GameObject.h"
#include "../RendererCom.h"
#include "../TransformCom.h"
#include "../CameraCom.h"
#include "../ColliderCom.h"
#include "../ParticleSystemCom.h"
#include "GameSource/Math/Collision.h"
#include "Graphics/Shaders/3D/ModelShader.h"
#include "SystemStruct/TransformUtils.h"

//ゲームオブジェクト
#pragma region GameObject

// 開始処理
void GameObject::Start()
{
    for (std::shared_ptr<Component>& component : components_)
    {
        component->Start();
    }
}

// 更新
void GameObject::Update(float elapsedTime)
{
    if (!isEnabled_)return;
    for (std::shared_ptr<Component>& component : components_)
    {
        if (!component->GetEnabled())continue;
        component->Update(elapsedTime);
    }
}

// 行列の更新
void GameObject::UpdateTransform()
{
    if (!isEnabled_)return;

    //親子の行列更新
        //親がいるか
    if (parentObject_.lock())
    {
        DirectX::XMFLOAT4X4 parentTransform = parentObject_.lock()->transform_->GetWorldTransform();
        transform_->SetParentTransform(parentTransform);
    }

    transform_->UpdateTransform();
}

// GUI表示
void GameObject::OnGUI()
{
    //有効
    ImGui::Checkbox(" ", &isEnabled_);
    ImGui::SameLine();

    // 名前
    {
        char buffer[1024];
        ::strncpy_s(buffer, sizeof(buffer), GetName(), sizeof(buffer));
        if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            SetName(buffer);
        }
    }

    //オブジェクトスピード
    ImGui::DragFloat("objSpeed", &objSpeed_, 0.01f);

    // コンポーネント
    for (std::shared_ptr<Component>& component : components_)
    {
        ImGui::Spacing();
        ImGui::Separator();

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (ImGui::TreeNodeEx(component->GetName(), nodeFlags))
        {
            bool enabled = component->GetEnabled();
            if (ImGui::Checkbox(" ", &enabled))
            {
                component->SetEnabled(enabled);
            }
            component->OnGUI();
            ImGui::TreePop();
        }
    }
}

void GameObject::Render2D(float elapsedTime)
{
    // コンポーネント
    for (std::shared_ptr<Component>& component : components_)
    {
        component->Render2D(elapsedTime);
    }
}

//親子
std::shared_ptr<GameObject> GameObject::AddChildObject()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    //親登録
    obj->parentObject_ = shared_from_this();
    //子供追加
    childrenObject_.emplace_back(obj->shared_from_this());

    return obj;
}

void GameObject::EraseExpiredChild()
{
    for (int childCount = 0; childCount < childrenObject_.size(); ++childCount)
    {
        if (childrenObject_[childCount].expired())
        {
            childrenObject_.erase(childrenObject_.begin() + childCount);
            --childCount;
        }
    }
}

void GameObject::AudioRelease()
{
    for (auto& com : components_)
    {
        com->AudioRelease();
    }
}

#pragma endregion	endGameObject

//ゲームオブジェクトマネージャー
#pragma region GameObjectManager

// 作成
std::shared_ptr<GameObject> GameObjectManager::Create()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::shared_ptr<GameObject> obj = std::make_shared<GameObject>();
    obj->AddComponent<TransformCom>();
    {
        static int id = 0;
        char name[256];
        ::sprintf_s(name, sizeof(name), "Actor%d", id++);
        obj->SetName(name);
    }
    startGameObject_.emplace_back(obj);
    return obj;
}

// 削除
void GameObjectManager::Remove(std::shared_ptr<GameObject> obj)
{
    removeGameObject_.insert(obj);
}

// 全削除
void GameObjectManager::AllRemove()
{
    for (auto& updateObj : updateGameObject_)
        Remove(updateObj);
    for (auto& startObj : startGameObject_)
        Remove(startObj);
}

// 更新
void GameObjectManager::Update(float elapsedTime)
{
    for (std::shared_ptr<GameObject>& obj : startGameObject_)
    {
        if (!obj)continue;

        //レンダラーコンポーネントがあればレンダーオブジェに入れる
        std::shared_ptr<RendererCom> rendererComponent = obj->GetComponent<RendererCom>();
        if (rendererComponent)
        {
            renderSortObject_.emplace_back(rendererComponent);
        }

        //パーティクルオブジェクトがあれば入れる
        std::shared_ptr<ParticleSystemCom> particleComponent = obj->GetComponent<ParticleSystemCom>();
        if (particleComponent)
        {
            particleObject_.emplace_back(particleComponent);
        }

        obj->Start();
        updateGameObject_.emplace_back(obj);

        //当たり判定コンポーネント追加
        std::shared_ptr<Collider> colliderComponent = obj->GetComponent<Collider>();
        if (colliderComponent)
        {
            colliderObject_.emplace_back(colliderComponent);
        }

        obj->UpdateTransform();
    }
    startGameObject_.clear();

    //当たり判定
    {
        //判定前のクリア
        for (auto& col : colliderObject_)
        {
            col.lock()->ColliderStartClear();
        }

        //判定
        for (int col1 = 0; col1 < colliderObject_.size(); ++col1)
        {
            if (!colliderObject_[col1].lock()->GetGameObject()->GetEnabled())continue;
            for (int col2 = col1 + 1; col2 < colliderObject_.size(); ++col2)
            {
                if (!colliderObject_[col2].lock()->GetGameObject()->GetEnabled())continue;
                colliderObject_[col1].lock()->ColliderVSOther(colliderObject_[col2].lock());
            }
        }
    }

    //更新
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        obj->Update(elapsedTime);
    }

    for (auto& f : future)
    {
        f.get();
    }
    future.clear();

    //削除
    std::vector<std::weak_ptr<GameObject>> parentObj;
    for (const std::shared_ptr<GameObject>& obj : removeGameObject_)
    {
        EraseObject(startGameObject_, obj);
        EraseObject(updateGameObject_, obj);

        std::set<std::shared_ptr<GameObject>>::iterator itSelection = selectionGameObject_.find(obj);
        if (itSelection != selectionGameObject_.end())
        {
            selectionGameObject_.erase(itSelection);
        }

        if (obj->GetParent())
            parentObj.emplace_back(obj->GetParent());
    }
    removeGameObject_.clear();

    //各オブジェクト解放(削除)
    {
        //child解放
        for (std::weak_ptr<GameObject> parent : parentObj)
        {
            if (!parent.expired())
                parent.lock()->EraseExpiredChild();
        }

        //collider解放
        for (int col = 0; col < colliderObject_.size(); ++col)
        {
            if (colliderObject_[col].expired())
            {
                colliderObject_.erase(colliderObject_.begin() + col);
                --col;
            }
        }
        //renderObject解放
        for (int ren = 0; ren < renderSortObject_.size(); ++ren)
        {
            if (renderSortObject_[ren].expired())
            {
                renderSortObject_.erase(renderSortObject_.begin() + ren);
                --ren;
            }
        }

        //particleObject解放
        for (int per = 0; per < particleObject_.size(); ++per)
        {
            if (particleObject_[per].expired())
            {
                particleObject_.erase(particleObject_.begin() + per);
                --per;
            }
        }
    }
}

// 行列更新
void GameObjectManager::UpdateTransform()
{
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        obj->UpdateTransform();
    }
}

// 描画
void GameObjectManager::Render()
{
    //デファードレンダリングの初期設定
    m_posteffect->DeferredFirstSet();

    //3D描画
    Render3D();

    //パーティクル描画
    ParticleRender();

    //デファードレンダリング終了
    m_posteffect->DeferredResourceSet();

    //ポストエフェクト
    m_posteffect->PostEffectRender();

    //debug
    if (Graphics::Instance().IsDebugGUI())
    {
        //当たり判定用デバッグ描画
        for (auto& col : colliderObject_)
        {
            if (!col.lock()->GetEnabled())continue;
            if (!col.lock()->GetGameObject()->GetEnabled())continue;
            col.lock()->DebugRender();
        }

        // リスター描画
        DrawLister();

        // 詳細描画
        DrawDetail();

        //ポストエフェクトimgui
        m_posteffect->PostEffectImGui();
    }
}

void GameObjectManager::Render2D(float elapsedTime)
{
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        obj->Render2D(elapsedTime);
    }
}

//ゲームオブジェクトを探す
std::shared_ptr<GameObject> GameObjectManager::Find(const char* name)
{
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        if (std::strcmp(obj->GetName(), name) == 0)return obj;
    }

    for (std::shared_ptr<GameObject>& obj : startGameObject_)
    {
        if (std::strcmp(obj->GetName(), name) == 0)return obj;
    }

    return nullptr;
}

//デバッグGUI、再起関数
void CycleDrawLister(std::shared_ptr<GameObject> obj, std::set<std::shared_ptr<GameObject>>& selectObject)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    //選んでいるなら、フラグを選択モードに
    if (selectObject.find(obj) != selectObject.end())
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    //子がいないなら、▼付けない
    if (obj->GetChildren().size() == 0)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    //デバック描画内容
    bool openNode = ImGui::TreeNodeEx(obj->GetName(), nodeFlags);

    if (ImGui::IsItemClicked())
    {
        // 単一選択だけ対応しておく
        selectObject.clear();
        selectObject.insert(obj);
    }

    if (!openNode)return;

    for (std::weak_ptr<GameObject>& child : obj->GetChildren())
    {
        CycleDrawLister(child.lock(), selectObject);
    }

    ImGui::TreePop();
}

//imguiguizmo
void GameObjectManager::DrawGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    if (selectionGameObject_.empty())
    {
        return;
    }

    std::shared_ptr<GameObject> selectedObject = *selectionGameObject_.begin();

    // ワールド行列の更新
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(selectedObject->transform_->GetScale().x, selectedObject->transform_->GetScale().y, selectedObject->transform_->GetScale().z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&selectedObject->transform_->GetRotation()));
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(selectedObject->transform_->GetLocalPosition().x, selectedObject->transform_->GetLocalPosition().y, selectedObject->transform_->GetLocalPosition().z);
    DirectX::XMStoreFloat4x4(&world, S * R * T);

    ImGuizmo::Manipulate(
        &view._11, &projection._11,           // ビュー＆プロジェクション行列
        m_guizmoOperation,                    // 操作
        m_guizmoMode,                         // 空間
        &world._11,                           // 操作するワールド行列
        nullptr                               // オプションのスナップ設定
    );

    //ローカル行列からSRT値に反映する
    DirectX::XMFLOAT3 scale = selectedObject->transform_->GetScale();
    DirectX::XMFLOAT4 rotate = selectedObject->transform_->GetRotation();
    DirectX::XMFLOAT3 position = selectedObject->transform_->GetWorldPosition();
    TransformUtils::MatrixToTransformation(world, &scale, &rotate, &position);
    selectedObject->transform_->SetScale(scale);
    selectedObject->transform_->SetRotation(rotate);
    selectedObject->transform_->SetWorldPosition(position);
}

// リスター描画
void GameObjectManager::DrawLister()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    isHiddenLister_ = !ImGui::Begin("GameObject Lister", nullptr, ImGuiWindowFlags_None);
    if (!isHiddenLister_)
    {
        for (std::shared_ptr<GameObject>& obj : updateGameObject_)
        {
            //parentがあればcontinue
            if (obj->GetParent())continue;

            //親子GUI用の再起関数
            CycleDrawLister(obj, selectionGameObject_);
        }
    }
    ImGui::End();
}

// 詳細描画
void GameObjectManager::DrawDetail()
{
    ImGui::SetNextWindowPos(ImVec2(950, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    isHiddenDetail_ = !ImGui::Begin("GameObject Detail", nullptr, ImGuiWindowFlags_None);
    if (!isHiddenDetail_)
    {
        std::shared_ptr<GameObject> lastSelected = selectionGameObject_.empty() ? nullptr : *selectionGameObject_.rbegin();
        if (lastSelected != nullptr)
        {
            lastSelected->OnGUI();
        }
    }
    ImGui::End();
}

//3D描画
void GameObjectManager::Render3D()
{
    if (renderSortObject_.size() <= 0)return;

    for (std::weak_ptr<RendererCom>& renderObj : renderSortObject_)
    {
        if (!renderObj.lock()->GetGameObject()->GetEnabled())continue;
        if (!renderObj.lock()->GetEnabled())continue;

        Model* model = renderObj.lock()->GetModel();
        if (model != nullptr)
        {
            renderObj.lock()->Render();
        }
    }
}

//パーティクル描画
void GameObjectManager::ParticleRender()
{
    if (particleObject_.size() <= 0)return;

    for (std::weak_ptr<ParticleSystemCom>& particleObj : particleObject_)
    {
        if (!particleObj.lock()->GetGameObject()->GetEnabled())continue;
        if (!particleObj.lock()->GetEnabled())continue;

        particleObj.lock()->Render();
    }
}

//オブジェクト解放
void GameObjectManager::EraseObject(std::vector<std::shared_ptr<GameObject>>& objs, std::shared_ptr<GameObject> removeObj)
{
    //子から解放
    for (auto& childObj : removeObj->GetChildren())
    {
        EraseObject(objs, childObj.lock());
    }

    std::vector<std::shared_ptr<GameObject>>::iterator it = std::find(objs.begin(), objs.end(), removeObj);
    if (it != objs.end())
    {
        removeObj->AudioRelease();
        objs.erase(it);
    }
}

#pragma endregion endGameObjectManager