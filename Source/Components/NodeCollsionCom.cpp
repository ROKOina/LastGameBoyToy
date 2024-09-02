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
    // �Փ˃p�����[�^�̍����������v�Z
    for (auto& cp : model->cp)
    {
        DirectX::XMFLOAT3 pos =
        {
            model->GetNodes()[cp.nodeid].worldTransform._41,
            model->GetNodes()[cp.nodeid].worldTransform._42,
            model->GetNodes()[cp.nodeid].worldTransform._43
        };

        DirectX::XMFLOAT3 endpos = { 0.0f, 0.0f, 0.0f };
        if (cp.endnodeid >= 0 && cp.endnodeid < model->GetNodes().size())
        {
            endpos =
            {
                model->GetNodes()[cp.endnodeid].worldTransform._41,
                model->GetNodes()[cp.endnodeid].worldTransform._42,
                model->GetNodes()[cp.endnodeid].worldTransform._43
            };
        }

        // `pos` �� `endpos` �܂ł̋������v�Z
        DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);
        DirectX::XMVECTOR endposVec = DirectX::XMLoadFloat3(&endpos);
        DirectX::XMVECTOR distanceVec = DirectX::XMVectorSubtract(endposVec, posVec);
        float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(distanceVec));

        // ������ݒ�
        cp.height = distance;

        //�f�o�b�O�v���~�e�B�u�`��
        if (debugrender)
        {
            // ��Ƀf�o�b�O�v���~�e�B�u��`��
            switch (static_cast<CollsionType>(cp.collsiontype))
            {
            case NodeCollsionCom::CollsionType::SPHER:
                // debugprimitive�`��
                Graphics::Instance().GetDebugRenderer()->DrawSphere(pos, cp.radius, { 1, 0, 0, 1 });
                break;

            case NodeCollsionCom::CollsionType::CYLINDER:
                // debugprimitive�`��
                Graphics::Instance().GetDebugRenderer()->DrawCylinder(pos, endpos, cp.radius, cp.height, { 1, 0, 0, 1 });
                break;

            default:
                break;
            }
        }
    }
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

        // ��Ƀf�o�b�O�v���~�e�B�u��`�悷�邽�߂ɁA�S�Ă̏Փ˃p�����[�^���`�F�b�N
        for (const auto& cp : model->cp)
        {
            DirectX::XMFLOAT3 pos =
            {
                model->GetNodes()[cp.nodeid].worldTransform._41,
                model->GetNodes()[cp.nodeid].worldTransform._42,
                model->GetNodes()[cp.nodeid].worldTransform._43
            };

            DirectX::XMFLOAT3 endpos = { 0.0f, 0.0f, 0.0f };
            if (cp.endnodeid >= 0 && cp.endnodeid < model->GetNodes().size())
            {
                endpos =
                {
                    model->GetNodes()[cp.endnodeid].worldTransform._41,
                    model->GetNodes()[cp.endnodeid].worldTransform._42,
                    model->GetNodes()[cp.endnodeid].worldTransform._43
                };
            }
        }

        ImGui::Checkbox((char*)u8"�f�o�b�O�`��", &debugrender);

        // �Փ˃p�����[�^��\��
        for (size_t i = 0; i < model->cp.size(); ++i)
        {
            auto& cp = model->cp[i];

            if (model->selectionNode != nullptr && cp.nodeid == model->selectionNode->nodeIndex)
            {
                ImGui::PushID(static_cast<int>(i));

                // �R���W�����̌`��ŏo��imgui�����߂�
                const char* collisionTypeNames[] = { "Sphere", "Cylinder" };
                int collisionTypeIndex = static_cast<int>(cp.collsiontype);
                if (ImGui::Combo("Collision Type", &collisionTypeIndex, collisionTypeNames, IM_ARRAYSIZE(collisionTypeNames)))
                {
                    cp.collsiontype = static_cast<int>(collisionTypeIndex);
                }

                switch (static_cast<CollsionType>(cp.collsiontype))
                {
                case NodeCollsionCom::CollsionType::SPHER:
                    ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 5.0f);
                    break;

                case NodeCollsionCom::CollsionType::CYLINDER:

                    // �m�[�h�I��p�̃��X�g��\��
                    if (model->selectionNode != nullptr)
                    {
                        if (ImGui::BeginCombo("EndNode Selection", "Select a Node"))
                        {
                            for (const auto& node : model->GetNodes())
                            {
                                bool isSelected = (node.nodeIndex == model->selectionNode->nodeIndex);
                                if (ImGui::Selectable(node.name, isSelected))
                                {
                                    // �m�[�h����I������endnodeid�ɐݒ�
                                    for (auto& cp : model->cp)
                                    {
                                        if (cp.nodeid == model->selectionNode->nodeIndex)
                                        {
                                            cp.endnodeid = node.nodeIndex;
                                            break;
                                        }
                                    }
                                }
                                if (isSelected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }

                    // �m�[�h�I���̂��߂̃h���b�v�_�E�����j���[
                    if (ImGui::BeginCombo("EndNodeChilderen", cp.endnodeid >= 0 ? model->GetNodes()[cp.endnodeid].name : "None"))
                    {
                        std::vector<Model::Node*> descendants;
                        model->GetAllDescendants(model->selectionNode->nodeIndex, descendants);

                        for (auto& node : descendants)
                        {
                            bool isSelected = (cp.endnodeid == node->nodeIndex);
                            if (ImGui::Selectable(node->name, isSelected))
                            {
                                cp.endnodeid = node->nodeIndex;
                            }
                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    // �Փ˃p�����[�^�̐ݒ�
                    ImGui::DragFloat("Radius", &cp.radius, 0.1f, 0.0f, 5.0f);
                    break;

                default:
                    break;
                }

                // �폜�{�^��
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
            bool hasCollisionParameter = false;

            // ���ɑI�����ꂽ�m�[�h�ɏՓ˃p�����[�^�����݂��邩�m�F
            for (const auto& cp : model->cp)
            {
                if (cp.nodeid == selectedNodeId)
                {
                    hasCollisionParameter = true;
                    break;
                }
            }

            if (!hasCollisionParameter)
            {
                if (ImGui::Button("Create"))
                {
                    Model::CollsionParameter newParam;
                    newParam.nodeid = selectedNodeId;
                    newParam.endnodeid = -1; // ������Ԃł͖����Ȓl��ݒ�
                    newParam.collsiontype = static_cast<int>(CollsionType::SPHER); // �����l��SPHER�ɐݒ�
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