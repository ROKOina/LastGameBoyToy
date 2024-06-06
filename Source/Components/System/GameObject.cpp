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

//�Q�[���I�u�W�F�N�g
#pragma region GameObject

// �J�n����
void GameObject::Start()
{
    for (std::shared_ptr<Component>& component : components_)
    {
        component->Start();
    }
}

// �X�V
void GameObject::Update(float elapsedTime)
{
    if (!isEnabled_)return;
    for (std::shared_ptr<Component>& component : components_)
    {
        if (!component->GetEnabled())continue;
        component->Update(elapsedTime);
    }
}

// �s��̍X�V
void GameObject::UpdateTransform()
{
    if (!isEnabled_)return;

    //�e�q�̍s��X�V
        //�e�����邩
    if (parentObject_.lock())
    {
        DirectX::XMFLOAT4X4 parentTransform = parentObject_.lock()->transform_->GetWorldTransform();
        transform_->SetParentTransform(parentTransform);
    }

    transform_->UpdateTransform();
}

// GUI�\��
void GameObject::OnGUI()
{
    //�L��
    ImGui::Checkbox(" ", &isEnabled_);
    ImGui::SameLine();

    // ���O
    {
        char buffer[1024];
        ::strncpy_s(buffer, sizeof(buffer), GetName(), sizeof(buffer));
        if (ImGui::InputText("Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            SetName(buffer);
        }
    }

    //�I�u�W�F�N�g�X�s�[�h
    ImGui::DragFloat("objSpeed", &objSpeed_, 0.01f);

    // �R���|�[�l���g
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
    // �R���|�[�l���g
    for (std::shared_ptr<Component>& component : components_)
    {
        component->Render2D(elapsedTime);
    }
}

//�e�q
std::shared_ptr<GameObject> GameObject::AddChildObject()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    //�e�o�^
    obj->parentObject_ = shared_from_this();
    //�q���ǉ�
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

//�Q�[���I�u�W�F�N�g�}�l�[�W���[
#pragma region GameObjectManager

// �쐬
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

// �폜
void GameObjectManager::Remove(std::shared_ptr<GameObject> obj)
{
    removeGameObject_.insert(obj);
}

// �S�폜
void GameObjectManager::AllRemove()
{
    for (auto& updateObj : updateGameObject_)
        Remove(updateObj);
    for (auto& startObj : startGameObject_)
        Remove(startObj);
}

// �X�V
void GameObjectManager::Update(float elapsedTime)
{
    for (std::shared_ptr<GameObject>& obj : startGameObject_)
    {
        if (!obj)continue;

        //�����_���[�R���|�[�l���g������΃����_�[�I�u�W�F�ɓ����
        std::shared_ptr<RendererCom> rendererComponent = obj->GetComponent<RendererCom>();
        if (rendererComponent)
        {
            renderSortObject_.emplace_back(rendererComponent);
        }

        //�p�[�e�B�N���I�u�W�F�N�g������Γ����
        std::shared_ptr<ParticleSystemCom> particleComponent = obj->GetComponent<ParticleSystemCom>();
        if (particleComponent)
        {
            particleObject_.emplace_back(particleComponent);
        }

        obj->Start();
        updateGameObject_.emplace_back(obj);

        //�����蔻��R���|�[�l���g�ǉ�
        std::shared_ptr<Collider> colliderComponent = obj->GetComponent<Collider>();
        if (colliderComponent)
        {
            colliderObject_.emplace_back(colliderComponent);
        }

        obj->UpdateTransform();
    }
    startGameObject_.clear();

    //�����蔻��
    {
        //����O�̃N���A
        for (auto& col : colliderObject_)
        {
            col.lock()->ColliderStartClear();
        }

        //����
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

    //�X�V
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        obj->Update(elapsedTime);
    }

    for (auto& f : future)
    {
        f.get();
    }
    future.clear();

    //�폜
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

    //�e�I�u�W�F�N�g���(�폜)
    {
        //child���
        for (std::weak_ptr<GameObject> parent : parentObj)
        {
            if (!parent.expired())
                parent.lock()->EraseExpiredChild();
        }

        //collider���
        for (int col = 0; col < colliderObject_.size(); ++col)
        {
            if (colliderObject_[col].expired())
            {
                colliderObject_.erase(colliderObject_.begin() + col);
                --col;
            }
        }
        //renderObject���
        for (int ren = 0; ren < renderSortObject_.size(); ++ren)
        {
            if (renderSortObject_[ren].expired())
            {
                renderSortObject_.erase(renderSortObject_.begin() + ren);
                --ren;
            }
        }

        //particleObject���
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

// �s��X�V
void GameObjectManager::UpdateTransform()
{
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        obj->UpdateTransform();
    }
}

// �`��
void GameObjectManager::Render()
{
    //3D�`��
    Render3D();

    //�p�[�e�B�N���`��
    ParticleRender();

    //debug
    if (Graphics::Instance().IsDebugGUI())
    {
        //�����蔻��p�f�o�b�O�`��
        for (auto& col : colliderObject_)
        {
            if (!col.lock()->GetEnabled())continue;
            if (!col.lock()->GetGameObject()->GetEnabled())continue;
            col.lock()->DebugRender();
        }

        // ���X�^�[�`��
        DrawLister();

        // �ڍו`��
        DrawDetail();
    }
}

void GameObjectManager::Render2D(float elapsedTime)
{
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        obj->Render2D(elapsedTime);
    }
}

//�Q�[���I�u�W�F�N�g��T��
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

//�f�o�b�OGUI�A�ċN�֐�
void CycleDrawLister(std::shared_ptr<GameObject> obj, std::set<std::shared_ptr<GameObject>>& selectObject)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    //�I��ł���Ȃ�A�t���O��I�����[�h��
    if (selectObject.find(obj) != selectObject.end())
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    //�q�����Ȃ��Ȃ�A���t���Ȃ�
    if (obj->GetChildren().size() == 0)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    //�f�o�b�N�`����e
    bool openNode = ImGui::TreeNodeEx(obj->GetName(), nodeFlags);

    if (ImGui::IsItemClicked())
    {
        // �P��I�������Ή����Ă���
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

    // ���[���h�s��̍X�V
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(selectedObject->transform_->GetScale().x, selectedObject->transform_->GetScale().y, selectedObject->transform_->GetScale().z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&selectedObject->transform_->GetRotation()));
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(selectedObject->transform_->GetLocalPosition().x, selectedObject->transform_->GetLocalPosition().y, selectedObject->transform_->GetLocalPosition().z);
    DirectX::XMStoreFloat4x4(&world, S * R * T);

    ImGuizmo::Manipulate(
        &view._11, &projection._11,           // �r���[���v���W�F�N�V�����s��
        m_guizmoOperation,                    // ����
        m_guizmoMode,                         // ���
        &world._11,                           // ���삷�郏�[���h�s��
        nullptr                               // �I�v�V�����̃X�i�b�v�ݒ�
    );

    //���[�J���s�񂩂�SRT�l�ɔ��f����
    DirectX::XMFLOAT3 scale = selectedObject->transform_->GetScale();
    DirectX::XMFLOAT4 rotate = selectedObject->transform_->GetRotation();
    DirectX::XMFLOAT3 position = selectedObject->transform_->GetWorldPosition();
    TransformUtils::MatrixToTransformation(world, &scale, &rotate, &position);
    selectedObject->transform_->SetScale(scale);
    selectedObject->transform_->SetRotation(rotate);
    selectedObject->transform_->SetWorldPosition(position);
}

// ���X�^�[�`��
void GameObjectManager::DrawLister()
{
    ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    isHiddenLister_ = !ImGui::Begin("GameObject Lister", nullptr, ImGuiWindowFlags_None);
    if (!isHiddenLister_)
    {
        for (std::shared_ptr<GameObject>& obj : updateGameObject_)
        {
            //parent�������continue
            if (obj->GetParent())continue;

            //�e�qGUI�p�̍ċN�֐�
            CycleDrawLister(obj, selectionGameObject_);
        }
    }
    ImGui::End();
}

// �ڍו`��
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

//3D�`��
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
            Graphics::Instance().GetModelShader(1)->Render(Graphics::Instance().GetDeviceContext(), model);
        }
    }
}

//�p�[�e�B�N���`��
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

//�I�u�W�F�N�g���
void GameObjectManager::EraseObject(std::vector<std::shared_ptr<GameObject>>& objs, std::shared_ptr<GameObject> removeObj)
{
    //�q������
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