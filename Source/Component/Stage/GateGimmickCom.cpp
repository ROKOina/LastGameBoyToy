#include "GateGimmickCom.h"

void GateGimmick::Start()
{

}

void GateGimmick::Update(float elapsedTime)
{
    //ステート更新処理
    state.Update(elapsedTime);
}
