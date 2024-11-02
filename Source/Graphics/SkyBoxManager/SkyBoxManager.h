#pragma once

#include <array>
#include <d3d11.h>
#include <wrl.h>

class SkyBoxManager
{
private:
    SkyBoxManager();
    SkyBoxManager(SkyBoxManager&) {}

public:
    // �V���O���g��
    static SkyBoxManager& Instance() {
        static SkyBoxManager instance;
        return instance;
    }

    // �����F�X�J�C�{�b�N�X�Ɋ֌W����4�̃e�N�X�`���̃p�X
    void LoadSkyBoxTextures(std::array<const char*, 4> filepath);

    // SRV�̐ݒ� �����FSRV�o�^�ԍ�
    void BindTextures(ID3D11DeviceContext* dc, int startRegisterIndex);

    // �X�J�C�{�b�N�X�̕`��
    void DrawSkyBox(ID3D11DeviceContext* dc);

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyBoxTextures[4];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> skyBoxShader;
};
