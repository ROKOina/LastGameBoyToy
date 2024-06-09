#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include "ModelResource.h"

class Model
{
public:
    Model(std::shared_ptr<ModelResource> resource);
    ~Model() {}

    struct Node
    {
        const char* name;
        Node* parent;
        DirectX::XMFLOAT3	scale;
        DirectX::XMFLOAT4	rotate;
        DirectX::XMFLOAT3	translate;
        DirectX::XMFLOAT4X4	localTransform;
        DirectX::XMFLOAT4X4	worldTransform;
        int layer;

        std::vector<Node*>	children;
    };

   
    // �s��v�Z
    void UpdateTransform(const DirectX::XMMATRIX& Transform);

    
    const std::vector<Node>& GetNodes() const { return nodes; }
    std::vector<Node>& GetNodes() { return nodes; }
    const ModelResource* GetResource() const { return resource.get(); }
    ModelResource* GetResource() { return resource.get(); }

   

    //�V���A���C�Y�ɓK�p������ׂ̏���
    void CopyModel();

    //�I�[�v���m�[�h���������ɋt�ɏ�������
    void CopyRefrectModel();

public:
    //�m�[�h�����@�㔼�g����
    Node* FindNode(const char* name);

    //����̃m�[�h�̔ԍ�����
    int FindNodeIndex(const char* name);


private:

    enum Layer
    {
        ALLBODY,
        UPPER,
        LOWER,
        LAYERMAX
    };

private:
    std::shared_ptr<ModelResource>	resource;
    std::vector<Node>				nodes;
    
};