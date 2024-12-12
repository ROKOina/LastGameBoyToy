#include "RespawnCom.h"
#include "Component\System\TransformCom.h"

void RespawnCom::Respawn()
{
    GetGameObject()->transform_->SetWorldPosition(respawnPos);
}
