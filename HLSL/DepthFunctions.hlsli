#include "Constants.hlsli"

// �Q�l : https://qiita.com/ScreenPocket/items/6bd9896314dc78f3b170
inline float LinearDepth(float z)
{
    return 1.0 / ((1 - cameraScope.y / cameraScope.x) * z + (cameraScope.y / cameraScope.x));
}