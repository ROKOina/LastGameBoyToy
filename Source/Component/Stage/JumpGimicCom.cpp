#include "JumpGimicCom.h"
#include "Component\Collsion\ColliderCom.h"

void JumpBoardCom::Update(float elapsedTime)
{
    const auto& collider = GetGameObject()->GetComponent<SphereColliderCom>().get();
    for (const auto& obj : collider->OnHitGameObject())
    {
        if (std::string(obj.gameObject.lock()->GetName()) == "player")
        {

        }
    }
}
