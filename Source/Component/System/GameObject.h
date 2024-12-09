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
class CPUParticle;
class GPUParticle;
class InstanceRenderer;
class Collider;
class PushBackCom;
class Sprite;
class Decal;
class PostEffect;
class Trail;

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

    // �Q�[���I�u�W�F�N�g�̔j���̍ۂɌĂ΂��
    void OnDestroy();

    // �s��̍X�V
    virtual void UpdateTransform();

    // GUI�\��
    virtual void OnGUI();

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
    void AddChildObject(std::shared_ptr<GameObject> obj) { childrenObject_.emplace_back(obj); obj->parentObject_ = shared_from_this(); };

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
using GameObj = std::shared_ptr<GameObject>;

// �Q�[���I�u�W�F�N�g�}�l�[�W���[
class GameObjectManager
{
    friend class GameObject;
private:
    GameObjectManager() { }
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
    // ���쐬�i�댯�o����Ύg��Ȃ��������������j
    std::shared_ptr<GameObject> CreateNowTime();    //�I�u�W�F�N�g��Ԃ������@��������g�����牺�̊֐��ĂԂ܂ŕʂ̃I�u�W�F�N�g�����Ȃ��ŉ������i����ŕԂ����I�u�W�F�N�g�̎q���͉��̊֐��ĂԑO�ɓo�^���Ă����j
    void CreateNowTimeSaveComponent(std::shared_ptr<GameObject> obj);   //�I�u�W�F�N�g�ɃR���|�[�l���g���ꂽ��Ă�

    // �폜
    void Remove(std::shared_ptr<GameObject> obj);
    //���폜�i�댯�o����Ύg��Ȃ��������������j
    void RemoveNowTime(std::weak_ptr<GameObject> obj);
    // �S�폜
    void AllRemove();

    // �X�V
    void Update(float elapsedTime);

    // �s��X�V
    void UpdateTransform();

    // �`��
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& lightdirection);

    //�Q�[���I�u�W�F�N�g��T��
    std::shared_ptr<GameObject> Find(const char* name);

    //�V�[���Q�[�����o�҂��t���O
    bool GetIsSceneGameStart() { return isSceneGameStart_; }
    void SetIsSceneGameStart(bool flag) { isSceneGameStart_ = flag; }

    //imguiguizmo
    void DrawGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    void RemoveGameObjects();
private:
    void StartUpObjects();
    //�ۑ��R���|�[�l���g��ǉ�
    void StartUpSaveComponent(std::shared_ptr<GameObject> obj);

    void CollideGameObjects();  //������
    void PushBackGameObjects(); //�����o��

    void DrawLister();
    void DrawDetail();

    void SortRenderObject();
    void SortInstanceRenderObject();

    //3D�`��
    void RenderDeferred();
    void InstanceRenderDeferred();
    void RenderForward();
    void InstanceRenderForward();
    void RenderShadow();
    void InstanceRenderShadow();
    void RenderUseDepth();
    void InstanceRenderUseDepth();

    //�|�X�g�G�t�F�N�g�̏��擾
    std::vector<std::shared_ptr<PostEffect>> GetActivePostEffects();

    //CPU�p�[�e�B�N���`��
    void CPUParticleRender();

    //GPU�p�[�e�B�N���`��
    void GPUParticleRender();

    //�f�J�[���`��
    void DecalRender();

    //�g���C���`��
    void TrailRender();

    //�X�v���C�g�`��
    void SpriteRender(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //�I�u�W�F�N�g���
    void EraseObject(std::vector<std::shared_ptr<GameObject>>& objs, std::shared_ptr<GameObject> removeObj);

    //�ۑ��R���|�[�l���gnull���
    void EraseComponet();

private:
    std::vector<std::shared_ptr<GameObject>>		startGameObject_;
    std::vector<std::shared_ptr<GameObject>>		updateGameObject_;
    std::set<std::shared_ptr<GameObject>>		selectionGameObject_;
    std::set<std::shared_ptr<GameObject>>		removeGameObject_;

    //��OBJ�쐬�p
    std::vector<std::shared_ptr<GameObject>>		creatNowTimeGameChildObject_;   ////��OBJ�쐬���N��������̎q�I�u�W�F�N�g�͂����ɒǉ������
    bool isCreatNowTimeObj = false; //��OBJ�쐬���N��������true�ɂȂ�i�q���������邽�߁j

    //imguiguizmo
    ImGuizmo::OPERATION m_guizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE m_guizmoMode = ImGuizmo::LOCAL;

    std::vector<std::weak_ptr<Collider>>	colliderObject_;
    std::vector<std::weak_ptr<PushBackCom>>	pushBackObject_;

    //�`�揇�Ɋi�[����
    std::vector<std::weak_ptr<RendererCom>>   renderSortObject_;
    // �f�t�@�[�h�`��I�u�W�F�N�g�̐�
    int deferredCount = -1;
    // �[�x�}�b�v���g�p����`��I�u�W�F�N�g�̐�
    int useDepthCount = -1;

    //CPU�p�[�e�B�N���`��p
    std::vector<std::weak_ptr<CPUParticle>> cpuparticleobject;

    //GPUparticle�`��
    std::vector<std::weak_ptr<GPUParticle>>gpuparticleobject;

    //�f�J�[���`��
    std::vector<std::weak_ptr<Decal>> decalobject;

    //�g���C���`��
    std::vector<std::weak_ptr<Trail>>trailobject;

    //�|�X�g�G�t�F�N�g�`��
    std::vector<std::weak_ptr<PostEffect>>posteffectobject;

    //�C���X�^���X�`��
    std::vector<std::weak_ptr<InstanceRenderer>>instanceobject;
    // �f�t�@�[�h�`��I�u�W�F�N�g�̐�
    int instancedeferredCount = -1;
    // �[�x�}�b�v���g�p����`��I�u�W�F�N�g�̐�
    int instanceuseDepthCount = -1;

    //�X�v���C�g�`��
    std::vector<std::weak_ptr<Sprite>>spriteobject;

    bool					isHiddenLister_ = false;
    bool					isHiddenDetail_ = false;

    //���o�҂��t���O�i�V�[���Q�[���j
    bool isSceneGameStart_ = false;

    std::mutex mutex_;
};
