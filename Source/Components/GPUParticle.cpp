#include "GPUParticle.h"
#include "Graphics/Graphics.h"
#include "Misc.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/Texture.h"
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
        CEREAL_NVP(color),
        CEREAL_NVP(startcolor),
        CEREAL_NVP(endcolor),
        CEREAL_NVP(shape),
        CEREAL_NVP(velocity),
        CEREAL_NVP(lifetime),
        CEREAL_NVP(luminance),
        CEREAL_NVP(speed),
        CEREAL_NVP(scale),
        CEREAL_NVP(startsize),
        CEREAL_NVP(endsize),
        CEREAL_NVP(orbitalvelocity),
        CEREAL_NVP(radial),
        CEREAL_NVP(startspeed),
        CEREAL_NVP(endspeed),
        CEREAL_NVP(velorandscale),
        CEREAL_NVP(strechflag),
        CEREAL_NVP(buoyancy),
        CEREAL_NVP(startgravity),
        CEREAL_NVP(endgravity),
        CEREAL_NVP(savedummy)
    );
}

template<class Archive>
void GPUParticle::SaveParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(m_blend),
        CEREAL_NVP(m_filename)
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
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);

    //�R���X�^���g�o�b�t�@�̃o�b�t�@�쐬�A�X�V
    buffer_desc.ByteWidth = sizeof(GPUparticleSaveConstants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, m_constantbuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());

    //�������̃s�N�Z���V�F�[�_�[���Z�b�g
    dc->CSSetUnorderedAccessViews(0, 1, m_particleuav.GetAddressOf(), NULL);
    dc->CSSetShader(m_initialzecomputeshader.Get(), NULL, 0);
    const UINT thread_group_count_x = align(static_cast<UINT>(maxparticle), THREAD) / THREAD;
    dc->Dispatch(thread_group_count_x, 1, 1);
    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);

    //�t�@�C���ǂݍ��ݏ���
    if (filename)
    {
        Desirialize(filename);
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_filename.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
}

//�X�V����
void GPUParticle::Update(float elapsedTime)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

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

    //�p�����[�^������
    if (m_gpu->data.loop == 0)
    {
        //Reset();
    }
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
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 0);

    //�v���~�e�B�u�g�|���W�[�ݒ�
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    //�V�F�[�_�[�Z�b�g
    dc->VSSetShader(m_vertexshader.Get(), NULL, 0);
    dc->PSSetShader(m_pixelshader.Get(), NULL, 0);
    dc->GSSetShader(m_geometryshader.Get(), NULL, 0);
    dc->GSSetShaderResources(0, 1, m_particlesrv.GetAddressOf());

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
    if (ImGui::Button("Save"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        LoadDesirialize();
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_filename.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        //���Z�b�g�֐�
        Reset();
        m_gpu->data.loop = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("ParameterReset"))
    {
        //�p�����[�^���Z�b�g�֐�
        ParameterReset();
    }

    //�e�N�X�`�����[�h
    char textureFile[256];
    ZeroMemory(textureFile, sizeof(textureFile));
    ::strncpy_s(textureFile, sizeof(textureFile), m_p.m_filename.c_str(), sizeof(textureFile));
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
            m_p.m_filename = relativeTextureFile;
            D3D11_TEXTURE2D_DESC texture2d_desc{};
            LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_filename.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
        }
    }
    ImGui::SameLine();
    ::strncpy_s(textureFile, sizeof(textureFile), m_p.m_filename.c_str(), sizeof(textureFile));
    if (ImGui::InputText("texture", textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_p.m_filename = textureFile;

        char drive[32], dir[256], fullPath[256];
        ::_splitpath_s(textureFile, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
        ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_filename.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
    ImGui::Text(J(u8"���\�[�X"));
    ImGui::Image(m_colormap.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
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
    ImGui::Checkbox(J(u8"�����t���O"), reinterpret_cast<bool*>(&m_gpu->data.isalive));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"���[�v"), reinterpret_cast<bool*>(&m_gpu->data.loop));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"�X�^�[�g"), reinterpret_cast<bool*>(&m_gpu->data.startflag));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"�X�g���b�`�r���{�[�hON"), reinterpret_cast<bool*>(&m_GSC.strechflag));
    ImGui::DragFloat(J(u8"��������"), &m_GSC.lifetime, 0.1f, 0.0f, 5.0f);
    ImGui::DragFloat2(J(u8"�X�P�[��"), &m_GSC.scale.x, 0.01f, 0);
    if (ImGui::DragFloat(J(u8"�S���̃X�P�[��"), &m_GSC.scale.x, 0.01f, 0))
    {
        m_GSC.scale.y = m_GSC.scale.x;
    }
    ImGui::ColorEdit4(J(u8"�F"), &m_GSC.color.x);
    ImGui::ColorEdit4(J(u8"�n�߂̐F"), &m_GSC.startcolor.x);
    ImGui::ColorEdit4(J(u8"�Ō�̐F"), &m_GSC.endcolor.x);
    ImGui::DragFloat3(J(u8"�P�x"), &m_GSC.luminance.x, 0.1f);
    ImGui::DragFloat3(J(u8"����"), &m_GSC.velocity.x, 0.1f);
    ImGui::DragFloat3(J(u8"��]�W��"), &m_GSC.orbitalvelocity.x, 0.1f);
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
    ImGui::DragFloat(J(u8"�e����ԌW��"), &m_GSC.radial, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat(J(u8"�X�s�[�h"), &m_GSC.speed, 0.1f, 0.0f, 100.0f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"�ŏ��̃X�s�[�h"), &m_GSC.startspeed, 0.1f, 0.01f, 100.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"�Ō�̃X�s�[�h"), &m_GSC.endspeed, 0.1f, 0.01f, 100.0f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"�ŏ��̑傫��"), &m_GSC.startsize, 0.01f, 0.01f, 1.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"�Ō�̑傫��"), &m_GSC.endsize, 0.01f, 0.01f, 1.0f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"�����ꏊ�����_��"), &m_GSC.shape.x, 0.1f, 0.0f, 100.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"���̓����_��"), &m_GSC.velorandscale, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat3(J(u8"����"), &m_GSC.buoyancy.x, 0.1f, -100.0f, 100.0f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"�ŏ��̏d��"), &m_GSC.startgravity, 0.01f, 0.0f, 100.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"�Ō�̏d��"), &m_GSC.endgravity, 0.01f, 0.0f, 100.0f);
}

//���Z�b�g�֐�
void GPUParticle::Reset()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //�������̃s�N�Z���V�F�[�_�[���Z�b�g
    dc->CSSetUnorderedAccessViews(0, 1, m_particleuav.GetAddressOf(), NULL);
    dc->CSSetShader(m_initialzecomputeshader.Get(), NULL, 0);
    const UINT thread_group_count_x = align(static_cast<UINT>(m_maxparticle), THREAD) / THREAD;
    dc->Dispatch(thread_group_count_x, 1, 1);
    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);
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
    m_GSC.color = { 1,1,1,1 };
    m_GSC.startcolor = { 1,1,1,1 };
    m_GSC.endcolor = { 1,1,1,1 };
    m_GSC.shape = { 0.0f,0.0f,0.0f,0.0f };
    m_GSC.velocity = { 0,0,0 };
    m_GSC.lifetime = 1.0f;
    m_GSC.luminance = { 1,1,1 };
    m_GSC.speed = 1.0f;
    m_GSC.scale = { 0.2f,0.2f };
    m_GSC.startsize = 1.0f;
    m_GSC.endsize = 1.0f;
    m_GSC.orbitalvelocity = { 0,0,0 };
    m_GSC.radial = { 0 };
    m_GSC.startspeed = 1.0f;
    m_GSC.endspeed = 1.0f;
    m_GSC.velorandscale = 0.0f;
    m_GSC.buoyancy = {};
    m_GSC.startgravity = 0.0f;
    m_GSC.endgravity = 0.0f;
}