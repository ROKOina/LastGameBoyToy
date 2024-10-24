#include "GPUParticle.h"
#include "Graphics/Graphics.h"
#include "Misc.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/Texture.h"
#include "GameSource/Math/Mathf.h"
#include "Dialog.h"
#include "Logger.h"
#include "Components/TransformCom.h"
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

CEREAL_CLASS_VERSION(GPUParticle::SaveParameter, 1)
CEREAL_CLASS_VERSION(GPUParticle::GPUparticleSaveConstants, 1)

// �V���A���C�Y
namespace DirectX
{
    template<class Archive>
    void serialize(Archive& archive, XMUINT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

template<class Archive>
void GPUParticle::GPUparticleSaveConstants::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(emitTime),
        CEREAL_NVP(lifeTime),
        CEREAL_NVP(stretchFlag),
        CEREAL_NVP(isLoopFlg),

        CEREAL_NVP(shape),

        CEREAL_NVP(baseColor),
        CEREAL_NVP(lifeStartColor),
        CEREAL_NVP(lifeEndColor),
        CEREAL_NVP(emitStartColor),
        CEREAL_NVP(emitEndColor),
        CEREAL_NVP(colorVariateByLife),
        CEREAL_NVP(colorScale),

        CEREAL_NVP(emitVec),
        CEREAL_NVP(spiralSpeed),
        CEREAL_NVP(orbitalVelocity),
        CEREAL_NVP(spiralstrong),
        CEREAL_NVP(veloRandScale),
        CEREAL_NVP(speed),
        CEREAL_NVP(emitStartSpeed),
        CEREAL_NVP(emitEndSpeed),

        CEREAL_NVP(scale),
        CEREAL_NVP(scaleVariateByLife),
        CEREAL_NVP(lifeStartSize),
        CEREAL_NVP(lifeEndSize),
        CEREAL_NVP(emitStartSize),
        CEREAL_NVP(emitEndSize),

        CEREAL_NVP(radial),
        CEREAL_NVP(buoyancy),
        CEREAL_NVP(emitStartGravity),
        CEREAL_NVP(emitEndGravity),

        CEREAL_NVP(strechscale),
        CEREAL_NVP(padding)
    );
}

template<class Archive>
void GPUParticle::SaveParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(m_blend),
        CEREAL_NVP(m_depthS),
        CEREAL_NVP(m_textureName)
    );
}

//���{��ɂ���}�N��
#define J(x) reinterpret_cast<const char*>(x)

UINT align(UINT num, UINT alignment)
{
    return (num + (alignment - 1)) & ~(alignment - 1);
}

//�R���X�g���N�^
GPUParticle::GPUParticle(const char* filename, size_t maxparticle) :m_maxparticle(maxparticle)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11Device* device = graphics.GetDevice();

    //�p�[�e�B�N���̍\���̂��o�b�t�@�Ƃ��Ċi�[
    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(EntityParticle) * maxparticle);
    buffer_desc.StructureByteStride = sizeof(EntityParticle);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    hr = device->CreateBuffer(&buffer_desc, NULL, m_particlebuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //�V�F�[�_�[���\�[�X�Ƃ��ĉ摜�ɂ��Ċi�[
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shader_resource_view_desc.Buffer.ElementOffset = 0;
    shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(maxparticle);
    hr = device->CreateShaderResourceView(m_particlebuffer.Get(), &shader_resource_view_desc, m_particlesrv.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //�r���[�쐬
    D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc{};
    unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    unordered_access_view_desc.Buffer.FirstElement = 0;
    unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(maxparticle);
    unordered_access_view_desc.Buffer.Flags = 0;
    hr = device->CreateUnorderedAccessView(m_particlebuffer.Get(), &unordered_access_view_desc, m_particleuav.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //�V�F�[�_�[�ǂݍ���
    CreateVsFromCso(device, "Shader\\GPUParticleVS.cso", m_vertexshader.ReleaseAndGetAddressOf(), NULL, NULL, 0);
    CreatePsFromCso(device, "Shader\\GPUParticlePS.cso", m_pixelshader.ReleaseAndGetAddressOf());
    CreateGsFromCso(device, "Shader\\GPUParticleGS.cso", m_geometryshader.ReleaseAndGetAddressOf());
    CreateCsFromCso(device, "Shader\\GPUParticleCS.cso", m_updatecomputeshader.ReleaseAndGetAddressOf());
    CreateCsFromCso(device, "Shader\\GPUParticleInitializeCS.cso", m_initialzecomputeshader.ReleaseAndGetAddressOf());

    //�R���X�^���g�o�b�t�@�̒�`�ƍX�V
    m_gpu = std::make_unique<ConstantBuffer<GPUParticleConstants>>(device);

    //�R���X�^���g�o�b�t�@�̃o�b�t�@�쐬�A�X�V
    buffer_desc.ByteWidth = sizeof(GPUparticleSaveConstants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, m_constantbuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //�t�@�C���ǂݍ��ݏ���
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    if (filename)
    {
        Desirialize(filename);
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
}

//�X�V����
void GPUParticle::Update(float elapsedTime)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // �I�u�W�F�N�g�̉�]�ɍ��킹�ăG�t�F�N�g����]�����鏈��
    DirectX::XMMATRIX transf = DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform());
    DirectX::XMVECTOR velo = DirectX::XMLoadFloat3(&m_GSC.emitVec);
    velo = DirectX::XMVector3TransformNormal(velo, transf);
    DirectX::XMStoreFloat3(&m_gpu->data.currentEmitVec, velo);

    //��~����
    if (stopFlg == true)return;

    // �P���Đ�
    if (m_GSC.isLoopFlg == false)
    {
        emitTimer += elapsedTime;

        if (emitTimer > m_GSC.emitTime)
        {
            m_gpu->data.isEmitFlg = false;
        }
    }

    //�R���X�^���g�o�b�t�@�̍X�V
    m_gpu->data.position = GetGameObject()->transform_->GetWorldPosition();
    m_gpu->data.rotation = GetGameObject()->transform_->GetRotation();
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());

    //�X�V����R���s���[�g�V�F�[�_�[���Z�b�g����
    dc->CSSetUnorderedAccessViews(0, 1, m_particleuav.GetAddressOf(), NULL);
    dc->CSSetShader(m_updatecomputeshader.Get(), NULL, 0);
    const UINT thread_group_count_x = align(static_cast<UINT>(m_maxparticle), THREAD) / THREAD;
    dc->Dispatch(thread_group_count_x, 1, 1);
    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);
}

//�`��
void GPUParticle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //�u�����h�X�e�[�g�ݒ�
    const float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    dc->OMSetBlendState(graphics.GetBlendState(static_cast<BLENDSTATE>(m_p.m_blend)), BlendFactor, 0xFFFFFFFF);

    //���X�^���C�U�̐ݒ�
    dc->RSSetState(graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    //�摜��n��
    dc->PSSetShaderResources(20, 1, m_colormap.GetAddressOf());

    //�f�v�X�X�e���V���X�e�[�g�ݒ�
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(static_cast<DEPTHSTATE>(m_p.m_depthS)), 0);

    //�v���~�e�B�u�g�|���W�[�ݒ�
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    //�V�F�[�_�[�Z�b�g
    dc->VSSetShader(m_vertexshader.Get(), NULL, 0);
    dc->PSSetShader(m_pixelshader.Get(), NULL, 0);
    dc->GSSetShader(m_geometryshader.Get(), NULL, 0);
    dc->GSSetShaderResources(0, 1, m_particlesrv.GetAddressOf());

    //�R���X�^���g�o�b�t�@�̍X�V
    m_gpu->data.position = GetGameObject()->transform_->GetWorldPosition();
    m_gpu->data.rotation = GetGameObject()->transform_->GetRotation();
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());

    //���
    dc->IASetInputLayout(NULL);
    dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    dc->Draw(static_cast<UINT>(m_maxparticle), 0);

    //�f�v�X�X�e���V���X�e�[�g�ݒ�
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_ON), 0);

    //���
    ID3D11ShaderResourceView* null_shader_resource_view{};
    dc->GSSetShaderResources(0, 1, &null_shader_resource_view);
    dc->VSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->GSSetShader(NULL, NULL, 0);
}

//imgui
void GPUParticle::OnGUI()
{
    SystemGUI();

    //�e�N�X�`�����[�h
    char textureFile[256];
    ZeroMemory(textureFile, sizeof(textureFile));
    ::strncpy_s(textureFile, sizeof(textureFile), m_p.m_textureName.c_str(), sizeof(textureFile));
    if (ImGui::Button("..."))
    {
        const char* filter = "Texture Files(*.DDS;*.dds;*.png;*.jpg;)\0*.DDS;*.dds;*.png;*.jpg;\0All Files(*.*)\0*.*;\0\0";
        DialogResult result = Dialog::OpenFileName(textureFile, sizeof(textureFile), filter, nullptr, Graphics::Instance().GetHwnd());
        if (result == DialogResult::OK)
        {
            std::filesystem::path path = std::filesystem::current_path();
            path.append("Data");

            char drive[32], dir[256], dirname[256];
            ::_splitpath_s(path.string().c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
            ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);
            dirname[strlen(dirname) - 1] = '\0';
            char relativeTextureFile[MAX_PATH];
            PathRelativePathToA(relativeTextureFile, dirname, FILE_ATTRIBUTE_DIRECTORY, textureFile, FILE_ATTRIBUTE_ARCHIVE);

            // �ǂݍ���
            m_p.m_textureName = relativeTextureFile;
            D3D11_TEXTURE2D_DESC texture2d_desc{};
            LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
        }
    }
    ImGui::SameLine();
    ::strncpy_s(textureFile, sizeof(textureFile), m_p.m_textureName.c_str(), sizeof(textureFile));
    if (ImGui::InputText("texture", textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_p.m_textureName = textureFile;

        char drive[32], dir[256], fullPath[256];
        ::_splitpath_s(textureFile, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
        ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
    ImGui::Text(J(u8"���\�[�X"));
    ImGui::Image(m_colormap.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

    ParameterGUI();
}

void GPUParticle::SystemGUI()
{
    if (ImGui::Button("Save"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        LoadDesirialize();
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
    ImGui::SameLine();
    if (ImGui::Button("PLAY"))
    {
        //���Z�b�g�֐�
        Play();
    }
    ImGui::SameLine();
    if (ImGui::Button("ParameterReset"))
    {
        //�p�����[�^���Z�b�g�֐�
        ParameterReset();
    }

    ImGui::Checkbox(J(u8"�����t���O"), reinterpret_cast<bool*>(&m_gpu->data.isalive));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"���[�v"), reinterpret_cast<bool*>(&m_GSC.isLoopFlg));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"��~"), &stopFlg);
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"�X�g���b�`�r���{�[�hON"), reinterpret_cast<bool*>(&m_GSC.stretchFlag));
    ImGui::DragFloat(J(u8"�X�g���b�`�r���{�[�h�̐L�΂��W��"), &m_GSC.strechscale, 0.1f, 1.0f, 100.0f);
    ImGui::Checkbox(J(u8"�폜�t���O"), &deleteflag);

    //�f�o�b�O�p�Ƀu�����h���[�h�ݒ�
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
    //�u�����h���[�h�ݒ胊�X�g�Ƃ̃T�C�Y���Ⴄ�ƃG���[���o��
    static_assert(ARRAYSIZE(BlendName) != static_cast<int>(BLENDSTATE::MAX) - 1, "BlendName Size Error!");
    //�u�����h���[�h�ݒ�
    ImGui::Combo("BlendMode", &m_p.m_blend, BlendName, static_cast<int>(BLENDSTATE::MAX), 10);

    //�f�o�b�O�p�ɐ[�x�X�e�[�g�ݒ�
    constexpr const char* dsName[] =
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
    //�u�����h���[�h�ݒ胊�X�g�Ƃ̃T�C�Y���Ⴄ�ƃG���[���o��
    static_assert(ARRAYSIZE(dsName) != static_cast<int>(DEPTHSTATE::MAX) - 1, "DEPTHSTATE Size Error!");
    //�u�����h���[�h�ݒ�
    ImGui::Combo("DepthState", &m_p.m_depthS, dsName, static_cast<int>(DEPTHSTATE::MAX), ARRAYSIZE(dsName));
}

void GPUParticle::ParameterGUI()
{
    ImGui::DragFloat(J(u8"�G�t�F�N�g�̍Đ�����"), &m_GSC.emitTime, 0.1f);
    ImGui::DragFloat(J(u8"�G�t�F�N�g�̐�������"), &emitTimer, 0.1f);
    ImGui::DragFloat(J(u8"�p�[�e�B�N���̎���"), &m_GSC.lifeTime, 0.1f, 0.0f, 5.0f);
    EmitGUI();
    SpeedGUI();
    ScaleGUI();
    ColorGUI();
}

void GPUParticle::ColorGUI()
{
    if (ImGui::TreeNode(J(u8"�J���[�֌W"))) {
        ImGui::ColorEdit4(J(u8"�x�[�X�̐F"), &m_GSC.baseColor.x);
        ImGui::DragFloat3(J(u8"�J���[�X�P�[��"), &m_GSC.colorScale.x, 0.1f);

        ImGui::Checkbox(J(u8"�p�[�e�B�N���̎����ɂ���ĕω�"), (bool*)&m_GSC.colorVariateByLife);

        if (m_GSC.scaleVariateByLife) {
            ImGui::ColorEdit4(J(u8"�n�߂̐F "), &m_GSC.lifeStartColor.x);
            ImGui::ColorEdit4(J(u8"�Ō�̐F "), &m_GSC.lifeEndColor.x);
        }
        else {
            ImGui::ColorEdit4(J(u8"�n�߂̐F"), &m_GSC.emitStartColor.x);
            ImGui::ColorEdit4(J(u8"�Ō�̐F"), &m_GSC.emitEndColor.x);
        }

        ImGui::TreePop();
    }
}

void GPUParticle::ScaleGUI()
{
    if (ImGui::TreeNode(J(u8"�X�P�[���֌W"))) {
        ImGui::DragFloat2(J(u8"�X�P�[��XY"), &m_GSC.scale.x, 0.01f, 0);
        if (ImGui::DragFloat(J(u8"�X�P�[��"), &m_GSC.scale.x, 0.01f, 0))
        {
            m_GSC.scale.y = m_GSC.scale.x;
        }

        ImGui::Checkbox(J(u8"�p�[�e�B�N���̎����ɂ���ĕω� "), (bool*)&m_GSC.scaleVariateByLife);

        if (m_GSC.scaleVariateByLife) {
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"�ŏ��̑傫�� "), &m_GSC.lifeStartSize, 0.01f, 0.01f, 1.0f);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"�Ō�̑傫�� "), &m_GSC.lifeEndSize, 0.01f, 0.01f, 1.0f);
        }
        else {
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"�ŏ��̑傫��"), &m_GSC.emitStartSize, 0.01f, 0.01f, 1.0f);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"�Ō�̑傫��"), &m_GSC.emitEndSize, 0.01f, 0.01f, 1.0f);
        }

        ImGui::TreePop();
    }
}

void GPUParticle::SpeedGUI()
{
    if (ImGui::TreeNode(J(u8"�X�s�[�h�֌W"))) {
        ImGui::DragFloat(J(u8"�X�s�[�h"), &m_GSC.speed, 0.1f, 0.0f, 100.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�ŏ��̃X�s�[�h"), &m_GSC.emitStartSpeed, 0.1f, 0.01f, 100.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�Ō�̃X�s�[�h"), &m_GSC.emitEndSpeed, 0.1f, 0.01f, 100.0f);

        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"���̓����_��"), &m_GSC.veloRandScale, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat(J(u8"����"), &m_GSC.buoyancy, 0.1f, -100.0f, 100.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�ŏ��̏d��"), &m_GSC.emitStartGravity, 0.01f, 0.0f, 100.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�Ō�̏d��"), &m_GSC.emitEndGravity, 0.01f, 0.0f, 100.0f);

        ImGui::TreePop();
    }
}

void GPUParticle::EmitGUI()
{
    if (ImGui::TreeNode(J(u8"�p�[�e�B�N���̔�΂��� �֌W"))) {
        if (ImGui::DragFloat3(J(u8"���˃x�N�g��"), &m_GSC.emitVec.x, 0.1f)) {
            m_GSC.emitVec = Mathf::Normalize(m_GSC.emitVec);
        }

        ImGui::DragFloat3(J(u8"��]�W��"), &m_GSC.orbitalVelocity.x, 0.1f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�����ꏊ�����_��"), &m_GSC.shape.x, 0.1f, 0.0f, 100.0f);

        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�����p�x"), &m_GSC.shape.w, 0.1f, 0.0f, 1.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"���̑傫��"), &m_GSC.shape.y, 0.1f, 0.0f, 360.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"���̏W�܂�"), &m_GSC.shape.z, 0.1f, 0.0f, 1.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�e����ԌW��"), &m_GSC.radial, 0.1f, -10.0f, 10.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�X�p�C�������x"), &m_GSC.spiralSpeed, 0.1f, 0.0f, 20.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"�X�p�C�������x"), &m_GSC.spiralstrong, 0.1f, 0.0f, 20.0f);

        ImGui::TreePop();
    }
}

//���Z�b�g�֐�
void GPUParticle::Play()
{
    m_gpu->data.isEmitFlg = true;
    emitTimer = 0.0f;
    deleteflag = true;

    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //�R���X�^���g�o�b�t�@�̍X�V
    m_gpu->data.position = GetGameObject()->transform_->GetWorldPosition();
    m_gpu->data.rotation = GetGameObject()->transform_->GetRotation();
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());

    //�������̃s�N�Z���V�F�[�_�[���Z�b�g
    dc->CSSetUnorderedAccessViews(0, 1, m_particleuav.GetAddressOf(), NULL);
    dc->CSSetShader(m_initialzecomputeshader.Get(), NULL, 0);
    const UINT thread_group_count_x = align(static_cast<UINT>(m_maxparticle), THREAD) / THREAD;
    dc->Dispatch(thread_group_count_x, 1, 1);
    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);
}

//���g�������֐�
void GPUParticle::DeleteMe(float deletetime)
{
    if (deleteflag)
    {
        if (emitTimer > deletetime)
        {
            GameObjectManager::Instance().Remove(GetGameObject());
        }
    }
}

//�V���A���C�Y
void GPUParticle::Serialize()
{
    static const char* filter = "Particle Files(*.gpuparticle)\0*.gpuparticle;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "gpuparticle", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);

            try
            {
                archive
                (
                    CEREAL_NVP(m_GSC),
                    CEREAL_NVP(m_p)
                );
            }
            catch (...)
            {
                LOG("particle deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

//�f�V���A���C�Y
void GPUParticle::Desirialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(m_GSC),
                CEREAL_NVP(m_p)
            );
        }
        catch (...)
        {
            LOG("particle deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

//�ǂݍ���
void GPUParticle::LoadDesirialize()
{
    static const char* filter = "particle Files(*.gpuparticle)\0*.gpuparticle;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Desirialize(filename);
    }
}

//�p�����[�^���Z�b�g
void GPUParticle::ParameterReset()
{
    m_GSC.lifeTime = 1.0f;
    m_GSC.shape = { 0.0f,0.0f,0.0f,0.0f };

    m_GSC.baseColor = { 1,1,1,1 };
    m_GSC.lifeStartColor = { 1,1,1,1 };
    m_GSC.lifeEndColor = { 1,1,1,1 };
    m_GSC.emitStartColor = { 1,1,1,1 };
    m_GSC.emitEndColor = { 1,1,1,1 };
    m_GSC.colorScale = { 1,1,1 };

    m_GSC.emitVec = { 0,0,0 };
    m_GSC.spiralSpeed = { 0 };
    m_GSC.orbitalVelocity = { 0,0,0 };
    m_GSC.veloRandScale = 0.0f;
    m_GSC.speed = 1.0f;
    m_GSC.emitStartSpeed = 1.0f;
    m_GSC.emitEndSpeed = 1.0f;

    m_GSC.scale = { 0.2f,0.2f };
    m_GSC.lifeStartSize = 1.0f;
    m_GSC.lifeEndSize = 1.0f;
    m_GSC.emitStartSize = 1.0f;
    m_GSC.emitEndSize = 1.0f;

    m_GSC.radial = { 0 };
    m_GSC.buoyancy = {};
    m_GSC.emitStartGravity = 0.0f;
    m_GSC.emitEndGravity = 0.0f;
    m_GSC.strechscale = 1.0f;
}