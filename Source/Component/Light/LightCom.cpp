#include "LightCom.h"
#include "Graphics/Graphics.h"

//�R���X�g���N�^
Light::Light(const char* filename)
{
    //Graphics& graphics = Graphics::Instance();
    //ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //// �萔�o�b�t�@�̍쐬
    //D3D11_BUFFER_DESC desc;
    //::memset(&desc, 0, sizeof(desc));
    //desc.Usage = D3D11_USAGE_DEFAULT;
    //desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //desc.CPUAccessFlags = 0;
    //desc.MiscFlags = 0;
    //desc.ByteWidth = sizeof(LightCB);
    //desc.StructureByteStride = 0;

    ////�R���X�^���g�o�b�t�@�̍쐬
    //HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, 0, m_lightCb.GetAddressOf());
    //_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //// �萔�o�b�t�@�ݒ�
    //dc->VSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    //dc->PSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    //dc->CSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    //dc->GSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    //dc->HSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    //dc->DSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
}

//�X�V����
void Light::Update(float elapsedTime)
{
}

//imgui
void Light::OnGUI()
{
}