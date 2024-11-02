#include "ColliderCom.h"
#include "Graphics\Graphics.h"
#include "Component/System/TransformCom.h"
#include "Component\Renderer\RendererCom.h"
#include "NodeCollsionCom.h"
#include <imgui.h>

//�����蔻��
#pragma region Collider

void Collider::OnGUI()
{
    bool hit = isHit;
    ImGui::Checkbox("isHit", &hit);
}

//����O�̃N���A
void Collider::ColliderStartClear()
{
    isHit = false;
    hitObj_.clear();
}

//������w�肵�Ĕ���
void Collider::ColliderVSOther(std::shared_ptr<Collider> otherSide)
{
    //�L����
    if (!isEnabled_)return;
    if (!otherSide->isEnabled_)return;

    //�^�O�Ŕ��肷�邩���߂�
    if ((myTag_ == NONE_COL) || (otherSide->myTag_ == NONE_COL))return;
    if ((judgeTag_ != otherSide->myTag_) && (myTag_ != otherSide->judgeTag_))return;

    //�`�󖈂ɓ����蔻���ς���
    int myType = colliderType_;
    int otherType = otherSide->colliderType_;

    bool isJudgeMy = false;
    bool isJudgeOther = false;

    //���C
    HitResult rayResult;
    if (myType == COLLIDER_TYPE::RayCollider)
    {
        isJudgeMy = RayVsNodeCollision(otherSide, rayResult, true);
    }
    HitResult otherRayResult;
    if (otherType == COLLIDER_TYPE::RayCollider)
    {
        isJudgeOther = RayVsNodeCollision(otherSide, otherRayResult, false);
    }
    //��
    if (myType == COLLIDER_TYPE::SphereCollider)
    {
        isJudgeMy = SphereVsNodeCollision(otherSide, true);
    }
    if (otherType == COLLIDER_TYPE::SphereCollider)
    {
        isJudgeOther = SphereVsNodeCollision(otherSide, false);
    }
    //�J�v�Z��
    if (myType == COLLIDER_TYPE::CapsuleCollider)
    {
        isJudgeMy = CapsuleVsNodeCollision(otherSide, true);
    }
    if (otherType == COLLIDER_TYPE::CapsuleCollider)
    {
        isJudgeOther = CapsuleVsNodeCollision(otherSide, false);
    }

    //�m�[�h�R���W�����p
    if (isJudgeMy)
    {
        if (judgeTag_ == otherSide->myTag_) {
            HitObj h;
            h.hitPos = rayResult.position;
            h.hitNormal = rayResult.normal;
            h.gameObject = otherSide->GetGameObject();
            hitObj_.emplace_back(h);
            isHit = true;
        }
    }
    if (isJudgeOther)
    {
        if (otherSide->judgeTag_ == myTag_) {
            HitObj h;
            h.hitPos = otherRayResult.position;
            h.hitNormal = otherRayResult.normal;
            h.gameObject = GetGameObject();
            otherSide->hitObj_.emplace_back(h);
            otherSide->isHit = true;
        }
    }
}

bool Collider::SphereVsNodeCollision(std::shared_ptr<Collider> otherSide, bool isMyRay)
{
    //�`��𔻒�
    std::shared_ptr<SphereColliderCom> sphere;
    Model* model;

    if (isMyRay)
    {
        sphere = std::static_pointer_cast<SphereColliderCom>(shared_from_this());

        auto& renderer = otherSide->GetGameObject()->GetComponent<RendererCom>();
        if (!renderer)return false;
        model = renderer->GetModel();
        if (model->cp.size() <= 0)return false;
    }
    else
    {
        sphere = std::static_pointer_cast<SphereColliderCom>(otherSide);

        auto& renderer = shared_from_this()->GetGameObject()->GetComponent<RendererCom>();
        if (!renderer)return false;
        model = renderer->GetModel();
        if (model->cp.size() <= 0)return false;
    }

    DirectX::XMFLOAT3 spherePos = sphere->GetGameObject()->transform_->GetWorldPosition();
    float sphereRadius = sphere->GetRadius();

    bool isHit = false;

    for (auto& nodeEntry : model->cp)
    {
        for (auto& col : nodeEntry.second)
        {
            DirectX::XMFLOAT3 startPos =
            {
                model->GetNodes()[col.nodeid].worldTransform._41,
                model->GetNodes()[col.nodeid].worldTransform._42,
                model->GetNodes()[col.nodeid].worldTransform._43
            };

            //sphere
            if (col.collsiontype == int(NodeCollsionCom::CollsionType::SPHER))
            {
                float length = Mathf::Length(startPos - spherePos);
                if (length < col.radius + sphereRadius)
                {
                    isHit = true;
                    //�������Ă��鎞�_��for���𔲂���
                    break;
                }
            }
            //clynder
            else if (col.collsiontype == int(NodeCollsionCom::CollsionType::CYLINDER))
            {
                if (col.endnodeid < 0)continue;
                DirectX::XMFLOAT3 endPos =
                {
                    model->GetNodes()[col.endnodeid].worldTransform._41,
                    model->GetNodes()[col.endnodeid].worldTransform._42,
                    model->GetNodes()[col.endnodeid].worldTransform._43
                };

                if (Collision::IntersectSphereVsCylider(spherePos, sphereRadius,
                    startPos, Mathf::Normalize(endPos - startPos), col.radius, col.height))
                {
                    isHit = true;
                    //�������Ă��鎞�_��for���𔲂���
                    break;
                }
            }
            //box
            else if (col.collsiontype == int(NodeCollsionCom::CollsionType::BOX))
            {
                //�{�b�N�X�̍s��X�P�[���𒲐�
                DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&model->GetNodes()[col.nodeid].worldTransform);
                mat.r[0] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[0]), col.scale.x);
                mat.r[1] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[1]), col.scale.y);
                mat.r[2] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[2]), col.scale.z);
                //�I�t�Z�b�g�|�X
                mat.r[3].m128_f32[0] = mat.r[3].m128_f32[0] + col.offsetpos.x;
                mat.r[3].m128_f32[1] = mat.r[3].m128_f32[1] + col.offsetpos.y;
                mat.r[3].m128_f32[2] = mat.r[3].m128_f32[2] + col.offsetpos.z;

                if (Collision::IntersectSphereVsOBB(DirectX::XMLoadFloat3(&spherePos), sphereRadius, mat))
                {
                    isHit = true;
                    //�������Ă��鎞�_��for���𔲂���
                    break;
                }
            }
        }
    }

    return isHit;
}

bool Collider::CapsuleVsNodeCollision(std::shared_ptr<Collider> otherSide, bool isMyRay)
{
    //�`��𔻒�
    std::shared_ptr<CapsuleColliderCom> capsule;
    Model* model;

    if (isMyRay)
    {
        capsule = std::static_pointer_cast<CapsuleColliderCom>(shared_from_this());

        auto& renderer = otherSide->GetGameObject()->GetComponent<RendererCom>();
        if (!renderer)return false;
        model = renderer->GetModel();
        if (model->cp.size() <= 0)return false;
    }
    else
    {
        capsule = std::static_pointer_cast<CapsuleColliderCom>(otherSide);

        auto& renderer = shared_from_this()->GetGameObject()->GetComponent<RendererCom>();
        if (!renderer)return false;
        model = renderer->GetModel();
        if (model->cp.size() <= 0)return false;
    }

    //�K�v�ȃp�����[�^�[�擾
    //�J�v�Z��
    CapsuleColliderCom::Capsule Capsule = capsule->GetCupsule();
    DirectX::XMFLOAT3 centerPos = capsule->GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 cP0 = { centerPos.x + Capsule.p0.x,centerPos.y + Capsule.p0.y,centerPos.z + Capsule.p0.z };
    DirectX::XMFLOAT3 cP1 = { centerPos.x + Capsule.p1.x,centerPos.y + Capsule.p1.y,centerPos.z + Capsule.p1.z };

    DirectX::XMVECTOR dCapsule = { cP1.x - cP0.x,cP1.y - cP0.y,cP1.z - cP0.z };

    bool isHit = false;

    for (auto& nodeEntry : model->cp)
    {
        for (auto& col : nodeEntry.second)
        {
            DirectX::XMFLOAT3 startPos =
            {
                model->GetNodes()[col.nodeid].worldTransform._41,
                model->GetNodes()[col.nodeid].worldTransform._42,
                model->GetNodes()[col.nodeid].worldTransform._43
            };

            //sphere
            if (col.collsiontype == int(NodeCollsionCom::CollsionType::SPHER))
            {
                float l = sqrtf(DirectX::XMVectorGetX(DirectX::XMVector3Dot(dCapsule, dCapsule)));
                dCapsule = DirectX::XMVector3Normalize(dCapsule);	// ���K��

                FLOAT t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dCapsule, { startPos.x - cP0.x, startPos.y - cP0.y, startPos.z - cP0.z }));	// �ˉe���̎Z�o
                DirectX::XMVECTOR Q = {};	// �ŋߓ_
                if (t < 0)
                    Q = DirectX::XMLoadFloat3(&cP0);
                else if (t > l)
                    Q = DirectX::XMLoadFloat3(&cP1);
                else
                    Q = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&cP0), DirectX::XMVectorScale(dCapsule, t));

                // ��������
                DirectX::XMVECTOR Len = DirectX::XMVectorSubtract(Q, DirectX::XMLoadFloat3(&startPos));

                if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(Len, Len)) < (col.radius + Capsule.radius) * (col.radius + Capsule.radius))// ���Q�擯�m�ō�����r
                {
                    isHit = true;
                    //�������Ă��鎞�_��for���𔲂���
                    break;
                }
            }
            //clynder
            else if (col.collsiontype == int(NodeCollsionCom::CollsionType::CYLINDER))
            {
                if (col.endnodeid < 0)continue;
                DirectX::XMFLOAT3 endPos =
                {
                    model->GetNodes()[col.endnodeid].worldTransform._41,
                    model->GetNodes()[col.endnodeid].worldTransform._42,
                    model->GetNodes()[col.endnodeid].worldTransform._43
                };

                //�J�v�Z��
                if (Collision::IntersectCapsuleVsCylinder(DirectX::XMLoadFloat3(&cP0), DirectX::XMLoadFloat3(&cP1), Capsule.radius,
                    DirectX::XMLoadFloat3(&startPos), DirectX::XMLoadFloat3(&endPos), col.radius))
                {
                    isHit = true;
                    //�������Ă��鎞�_��for���𔲂���
                    break;
                }
            }
            //box
            else if (col.collsiontype == int(NodeCollsionCom::CollsionType::BOX))
            {
                //�{�b�N�X�̍s��X�P�[���𒲐�
                DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&model->GetNodes()[col.nodeid].worldTransform);
                mat.r[0] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[0]), col.scale.x);
                mat.r[1] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[1]), col.scale.y);
                mat.r[2] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[2]), col.scale.z);
                //�I�t�Z�b�g�|�X
                mat.r[3].m128_f32[0] = mat.r[3].m128_f32[0] + col.offsetpos.x;
                mat.r[3].m128_f32[1] = mat.r[3].m128_f32[1] + col.offsetpos.y;
                mat.r[3].m128_f32[2] = mat.r[3].m128_f32[2] + col.offsetpos.z;

                if (Collision::IntersectCapsuleVsOBB(DirectX::XMLoadFloat3(&cP0), DirectX::XMLoadFloat3(&cP1), Capsule.radius,
                    mat))
                {
                    isHit = true;
                    //�������Ă��鎞�_��for���𔲂���
                    break;
                }
            }
        }
    }

    return isHit;
}

bool Collider::RayVsNodeCollision(std::shared_ptr<Collider> otherSide, HitResult& h, bool isMyRay)
{
    //�`��𔻒�
    std::shared_ptr<RayColliderCom> ray;
    Model* model;

    if (isMyRay)
    {
        ray = std::static_pointer_cast<RayColliderCom>(shared_from_this());

        auto& renderer = otherSide->GetGameObject()->GetComponent<RendererCom>();
        if (!renderer)return false;
        model = renderer->GetModel();
        if (model->cp.size() <= 0)return false;
    }
    else
    {
        ray = std::static_pointer_cast<RayColliderCom>(otherSide);

        auto& renderer = shared_from_this()->GetGameObject()->GetComponent<RendererCom>();
        if (!renderer)return false;
        model = renderer->GetModel();
        if (model->cp.size() <= 0)return false;
    }

    DirectX::XMFLOAT3 s = ray->GetStart();
    DirectX::XMFLOAT3 e = ray->GetEnd();

    //�q�b�g�������ő�ŏ�����
    h.distance = FLT_MAX;

    bool isHit = false;

    for (auto& nodeEntry : model->cp)
    {
        for (auto& col : nodeEntry.second)
        {
            DirectX::XMFLOAT3 startPos =
            {
                model->GetNodes()[col.nodeid].worldTransform._41,
                model->GetNodes()[col.nodeid].worldTransform._42,
                model->GetNodes()[col.nodeid].worldTransform._43
            };

            //sphere
            if (col.collsiontype == int(NodeCollsionCom::CollsionType::SPHER))
            {
                HitResult hit;
                if (Collision::IntersectRayVsSphere(
                    DirectX::XMLoadFloat3(&s), DirectX::XMLoadFloat3(&Mathf::Normalize(e - s)), Mathf::Length(e - s),
                    DirectX::XMLoadFloat3(&startPos), col.radius, hit))
                {
                    //���������ċ߂���Ώ㏑������
                    if (h.distance > hit.distance)
                    {
                        h = hit;
                        isHit = true;
                    }
                }
            }
            //clynder
            else if (col.collsiontype == int(NodeCollsionCom::CollsionType::CYLINDER))
            {
                DirectX::XMFLOAT3 endPos =
                {
                    model->GetNodes()[col.endnodeid].worldTransform._41,
                    model->GetNodes()[col.endnodeid].worldTransform._42,
                    model->GetNodes()[col.endnodeid].worldTransform._43
                };

                HitResult hit;
                if (Collision::IntersectRayVsOrientedCylinder(
                    DirectX::XMLoadFloat3(&s), DirectX::XMLoadFloat3(&Mathf::Normalize(e - s)), Mathf::Length(e - s),
                    DirectX::XMLoadFloat3(&startPos), DirectX::XMLoadFloat3(&endPos), col.radius, hit))
                {
                    //���������ċ߂���Ώ㏑������
                    if (h.distance > hit.distance)
                    {
                        h = hit;
                        isHit = true;
                    }
                }
            }
            //box
            else if (col.collsiontype == int(NodeCollsionCom::CollsionType::BOX))
            {
                //�{�b�N�X�̍s��X�P�[���𒲐�
                DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&model->GetNodes()[col.nodeid].worldTransform);
                mat.r[0] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[0]), col.scale.x);
                mat.r[1] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[1]), col.scale.y);
                mat.r[2] = DirectX::XMVectorScale(DirectX::XMVector3Normalize(mat.r[2]), col.scale.z);
                //�I�t�Z�b�g�|�X
                mat.r[3].m128_f32[0] = mat.r[3].m128_f32[0] + col.offsetpos.x;
                mat.r[3].m128_f32[1] = mat.r[3].m128_f32[1] + col.offsetpos.y;
                mat.r[3].m128_f32[2] = mat.r[3].m128_f32[2] + col.offsetpos.z;

                HitResult hit;
                if (Collision::IntersectRayVsBOX(
                    DirectX::XMLoadFloat3(&s), DirectX::XMLoadFloat3(&Mathf::Normalize(e - s)), Mathf::Length(e - s),
                    mat, hit))
                {
                    //���������ċ߂���Ώ㏑������
                    if (h.distance > hit.distance)
                    {
                        h = hit;
                        isHit = true;
                    }
                }
            }
        }
    }

    if (isHit)ray->SetHitPosDebug(h.position);

    return isHit;
}

#pragma endregion

/// ������`��֐� ///

//��
#pragma region SphereCollider

// GUI�`��
void SphereColliderCom::OnGUI()
{
    Collider::OnGUI();

    ImGui::DragFloat("radius", &radius_, 0.1f);
    ImGui::DragFloat3("offsetPos", &offsetButtonPos_.x, 0.1f);
    if (isPushBack_)
    {
        ImGui::DragFloat("weight", &weight_, 0.1f, 0, 10);
    }
}

// debug�`��
void SphereColliderCom::DebugRender()
{
    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
    pos = { pos.x + offsetButtonPos_.x,pos.y + offsetButtonPos_.y,pos.z + offsetButtonPos_.z };
    Graphics::Instance().GetDebugRenderer()->DrawSphere(
        pos, radius_, { 1,0,0,1 });
}

#pragma endregion

//�l�p
#pragma region BoxCollider

// GUI�`��
void BoxColliderCom::OnGUI()
{
    Collider::OnGUI();

    ImGui::DragFloat3("size", &size_.x, 0.1f);
    ImGui::DragFloat3("offsetPos", &offsetButtonPos_.x, 0.1f);
}

// debug�`��
void BoxColliderCom::DebugRender()
{
    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
    pos = { pos.x + offsetButtonPos_.x,pos.y + offsetButtonPos_.y,pos.z + offsetButtonPos_.z };
    Graphics::Instance().GetDebugRenderer()->DrawBox(pos, size_, { 0,1,0,1 });
}

#pragma endregion

//�J�v�Z��
#pragma region CapsuleCollider

// GUI�`��
void CapsuleColliderCom::OnGUI()
{
    Collider::OnGUI();

    ImGui::DragFloat3("p0", &capsule_.p0.x, 0.1f);
    ImGui::DragFloat3("p1", &capsule_.p1.x, 0.1f);
    ImGui::DragFloat("radius", &capsule_.radius, 0.01f);
}

// debug�`��
void CapsuleColliderCom::DebugRender()
{
    //p0
    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 pos0 = { capsule_.p0.x + pos.x,capsule_.p0.y + pos.y,capsule_.p0.z + pos.z };
    Graphics::Instance().GetDebugRenderer()->DrawSphere(pos0, capsule_.radius, { 0,0,1,1 });
    //p1
    DirectX::XMFLOAT3 pos1 = { capsule_.p1.x + pos.x,capsule_.p1.y + pos.y,capsule_.p1.z + pos.z };
    Graphics::Instance().GetDebugRenderer()->DrawSphere(pos1, capsule_.radius, { 0,0,1,1 });
    //�~������
    DirectX::XMVECTOR P0 = DirectX::XMLoadFloat3(&pos0);
    DirectX::XMVECTOR P1 = DirectX::XMLoadFloat3(&pos1);
    float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(P0, P1)));
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(pos0, pos1, capsule_.radius, length, { 0,0,1,1 });
}

#pragma endregion

//ray
#pragma region RayCollider

void RayColliderCom::OnGUI()
{
    Collider::OnGUI();

    ImGui::DragFloat3("start", &start.x, 0.1f);
    ImGui::DragFloat3("end", &end.x, 0.1f);
    ImGui::Checkbox("hitDraw", &hitDraw);
}

// debug�`��
void RayColliderCom::DebugRender()
{
    float debugRadius = 0.1f;
    //p0
    DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 pos0 = { start.x,start.y,start.z };
    Graphics::Instance().GetDebugRenderer()->DrawSphere(pos0, debugRadius, { 1,0,1,1 });
    //p1
    DirectX::XMFLOAT3 pos1 = { end.x,end.y,end.z };
    Graphics::Instance().GetDebugRenderer()->DrawSphere(pos1, debugRadius, { 1,0,1,1 });
    //�~������
    DirectX::XMVECTOR P0 = DirectX::XMLoadFloat3(&pos0);
    DirectX::XMVECTOR P1 = DirectX::XMLoadFloat3(&pos1);
    float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(P0, P1)));
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(pos0, pos1, debugRadius, length, { 1,0,1,1 });

    if (hitDraw)
    {
        Graphics::Instance().GetDebugRenderer()->DrawSphere(hitPos, debugRadius + 0.2f, { 1,1,0.1f,1 });
    }
}

#pragma endregion