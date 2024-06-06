#pragma once

#include <memory>
#include <vector>
#include <set>
#include <string>
#include <DirectXMath.h>
#include <future>
#include <imgui.h>
#include <ImGuizmo.h>

// �O���錾
class Component;
class TransformCom;
class RendererCom;
class ParticleSystemCom;
class Collider;

// �Q�[���I�u�W�F�N�g
class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
    GameObject() {  }
    virtual ~GameObject() {};

    // �J�n����
    virtual void Start();

    // �X�V����
    virtual void Update(float elapsedTime);

    // �s��̍X�V
    virtual void UpdateTransform();

    // GUI�\��
    virtual void OnGUI();

    //sprite�`��p
    virtual void Render2D(float elapsedTime);

    // ���O�̐ݒ�
    void SetName(const char* name) { this->name_ = name; }

    // ���O�̎擾
    const char* GetName() const { return name_.c_str(); }

    // �R���|�[�l���g�ǉ�
    template<class T, class... Args>
    std::shared_ptr<T> AddComponent(Args... args)
    {
        std::shared_ptr<T> component = std::make_shared<T>(args...);
        component->SetGameObject(shared_from_this());

        //transform�̏ꍇ�͕ێ�����
        if (std::strcmp(component->GetName(), "Transform") == 0)
            transform_ = std::dynamic_pointer_cast<TransformCom>(component);

        components_.emplace_back(component);
        return component;
    }

    // �R���|�[�l���g�擾
    template<class T>
    std::shared_ptr<T> GetComponent()
    {
        for (std::shared_ptr<Component>& component : components_)
        {
            std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(component);
            if (p == nullptr) continue;
            return p;
        }
        return nullptr;
    }

    //�e�q
    //�ǉ�
    std::shared_ptr<GameObject> AddChildObject();

    //�e�擾
    std::shared_ptr<GameObject> GetParent() { return parentObject_.lock(); }

    //�q���B�擾 (weak_ptr�Ȃ̂Œ���)
    std::vector<std::weak_ptr<GameObject>> GetChildren() { return childrenObject_; }
    std::shared_ptr<GameObject> GetChildFind(const char* name) {
        for (auto& child : childrenObject_)
        {
            if (std::strcmp(name, child.lock()->GetName()) == 0)
                return child.lock();
        }
        return nullptr;
    }

    //�q��������؂�̃|�C���^�폜
    void EraseExpiredChild();

    const bool GetEnabled() const { return isEnabled_; }
    void SetEnabled(bool enabled) { isEnabled_ = enabled; }

    const float GetObjSpeed() const { return objSpeed_; }
    void SetObjSpeed(float speed) { objSpeed_ = speed; }

    //�����
    void AudioRelease();

public:
    std::shared_ptr<TransformCom> transform_;

private:
    std::string	name_;

    std::vector<std::shared_ptr<Component>>	components_;

    //�e�q
    std::weak_ptr<GameObject> parentObject_;
    std::vector<std::weak_ptr<GameObject>> childrenObject_;

    //�L����
    bool isEnabled_ = true;

    //�I�u�W�F�N�g�X�s�[�h�i�X���[�ɂł���j
    float objSpeed_ = 1;
};

// �Q�[���I�u�W�F�N�g�}�l�[�W���[
class GameObjectManager
{
private:
    GameObjectManager() {}
    ~GameObjectManager() {}

public:
    // �C���X�^���X�擾
    static GameObjectManager& Instance()
    {
        static GameObjectManager instance;
        return instance;
    }

    // �쐬
    std::shared_ptr<GameObject> Create();

    // �폜
    void Remove(std::shared_ptr<GameObject> obj);
    // �S�폜
    void AllRemove();

    // �X�V
    void Update(float elapsedTime);

    // �s��X�V
    void UpdateTransform();

    // �`��
    void Render();

    //sprite�`��
    void Render2D(float elapsedTime);

    //�Q�[���I�u�W�F�N�g��T��
    std::shared_ptr<GameObject> Find(const char* name);

    //�V�[���Q�[�����o�҂��t���O
    bool GetIsSceneGameStart() { return isSceneGameStart_; }
    void SetIsSceneGameStart(bool flag) { isSceneGameStart_ = flag; }

    //imguiguizmo
    void DrawGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

private:
    void DrawLister();
    void DrawDetail();

    //3D�`��
    void Render3D();

    //�p�[�e�B�N���`��
    void ParticleRender();

    //�I�u�W�F�N�g���
    void EraseObject(std::vector<std::shared_ptr<GameObject>>& objs, std::shared_ptr<GameObject> removeObj);

private:
    std::vector<std::shared_ptr<GameObject>>		startGameObject_;
    std::vector<std::shared_ptr<GameObject>>		updateGameObject_;
    std::set<std::shared_ptr<GameObject>>		selectionGameObject_;
    std::set<std::shared_ptr<GameObject>>		removeGameObject_;

    //imguiguizmo
    ImGuizmo::OPERATION m_guizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE m_guizmoMode = ImGuizmo::LOCAL;

    std::vector<std::weak_ptr<Collider>>	colliderObject_;

    //�`�揇�Ɋi�[����
    std::vector<std::weak_ptr<RendererCom>>   renderSortObject_;

    //�p�[�e�B�N���`��p
    std::vector<std::weak_ptr<ParticleSystemCom>>   particleObject_;

    bool					isHiddenLister_ = false;
    bool					isHiddenDetail_ = false;

    //���o�҂��t���O�i�V�[���Q�[���j
    bool isSceneGameStart_ = false;

    //�X���b�h�p
    std::vector<std::future<void>> future;

    std::mutex mutex_;
};