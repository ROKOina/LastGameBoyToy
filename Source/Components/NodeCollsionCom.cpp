#include "NodeCollsionCom.h"
#include "RendererCom.h"
#include <SystemStruct/TransformUtils.h>

//�R���X�g���N�^
NodeCollsionCom::NodeCollsionCom(const char* filename)
{
    m_filename = filename;
}

//�J�n����
void NodeCollsionCom::Start()
{
    // ���f�����烊�\�[�X���擾
    model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    if (m_filename != nullptr && model != nullptr)
    {
        model->Deserialize(m_filename);
    }
}

//�X�V����
void NodeCollsionCom::Update(float elapsedTime)
{
}

//GUI�`��
void NodeCollsionCom::OnGUI()
{
    static std::unordered_map<int, bool> nodeCreationState;

    if (model != nullptr)
    {
        // ���f���m�[�h��ImGui�ŕ\��
        for (Model::Node& node : model->GetNodes())
        {
            if (node.parent == nullptr)
            {
                model->ImGui(&node);
            }
        }

        // �Փ˃p�����[�^��\��
        for (size_t i = 0; i < model->cp.size(); ++i)
        {
            auto& cp = model->cp[i];
            DirectX::XMFLOAT3 pos = {
                model->GetNodes()[cp.nodeid].worldTransform._41,
                model->GetNodes()[cp.nodeid].worldTransform._42,
                model->GetNodes()[cp.nodeid].worldTransform._43
            };
            Graphics::Instance().GetDebugRenderer()->DrawSphere(pos, cp.radius, { 1, 0, 0, 1 });

            if (model->selectionNode != nullptr && cp.nodeid == model->selectionNode->nodeIndex)
            {
                ImGui::PushID(static_cast<int>(i));
                ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 2.0f);
                if (ImGui::Button("Delete"))
                {
                    model->cp.erase(model->cp.begin() + i);
                    // �m�[�h���폜���ꂽ�̂ŁA�ēxCreate�\�ɂ���
                    nodeCreationState[cp.nodeid] = false;
                    ImGui::PopID();
                    break; // �폜�����烋�[�v���I��
                }
                ImGui::PopID();
            }
        }

        // �V�����Փ˃p�����[�^���쐬
        if (model->selectionNode != nullptr)
        {
            int selectedNodeId = model->selectionNode->nodeIndex;
            if (nodeCreationState.find(selectedNodeId) == nodeCreationState.end() || !nodeCreationState[selectedNodeId])
            {
                if (ImGui::Button("Create"))
                {
                    Model::CollsionParameter newParam;
                    newParam.nodeid = selectedNodeId;
                    model->cp.emplace_back(newParam);
                    nodeCreationState[selectedNodeId] = true; // �쐬�ς݂Ƃ��ċL�^
                }
            }
            else
            {
                ImGui::Text((char*)u8"���̃m�[�h�ɂ͊��ɏՓ˃p�����[�^���쐬����Ă��܂�");
            }
        }

        // �ۑ�����ѓǂݍ��݃{�^��
        if (ImGui::Button("Save"))
        {
            model->Serialize();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            model->LoadDesirialize();
        }
    }
}