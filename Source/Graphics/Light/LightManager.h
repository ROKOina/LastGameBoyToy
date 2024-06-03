#pragma once

#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include "Light.h"

// ���C�g�Ǘ��N���X
class LightManager
{
    LightManager();
    ~LightManager();

public:

    //�C���X�^���X�擾
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
    int GetLightCount() { return static_cast<int>(m_lights.size()); }
    Light* GetLight(const int& index) const { return m_lights.at(index); }

private:
    std::vector<Light*>	m_lights;

    Microsoft::WRL::ComPtr<ID3D11Buffer>m_lightCb; // �萔�o�b�t�@
};
