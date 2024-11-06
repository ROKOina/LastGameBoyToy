#include <imgui.h>
#include "../../Graphics/Graphics.h"
#include "Component.h"
#include "GameObject.h"
#include "Component/Renderer/RendererCom.h"
#include "TransformCom.h"
#include "Component/Camera/CameraCom.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Collsion/PushBackCom.h"
#include "Math/Collision.h"
#include "Graphics/3DShader/ModelShader.h"
#include "Math/TransformUtils.h"
#include "Component/Particle/CPUParticle.h"
#include "Component/Particle/GPUParticle.h"
#include "Component/Camera/FreeCameraCom.h"
#include "Component/Sprite/Sprite.h"
#include "Component/Renderer/InstanceRendererCom.h"
#include "Component\Renderer\DecalCom.h"
#include "Component\PostEffect\PostEffect.h"

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

void GameObject::OnDestroy()
{
    for (std::shared_ptr<Component>& component : components_)
    {
        if (!component->GetEnabled())continue;
        component->OnDestroy();
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
    int componentIndex = 0;
    for (std::shared_ptr<Component>& component : components_)
    {
        ImGui::Spacing();
        ImGui::Separator();

        // �e�R���|�[�l���g�ɌŗL�̎��ʎq��ǉ�
        std::string componentLabel = std::string(component->GetName()) + "##" + std::to_string(componentIndex);
        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        if (ImGui::TreeNodeEx(componentLabel.c_str(), nodeFlags))
        {
            bool enabled = component->GetEnabled();
            std::string checkboxLabel = "##Enabled" + std::to_string(componentIndex);
            if (ImGui::Checkbox(checkboxLabel.c_str(), &enabled))
            {
                component->SetEnabled(enabled);
            }

            component->OnGUI();
            ImGui::TreePop();
        }

        componentIndex++;
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
    //std::lock_guard<std::mutex> lock(mutex_);

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

//�e���L�����m�F
bool IsParentEnable(const std::weak_ptr<GameObject> obj)
{
    auto& parent = obj.lock()->GetParent();

    if (!parent)return true;    //�e�����Ȃ��ꍇ
    if (!parent->GetEnabled())
    {
        return false; //�e���������̎�
    }

    return IsParentEnable(parent);  //�ċA����
}

// �X�V
void GameObjectManager::Update(float elapsedTime)
{
    // �V�����������ꂽ�Q�[���I�u�W�F�N�g�̏����ݒ�
    StartUpObjects();

    //�����蔻��
    CollideGameObjects();
    //�����o������
    PushBackGameObjects();

    //�X�V
    for (std::shared_ptr<GameObject>& obj : updateGameObject_)
    {
        if (IsParentEnable(obj))
            obj->Update(elapsedTime);
    }

    //�폜
    RemoveGameObjects();
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
void GameObjectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& lightdirection)
{
    // �L���ȃ|�X�g�G�t�F�N�g�����W
    auto activeEffects = GetActivePostEffects();

    // �e�`��
    for (auto& effect : activeEffects)
    {
        effect->GetCascadedShadow()->Make(Graphics::Instance().GetDeviceContext(), view, projection, lightdirection, effect->m_criticaldepthvalue, [&]()
            {
                RenderShadow();
                InstanceRenderShadow();
            });
    }

    // �I�t�X�N���[���`��J�n
    for (auto& effect : activeEffects)
    {
        effect->StartOffScreenRendering();
    }

    // �f�t�@�[�h�����_�����O�̏����ݒ�
    for (auto& effect : activeEffects)
    {
        effect->SetDeferredTarget();
    }

    // 3D�`��
    RenderDeferred();
    InstanceRenderDeferred();

    // �f�t�@�[�h�����_�����O�I��
    for (auto& effect : activeEffects)
    {
        effect->EndDeferred();
    }

    // �[�x�}�b�v���R�s�[����GPU�ɐݒ�
    for (auto& effect : activeEffects)
    {
        effect->DepthCopyAndBind(8);
    }

    // �e��p�[�e�B�N���ƃt�H���[�h�����_�����O
    CPUParticleRender();
    GPUParticleRender();
    RenderForward();
    InstanceRenderForward();

    // �f�o�b�O�����_�[
    auto& graphics = Graphics::Instance();
    graphics.GetDebugRenderer()->Render(graphics.GetDeviceContext(), view, projection);
    graphics.GetLineRenderer()->Render(graphics.GetDeviceContext(), view, projection);

    // �[�x�}�b�v���g�p����V�F�[�_�[�`��
    RenderUseDepth();
    InstanceRenderUseDepth();

    // �f�J�[���`��
    DecalRender();

    // �|�X�g�G�t�F�N�g�`��
    for (auto& effect : activeEffects)
    {
        effect->PostEffectRender();
    }

    // �X�v���C�g�`��
    SpriteRender(view, projection);

    // �f�o�b�O���̕`��
    if (graphics.IsDebugGUI())
    {
        // �����蔻��p�f�o�b�O�`��
        for (auto& col : colliderObject_)
        {
            if (auto collider = col.lock())
            {
                if (collider->GetEnabled() && collider->GetGameObject()->GetEnabled())
                {
                    collider->DebugRender();
                }
            }
        }

        // �����o������p�f�o�b�O�`��
        for (auto& pb : pushBackObject_)
        {
            if (auto pushBack = pb.lock())
            {
                if (pushBack->GetEnabled() && pushBack->GetGameObject()->GetEnabled())
                {
                    pushBack->DebugRender();
                }
            }
        }

        // ���X�^�[�Əڍׂ̕`��
        DrawLister();
        DrawDetail();
    }

    // �M�Y���̕`��
    DrawGuizmo(view, projection);
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

        if (ImGui::IsMouseDoubleClicked(0))
        {
            ImGuiIO& io = ImGui::GetIO();

            // ImGui��Ƀ}�E�X�J�[�\��������ꍇ�͏������Ȃ�
            if (!io.WantCaptureMouse) return;

            GameObjectManager::Instance().Find("freecamera")->GetComponent<FreeCameraCom>()->SetFocusPos(obj->transform_->GetWorldPosition());
            GameObjectManager::Instance().Find("freecamera")->GetComponent<FreeCameraCom>()->SetDistance(5.0f);
        }
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
    auto& parent = selectedObject->GetParent();
    DirectX::XMFLOAT4X4 world = {};

    if (parent) {
        parent->transform_->UpdateTransform();
        selectedObject->transform_->SetParentTransform(parent->transform_->GetWorldTransform());
    }
    selectedObject->transform_->UpdateTransform();
    world = selectedObject->transform_->GetWorldTransform();

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

    if (parent) {
        // �e�̃��[���h�t�s��Ǝ��g�̃��[���h�s�����Z���邱�ƂŎ��g�̃��[�J���s����Z�o�ł���
        DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&parent->transform_->GetWorldTransform());
        DirectX::XMMATRIX ParentInverseTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&world);
        DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(WorldTransform, ParentInverseTransform);
        DirectX::XMStoreFloat4x4(&world, LocalTransform);

        TransformUtils::MatrixToTransformation(world, &scale, &rotate, &position);
        selectedObject->transform_->SetScale(scale);
        selectedObject->transform_->SetRotation(rotate);
        selectedObject->transform_->SetLocalPosition(position);
    }
    else {
        selectedObject->transform_->SetScale(scale);
        selectedObject->transform_->SetRotation(rotate);
        selectedObject->transform_->SetWorldPosition(position);
    }

    //�{�[������guizmo
    if (renderSortObject_.size() <= 0)return;
    for (std::weak_ptr<RendererCom>& r : renderSortObject_)
    {
        r.lock()->BoneGuizmo(view, projection);
    }
}

void GameObjectManager::StartUpObjects()
{
    if (startGameObject_.empty())return;

    for (std::shared_ptr<GameObject>& obj : startGameObject_)
    {
        if (!obj)continue;

        //�|�X�g�G�t�F�N�g������Γ���
        std::shared_ptr<PostEffect>posteffectcomp = obj->GetComponent<PostEffect>();
        if (posteffectcomp)
        {
            posteffectobject.emplace_back(posteffectcomp);
        }

        //�����_���[�R���|�[�l���g������΃����_�[�I�u�W�F�ɓ����
        std::shared_ptr<RendererCom> rendererComponent = obj->GetComponent<RendererCom>();
        if (rendererComponent)
        {
            //���f�����X���b�h�ǂݍ���
            rendererComponent->JoinThred();
            rendererComponent->GetModel()->GetResource()->JoinTexTH();

            renderSortObject_.emplace_back(rendererComponent);
        }

        //CPU�p�[�e�B�N���I�u�W�F�N�g������Γ����
        std::shared_ptr<CPUParticle> cpuparticlecomp = obj->GetComponent<CPUParticle>();
        if (cpuparticlecomp)
        {
            cpuparticleobject.emplace_back(cpuparticlecomp);
        }

        //GPU�p�[�e�B�N���I�u�W�F�N�g������Γ���
        std::shared_ptr<GPUParticle>gpuparticlecomp = obj->GetComponent<GPUParticle>();
        if (gpuparticlecomp)
        {
            gpuparticleobject.emplace_back(gpuparticlecomp);
        }

        //�X�v���C�g�I�u�W�F�N�g������Γ���
        std::shared_ptr<Sprite>spritecomp = obj->GetComponent<Sprite>();
        if (spritecomp)
        {
            spriteobject.emplace_back(spritecomp);
        }

        //�C���X�^���X�I�u�W�F�N�g������Γ���
        std::shared_ptr<InstanceRenderer>instancecomp = obj->GetComponent<InstanceRenderer>();
        if (instancecomp)
        {
            instanceobject.emplace_back(instancecomp);
        }

        //�f�J�[���I�u�W�F�N�g������Γ���
        std::shared_ptr<Decal>decalcomp = obj->GetComponent<Decal>();
        if (decalcomp)
        {
            decalobject.emplace_back(decalcomp);
        }

        obj->Start();
        updateGameObject_.emplace_back(obj);

        //�����蔻��R���|�[�l���g�ǉ�
        std::shared_ptr<Collider> colliderComponent = obj->GetComponent<Collider>();
        if (colliderComponent)
        {
            colliderObject_.emplace_back(colliderComponent);
        }

        //�����o������R���|�[�l���g�ǉ�
        std::shared_ptr<PushBackCom> pushBackComponent = obj->GetComponent<PushBackCom>();
        if (pushBackComponent)
        {
            pushBackObject_.emplace_back(pushBackComponent);
        }

        obj->UpdateTransform();
    }
    startGameObject_.clear();

    // �`��I�u�W�F�N�g�̃\�[�g
    SortRenderObject();
    SortInstanceRenderObject();
}

void GameObjectManager::CollideGameObjects()
{
    // ����O�̃N���A
    for (auto& col : colliderObject_)
    {
        col.lock()->ColliderStartClear();
    }

    // ����
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

void GameObjectManager::PushBackGameObjects()
{
    // ����O�̃N���A
    for (auto& pb : pushBackObject_)
    {
        pb.lock()->ResetHitFlg();
    }

    // ����
    for (int pb1 = 0; pb1 < pushBackObject_.size(); ++pb1)
    {
        if (!pushBackObject_[pb1].lock()->GetGameObject()->GetEnabled())continue;
        for (int pb2 = pb1 + 1; pb2 < pushBackObject_.size(); ++pb2)
        {
            if (!pushBackObject_[pb2].lock()->GetGameObject()->GetEnabled())continue;
            pushBackObject_[pb1].lock()->PushBackUpdate(pushBackObject_[pb2].lock());
        }
    }
}

void GameObjectManager::RemoveGameObjects()
{
    // �폜���X�g�ɉ����Ȃ�������A�������Ȃ�
    if (removeGameObject_.empty())return;

    // �폜���X�g�̃I�u�W�F�N�g���폜
    {
        std::vector<std::weak_ptr<GameObject>> parentObj;
        for (const std::shared_ptr<GameObject>& obj : removeGameObject_)
        {
            obj->OnDestroy();
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

        //child���
        for (std::weak_ptr<GameObject> parent : parentObj)
        {
            if (!parent.expired()) {
                parent.lock()->EraseExpiredChild();
            }
        }
    }

    //�e�I�u�W�F�N�g���(�폜)
      //collider���
    for (int col = 0; col < colliderObject_.size(); ++col)
    {
        if (colliderObject_[col].expired())
        {
            colliderObject_.erase(colliderObject_.begin() + col);
            --col;
        }
    }
    //pushback���
    for (int pb = 0; pb < pushBackObject_.size(); ++pb)
    {
        if (pushBackObject_[pb].expired())
        {
            pushBackObject_.erase(pushBackObject_.begin() + pb);
            --pb;
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

    //posteffect���
    for (int pos = 0; pos < posteffectobject.size(); ++pos)
    {
        if (posteffectobject[pos].expired())
        {
            posteffectobject.erase(posteffectobject.begin() + pos);
            --pos;
        }
    }

    //cpuparticleObject���
    for (int per = 0; per < cpuparticleobject.size(); ++per)
    {
        if (cpuparticleobject[per].expired())
        {
            cpuparticleobject.erase(cpuparticleobject.begin() + per);
            --per;
        }
    }

    //gpuparticleobject���
    for (int per = 0; per < gpuparticleobject.size(); ++per)
    {
        if (gpuparticleobject[per].expired())
        {
            gpuparticleobject.erase(gpuparticleobject.begin() + per);
            --per;
        }
    }

    //spriteobject���
    for (int spr = 0; spr < spriteobject.size(); ++spr)
    {
        if (spriteobject[spr].expired())
        {
            spriteobject.erase(spriteobject.begin() + spr);
            --spr;
        }
    }

    //�C���X�^���X���
    for (int ins = 0; ins < instanceobject.size(); ++ins)
    {
        if (instanceobject[ins].expired())
        {
            instanceobject.erase(instanceobject.begin() + ins);
            --ins;
        }
    }

    //�f�J�[�����
    for (int d = 0; d < decalobject.size(); ++d)
    {
        if (decalobject[d].expired())
        {
            decalobject.erase(decalobject.begin() + d);
            --d;
        }
    }

    //�\�[�g
    SortRenderObject();
    SortInstanceRenderObject();
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

void GameObjectManager::SortRenderObject()
{
    //���ʂ̃��f���`��
    std::sort(renderSortObject_.begin(), renderSortObject_.end(),
        [&](std::weak_ptr<RendererCom>& left, std::weak_ptr<RendererCom>& right) {
            return left.lock()->GetShaderMode() < right.lock()->GetShaderMode();
        });

    deferredCount = std::count_if(renderSortObject_.begin(), renderSortObject_.end(),
        [&](std::weak_ptr<RendererCom>& ren) {
            return ren.lock()->GetShaderMode() <= SHADER_ID_MODEL::DEFERRED;
        });

    useDepthCount = std::count_if(renderSortObject_.begin(), renderSortObject_.end(),
        [&](std::weak_ptr<RendererCom>& ren) {
            return ren.lock()->GetShaderMode() == SHADER_ID_MODEL::USE_DEPTH_MAP;
        });
}

//�C���X�^���V���O�̃\�[�g
void GameObjectManager::SortInstanceRenderObject()
{
    std::sort(instanceobject.begin(), instanceobject.end(),
        [&](std::weak_ptr<InstanceRenderer>& left, std::weak_ptr<InstanceRenderer>& right) {
            return left.lock()->GetShaderMode() < right.lock()->GetShaderMode();
        });

    instancedeferredCount = std::count_if(instanceobject.begin(), instanceobject.end(),
        [&](std::weak_ptr<InstanceRenderer>& ren) {
            return ren.lock()->GetShaderMode() == SHADER_ID_MODEL::DEFERRED;
        });

    instanceuseDepthCount = std::count_if(instanceobject.begin(), instanceobject.end(),
        [&](std::weak_ptr<InstanceRenderer>& ren) {
            return ren.lock()->GetShaderMode() == SHADER_ID_MODEL::USE_DEPTH_MAP;
        });
}

//3D�`��
void GameObjectManager::RenderDeferred()
{
    if (renderSortObject_.size() <= 0)return;
    if (deferredCount == 0)return;

    int drawCount = 0;
    for (std::weak_ptr<RendererCom>& renderObj : renderSortObject_)
    {
        // �f�t�@�[�h�����_�����O�̃��f���̂ݕ`�悷��
        if (drawCount == deferredCount)return;
        drawCount++;

        if (!renderObj.lock()->GetGameObject()->GetEnabled())continue;
        if (!renderObj.lock()->GetEnabled())continue;

        Model* model = renderObj.lock()->GetModel();
        if (model != nullptr)
        {
            renderObj.lock()->Render();
        }
    }
}

void GameObjectManager::InstanceRenderDeferred()
{
    if (instanceobject.size() <= 0)return;
    if (instancedeferredCount == 0)return;

    int drawCount = 0;
    for (std::weak_ptr<InstanceRenderer>& renderObj : instanceobject)
    {
        // �f�t�@�[�h�����_�����O�̃��f���̂ݕ`�悷��
        if (drawCount == instancedeferredCount)return;
        drawCount++;

        if (!renderObj.lock()->GetGameObject()->GetEnabled())continue;
        if (!renderObj.lock()->GetEnabled())continue;

        Model* model = renderObj.lock()->GetModel();
        if (model != nullptr)
        {
            renderObj.lock()->Render();
        }
    }
}

void GameObjectManager::RenderForward()
{
    if (renderSortObject_.size() <= 0)return;

    // �t�H���[�h�����_�����O����I�u�W�F�N�g�̐�
    int drawVolume = renderSortObject_.size() - deferredCount - useDepthCount;
    if (drawVolume <= 0)return;

    for (int i = 0; i < drawVolume; ++i)
    {
        int index = deferredCount + i;

        if (!renderSortObject_[index].lock()->GetGameObject()->GetEnabled())continue;
        if (!renderSortObject_[index].lock()->GetEnabled())continue;

        Model* model = renderSortObject_[index].lock()->GetModel();
        if (model != nullptr)
        {
            renderSortObject_[index].lock()->Render();
        }
    }
}

void GameObjectManager::InstanceRenderForward()
{
    if (instanceobject.size() <= 0)return;

    // �t�H���[�h�����_�����O����I�u�W�F�N�g�̐�
    int drawVolume = instanceobject.size() - instancedeferredCount - instanceuseDepthCount;
    if (drawVolume <= 0)return;

    for (int i = 0; i < drawVolume; ++i)
    {
        int index = instancedeferredCount + i;

        if (!instanceobject[index].lock()->GetGameObject()->GetEnabled())continue;
        if (!instanceobject[index].lock()->GetEnabled())continue;

        Model* model = instanceobject[index].lock()->GetModel();
        if (model != nullptr)
        {
            instanceobject[index].lock()->Render();
        }
    }
}

//�e�`��
void GameObjectManager::RenderShadow()
{
    if (renderSortObject_.size() <= 0)return;

    //�e�`��
    for (std::weak_ptr<RendererCom>& modelrender : renderSortObject_)
    {
        if (!modelrender.lock()->GetGameObject()->GetEnabled())continue;
        if (!modelrender.lock()->GetEnabled())continue;

        Model* model = modelrender.lock()->GetModel();
        if (model != nullptr)
        {
            modelrender.lock()->ShadowRender();
        }
    }
}

void GameObjectManager::InstanceRenderShadow()
{
    if (instanceobject.size() <= 0)return;

    //�e�`��
    for (std::weak_ptr<InstanceRenderer>& modelrender : instanceobject)
    {
        if (!modelrender.lock()->GetGameObject()->GetEnabled())continue;
        if (!modelrender.lock()->GetEnabled())continue;

        Model* model = modelrender.lock()->GetModel();
        if (model != nullptr)
        {
            modelrender.lock()->ShadowRender();
        }
    }
}

void GameObjectManager::RenderUseDepth()
{
    if (renderSortObject_.size() <= 0) return;

    // �t�H���[�h�����_�����O����I�u�W�F�N�g�̐�
    int drawVolume = useDepthCount;
    if (drawVolume <= 0) return;

    for (int i = 0; i < drawVolume; ++i)
    {
        int index = i + renderSortObject_.size() - useDepthCount;

        if (!renderSortObject_[index].lock()->GetGameObject()->GetEnabled())continue;
        if (!renderSortObject_[index].lock()->GetEnabled())continue;

        Model* model = renderSortObject_[index].lock()->GetModel();
        if (model != nullptr)
        {
            renderSortObject_[index].lock()->Render();
        }
    }
}

void GameObjectManager::InstanceRenderUseDepth()
{
    if (instanceobject.size() <= 0) return;

    // �t�H���[�h�����_�����O����I�u�W�F�N�g�̐�
    int drawVolume = instanceuseDepthCount;
    if (drawVolume <= 0) return;

    for (int i = 0; i < drawVolume; ++i)
    {
        int index = i + instanceobject.size() - instanceuseDepthCount;

        if (!instanceobject[index].lock()->GetGameObject()->GetEnabled())continue;
        if (!instanceobject[index].lock()->GetEnabled())continue;

        Model* model = instanceobject[index].lock()->GetModel();
        if (model != nullptr)
        {
            instanceobject[index].lock()->Render();
        }
    }
}

//�|�X�g�G�t�F�N�g�̏��擾
std::vector<std::shared_ptr<PostEffect>> GameObjectManager::GetActivePostEffects()
{
    std::vector<std::shared_ptr<PostEffect>> activeEffects;
    for (auto& po : posteffectobject)
    {
        if (auto effect = po.lock())
        {
            if (effect->GetGameObject()->GetEnabled() && effect->GetEnabled())
            {
                activeEffects.push_back(effect);
            }
        }
    }
    return activeEffects;
}

//CPU�p�[�e�B�N���`��
void GameObjectManager::CPUParticleRender()
{
    if (cpuparticleobject.size() <= 0)return;

    for (std::weak_ptr<CPUParticle>& po : cpuparticleobject)
    {
        if (!po.lock()->GetGameObject()->GetEnabled())continue;
        if (!po.lock()->GetEnabled())continue;

        po.lock()->Render();
    }
}

//GPU�p�[�e�B�N���`��
void GameObjectManager::GPUParticleRender()
{
    if (gpuparticleobject.size() <= 0)return;

    for (std::weak_ptr<GPUParticle>& po : gpuparticleobject)
    {
        if (!po.lock()->GetGameObject()->GetEnabled())continue;
        if (!po.lock()->GetEnabled())continue;

        po.lock()->Render();
    }
}

//�f�J�[���`��
void GameObjectManager::DecalRender()
{
    if (decalobject.size() <= 0)return;

    for (std::weak_ptr<Decal>& d : decalobject)
    {
        if (!d.lock()->GetGameObject()->GetEnabled())continue;
        if (!d.lock()->GetEnabled())continue;

        d.lock()->Render();
    }
}

//�X�v���C�g�`��
void GameObjectManager::SpriteRender(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    if (spriteobject.size() <= 0)return;

    for (std::weak_ptr<Sprite>& sp : spriteobject)
    {
        if (!sp.lock()->GetGameObject()->GetEnabled())continue;
        if (!sp.lock()->GetEnabled())continue;

        if (IsParentEnable(sp.lock()->GetGameObject()))
            sp.lock()->Render(view, projection);
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