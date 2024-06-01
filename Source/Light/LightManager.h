#pragma once

#include <vector>
#include <memory>
#include "Light.h"

#include "Graphics/Texture.h"

// ���C�g�Ǘ��N���X
class LightManager
{
    LightManager();
    ~LightManager();

public:
    static LightManager& Instance()
    {
        static	LightManager	lightManager;
        return	lightManager;
    }

    // ���C�g��o�^����
    void Register(Light* light);

    // ���C�g�̓o�^����������
    void Remove(Light* light);

    // �o�^�ς݂̃��C�g��S�폜����
    void Clear();

    // ���C�g����RenderContext�ɐς�
    void UpdateConstatBuffer();

    // �f�o�b�O���̕\��
    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�̕\��
    void DrawDebugPrimitive();

    // ���C�g�̏��
    int GetLightCount() const { return static_cast<int>(lights.size()); }
    Light* GetLight(const int& index) const { return lights.at(index); }

    // �V���h�E�}�b�v�Ɏg�p���郉�C�g
    Light* GetShadowLight() const { return shadowLight; }
    void SetShadowLight(Light* light) { shadowLight = light; }

private:
    // GPU�ɃX�J�C�{�b�N�X��ݒ肷��
    void SetSkyboxSrv();

private:
    std::vector<Light*>	lights;
    Light* shadowLight = nullptr;

    Microsoft::WRL::ComPtr<ID3D11Buffer>	lightCb; // �萔�o�b�t�@
};
