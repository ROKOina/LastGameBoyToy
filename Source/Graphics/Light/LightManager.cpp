#include "LightManager.h"
#include "Graphics/Graphics.h"
#include "Misc.h"
#include <imgui.h>

//�R���X�g���N�^
LightManager::LightManager()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // �萔�o�b�t�@�̍쐬
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(LightCB);
    desc.StructureByteStride = 0;

    //�R���X�^���g�o�b�t�@�̍쐬
    HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, 0, m_lightCb.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    // �萔�o�b�t�@�ݒ�
    dc->VSSetConstantBuffers(8, 1, m_lightCb.GetAddressOf());
    dc->PSSetConstantBuffers(8, 1, m_lightCb.GetAddressOf());
    dc->CSSetConstantBuffers(8, 1, m_lightCb.GetAddressOf());
    dc->GSSetConstantBuffers(8, 1, m_lightCb.GetAddressOf());
    dc->HSSetConstantBuffers(8, 1, m_lightCb.GetAddressOf());
    dc->DSSetConstantBuffers(8, 1, m_lightCb.GetAddressOf());
}

//�f�X�g���N�^
LightManager::~LightManager()
{
    Clear();
}

// ���C�g��o�^����
void LightManager::Register(Light* light)
{
    m_lights.emplace_back(light);
}

// ���C�g�̓o�^����������
void LightManager::Remove(Light* light)
{
    std::vector<Light*>::iterator	it = std::find(m_lights.begin(), m_lights.end(), light);
    if (it != m_lights.end())
    {
        m_lights.erase(it);
        delete	light;
    }
}

// �o�^�ς݂̃��C�g��S�폜����
void LightManager::Clear()
{
    for (Light* light : m_lights)
    {
        delete	light;
    }
    m_lights.clear();
}

//�R���X�^���g�o�b�t�@�̍X�V
void LightManager::UpdateConstatBuffer()
{
    LightCB cb = {};

    // �o�^����Ă�������̏���ݒ�
    for (Light* light : m_lights)
    {
        light->PushRenderContext(cb);
    }

    Graphics::Instance().GetDeviceContext()->UpdateSubresource(m_lightCb.Get(), 0, 0, &cb, 0, 0);
}

// �f�o�b�O���̕\��
void LightManager::DrawDebugGUI()
{
    if (ImGui::TreeNode("Lights"))
    {
        int nodeId = 0;
        for (Light* light : m_lights)
        {
            ImGui::PushID(nodeId++);
            light->DrawDebugGUI();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

// �f�o�b�O�v���~�e�B�u�̕\��
void LightManager::DrawDebugPrimitive()
{
    for (Light* light : m_lights)
    {
        light->DrawDebugPrimitive();
    }
}