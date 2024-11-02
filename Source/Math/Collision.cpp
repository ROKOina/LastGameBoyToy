#include "Collision.h"
#include "Graphics/Graphics.h"
#include <cmath>
#include "QuaternionStruct.h"

//球と球の交差判定
bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outPositionB)
{
    //B->Aの単位ベクトルを算出
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);  //引き算で方向を求めるa->b
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = スクエア(平方根)    2乗していると考えて良い
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //距離測定
    float range = radiusA + radiusB;
    if (range * range < lengthSq)
    {
        return false;
    }

    Vec = DirectX::XMVector3Normalize(Vec);     //正規化して単位ベクトルに
    Vec = DirectX::XMVectorScale(Vec, range);   //AとBの距離分のベクトル作成

    PositionB = DirectX::XMVectorAdd(PositionA, Vec);   //A->Bに押し出すので、
    //ポジションAを足し、Vecを足す

//AがBを押し出す
// PositionB = DirectX::XMVectorAdd(Vec, LengthSq );
    DirectX::XMStoreFloat3(&outPositionB, PositionB);

    return true;
}

//円柱と円柱の交差判定
bool Collision::IntersectCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outPositionB
)
{
    //Aの足元がBの頭より上なら当たっていない
    if (positionA.y > positionB.y + heightB)
    {
        return false;
    }

    //Aの頭がBの足元より下なら当たっていない
    if (positionA.y + heightA < positionB.y)
    {
        return false;
    }
    //    //カイトウ
    ////XZ平面での範囲チェック
    //    float vx = positionB.x - positionA.x;
    //    float vz = positionB.z - positionA.z;
    //    float rangeA = radiusA + radiusB;
    //    float distXZ = sqrtf(vx * vx + vz * vz);
    //    if (distXZ > rangeA)
    //    {
    //        return false;
    //    }
    //    //AがBを押し出す
    //    vx /= distXZ;
    //    vz /= distXZ;
    //    outPositionB.x = positionA.x + (vx * rangeA);
    //    outPositionB.y = positionA.y;
    //    outPositionB.z = positionA.z + (vz * rangeA);
    //
    //    return true;

        //自分
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);  //引き算で方向を求めるa->b
    Vec = DirectX::XMVectorSetY(Vec, 0); //yだけ0に
    //DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = スクエア(平方根)    2乗していると考えて良い

    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = スクエア(平方根)    2乗していると考えて良い
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //距離測定
    float range = radiusA + radiusB;
    if (range * range < lengthSq)
    {
        return false;
    }

    Vec = DirectX::XMVector3Normalize(Vec);     //正規化して単位ベクトルに
    Vec = DirectX::XMVectorScale(Vec, range);   //AとBの距離分のベクトル作成

    PositionB = DirectX::XMVectorAdd(PositionA, Vec);   //A->Bに押し出すので、

    outPositionB.x = DirectX::XMVectorGetX(PositionB);
    outPositionB.y = 0;
    outPositionB.z = DirectX::XMVectorGetZ(PositionB);

    return true;
}

//球と円柱の交差判定
bool Collision::IntersectSphereVsCylider(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition,
    float cylinderRadius,
    float cylinderHeight,
    DirectX::XMFLOAT3& outCylinderPosition
)
{
    //長方形と球を原点に持っていく
    DirectX::XMFLOAT3 spherePos0 = {
        spherePosition.x - cylinderPosition.x,
         spherePosition.y - cylinderPosition.y,
          spherePosition.z - cylinderPosition.z
    };

    //if (spherePos0.y * spherePos0.y > (cylinderHeight + sphereRadius) * (cylinderHeight + sphereRadius))return false;//y判定
    if (spherePos0.x * spherePos0.x > (cylinderRadius + sphereRadius) * (cylinderRadius + sphereRadius))return false;//x判定
    if (spherePos0.z * spherePos0.z > (cylinderRadius + sphereRadius) * (cylinderRadius + sphereRadius))return false;//z判定

    return true;

    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&spherePosition);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&cylinderPosition);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);  //引き算で方向を求めるa->b
    //DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = スクエア(平方根)    2乗していると考えて良い
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);  //sq = スクエア(平方根)    2乗していると考えて良い
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    PositionB = DirectX::XMVectorAdd(PositionA, Vec);   //A->Bに押し出すので、
    //ポジションAを足し、Vecを足す
    DirectX::XMStoreFloat3(&outCylinderPosition, PositionB);

    return true;
}

//球と円柱の交差判定(回転あり)
bool Collision::IntersectSphereVsCylider(
    const DirectX::XMFLOAT3& spherePosition,
    float sphereRadius,
    const DirectX::XMFLOAT3& cylinderPosition,
    const DirectX::XMFLOAT3& cylinderDir,
    float cylinderRadius,
    float cylinderHeight
)
{
    QuaternionStruct q;
    //上
    DirectX::XMVECTOR Right = { 1,0,0 };
    if (cylinderDir.x * cylinderDir.x - DirectX::XMVectorGetX(Right) * DirectX::XMVectorGetX(Right) < 0.001)
    {
        Right = { 0.9f,0.1f,0 };
    }
    DirectX::XMVECTOR Up = DirectX::XMVector3Cross(Right, DirectX::XMLoadFloat3(&cylinderDir));
    DirectX::XMFLOAT3 up;
    DirectX::XMStoreFloat3(&up, Up);
    //上方向に伸びるのでupを前に
    q = q.LookRotation(up, cylinderDir);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&q.dxFloat4));

    DirectX::XMMATRIX P = DirectX::XMMatrixTranslation(cylinderPosition.x, cylinderPosition.y, cylinderPosition.z);
    DirectX::XMMATRIX Mat = R * P;
    DirectX::XMMATRIX InvMat = DirectX::XMMatrixInverse(nullptr, Mat);

    //判定１
    DirectX::XMFLOAT3 spherePos0 = {
        spherePosition.x - cylinderPosition.x,
         spherePosition.y - cylinderPosition.y,
          spherePosition.z - cylinderPosition.z
    };
    float t = DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&spherePos0), R.r[1]).m128_f32[0];
    DirectX::XMVECTOR VerSph = DirectX::XMVectorScale(R.r[1], t);   //円柱の上軸とスフィアの垂直な点
    float leng = DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMVectorAdd(VerSph, DirectX::XMLoadFloat3(&cylinderPosition)), DirectX::XMLoadFloat3(&spherePosition))).m128_f32[0];

    if (leng > cylinderRadius + sphereRadius)return false;

    //判定２
    DirectX::XMVECTOR localSpherePos = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&spherePosition), InvMat);
    //ローカルスフィアを絶対値に
    DirectX::XMFLOAT3 localSphereABS = { fabs(localSpherePos.m128_f32[0]),localSpherePos.m128_f32[1],fabs(localSpherePos.m128_f32[2]) };

    //最近点を求める
    DirectX::XMFLOAT3 nearPos;

    //x
    if (localSphereABS.x * localSphereABS.x > cylinderRadius * cylinderRadius)
        nearPos.x = cylinderRadius;
    else
        nearPos.x = localSphereABS.x;
    //y
    if (localSphereABS.y > cylinderHeight)
        nearPos.y = cylinderHeight;
    else if (localSphereABS.y < 0)
        nearPos.y = 0;
    else
        nearPos.y = localSphereABS.y;
    //z
    if (localSphereABS.z * localSphereABS.z > cylinderRadius * cylinderRadius)
        nearPos.z = cylinderRadius;
    else
        nearPos.z = localSphereABS.z;

    //当たり判定
    float len = DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&nearPos), DirectX::XMLoadFloat3(&localSphereABS))).m128_f32[0];
    if (len < sphereRadius)
    {
        return true;
    }

    return false;
}

//レイとモデルの交差判定
bool Collision::IntersectRayVsModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const Model* model,
    HitResult& result)
{
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    //ワールド空間のレイの長さ
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    const ModelResource* resource = model->GetResource();
    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        //メッシュノード取得
        const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);

        //レイをワールド空間からローカル空間へ変換
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        //レイの長さ
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        //三角形（面）との交差判定
        const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        int materialIndex = -1;
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                //三角形の頂点を抽出
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                //三角形の三辺ベクトルを算出
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                //三角形の法線ベクトルを算出
                DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AB, BC));

                //内積の結果がプラスならば裏向き
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                float dotLen = DirectX::XMVectorGetX(Dot);
                if (dotLen >= 0) continue;   //裏向きの場合continue

                //レイと平面の交点を算出
                DirectX::XMVECTOR SA = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(A, S), N);

                float x = DirectX::XMVectorGetX(SA) / DirectX::XMVectorGetX(Dot);
                if (x<0.0f || x>neart)continue; //交点までの距離が今までに計算した最近距離より
                //大きい時はスキップ
                DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

                //交点が三角形の内側にあるか判定
                //一つ目
                DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB); //三角形の外積
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);  //法線と外積の内積を求める
                float dot1;
                dot1 = DirectX::XMVectorGetX(Dot1);
                if (dot1 < 0)continue;  //三角形の外側ならcontinue

                //二つ目
                DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC); //三角形の外積
                DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);  //法線と外積の内積を求める
                float dot2;
                dot2 = DirectX::XMVectorGetX(Dot2);
                if (dot2 < 0)continue;  //三角形の外側ならcontinue

                //三つ目
                DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA); //三角形の外積
                DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);  //法線と外積の内積を求める
                float dot3;
                dot3 = DirectX::XMVectorGetX(Dot3);
                if (dot3 < 0)continue;  //三角形の外側ならcontinue

                //最近距離を更新
                neart = x;

                //交点を法線を更新
                HitPosition = P;
                HitNormal = N;
                materialIndex = subset.materialIndex;
            }
        }
        if (materialIndex >= 0)
        {
            //ローカル空間からワールド空間へ変換
            DirectX::XMVECTOR WorldPosition =
                DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            //ヒット情報保存
            if (result.distance > distance)
            {
                DirectX::XMVECTOR WorldNormal =
                    DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                //DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                hit = true;
            }
        }
    }

    return hit;
}

// レイVs球
bool Collision::IntersectRayVsSphere(
    const DirectX::XMVECTOR& rayStart,
    const DirectX::XMVECTOR& rayDirection,		// 要正規化
    float rayDist,
    const DirectX::XMVECTOR& spherePos,
    float radius,
    HitResult& result)
{
    DirectX::XMVECTOR ray2sphere = DirectX::XMVectorSubtract(spherePos, rayStart);
    float projection = DirectX::XMVectorGetX(DirectX::XMVector3Dot(ray2sphere, rayDirection));
    float distSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(ray2sphere)) - projection * projection;

    if (distSq < radius * radius)
    {
        float distance = projection - sqrtf(radius * radius - distSq);
        if (distance > 0.0f)
        {
            if (distance < rayDist)
            {
                DirectX::XMVECTOR Position = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, distance));
                DirectX::XMStoreFloat3(&result.position, Position);
                result.distance = distance;
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(Position, spherePos)));

                return true;
            }
        }
    }

    return false;
}

bool Collision::IntersectRayVsOrientedCylinder(const DirectX::XMVECTOR& rayStart, const DirectX::XMVECTOR& rayDirection, float rayDist, const DirectX::XMVECTOR& startCylinder, const DirectX::XMVECTOR& endCylinder, float radius, HitResult& result, DirectX::XMVECTOR* onCenterLinPos)
{
    using namespace DirectX;

    XMVECTOR d = XMVectorSubtract(endCylinder, startCylinder);
    XMVECTOR m = XMVectorSubtract(rayStart, startCylinder);
    XMVECTOR n = XMVectorScale(rayDirection, rayDist);

    float md = XMVectorGetX(XMVector3Dot(m, d));
    float nd = XMVectorGetX(XMVector3Dot(n, d));
    float dd = XMVectorGetX(XMVector3Dot(d, d));

    // 線分全体が円柱の底面・上面に垂直なスラブに対して外側にあるかどうかを判定
    if (md < 0.0f && md + nd < 0.0f) return false;
    if (md > dd && md + nd > dd) return false;

    float nn = XMVectorGetX(XMVector3Dot(n, n));
    float mm = XMVectorGetX(XMVector3Dot(m, m));
    float a = dd * nn - nd * nd;
    float k = mm - radius * radius;
    float c = dd * k - md * md;

    // レイと円柱の軸の平行性をチェック
    const float parallelThreshold = 1e-5f;
    bool isParallel = fabsf(a) < parallelThreshold * dd * nn;

    if (isParallel)
    {
        // レイが円柱の軸に平行な場合
        if (c > 0.0f) return false;  // レイは円柱の外側

        float t;
        if (md < 0.0f)
            t = -md / nd;  // 底面との交差
        else if (md > dd)
            t = (dd - md) / nd;  // 上面との交差
        else
            t = 0.0f;  // レイの始点が円柱内部

        if (t < 0.0f || t > 1.0f) return false;

        result.distance = t * rayDist;
        XMStoreFloat3(&result.position, XMVectorAdd(rayStart, XMVectorScale(rayDirection, result.distance)));
        XMVECTOR closestPoint = XMVectorAdd(startCylinder, XMVectorScale(d, md / dd));
        XMStoreFloat3(&result.normal, XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&result.position), closestPoint)));

        if (onCenterLinPos)
        {
            *onCenterLinPos = closestPoint;
        }
        return true;
    }

    // レイが円柱の軸に平行でない場合
    float mn = XMVectorGetX(XMVector3Dot(m, n));
    float b = dd * mn - nd * md;
    float D = b * b - a * c;  // 判別式

    if (D < 0.0f) return false;  // 実数解がないので交差していない

    // 解の公式により、交点までの距離を算出
    float sqrtD = sqrtf(D);
    float t1 = (-b - sqrtD) / a;
    float t2 = (-b + sqrtD) / a;

    // t1とt2の順序を保証
    if (t1 > t2) std::swap(t1, t2);

    float t;
    if (t1 >= 0.0f && t1 <= 1.0f)
        t = t1;
    else if (t2 >= 0.0f && t2 <= 1.0f)
        t = t2;
    else
        return false;  // 両方の解が範囲外

    float z = md + t * nd;

    // 円柱の高さ内にあるか確認
    if (z < 0.0f)
    {
        // 底面との交差を確認
        t = -md / nd;
        if (t < 0.0f || t > 1.0f) return false;
        if (a * t * t + 2 * b * t + c > 0.0f) return false;
    }
    else if (z > dd)
    {
        // 上面との交差を確認
        t = (dd - md) / nd;
        if (t < 0.0f || t > 1.0f) return false;
        if (a * t * t + 2 * b * t + c > 0.0f) return false;
    }

    // 交差が確定
    result.distance = t * rayDist;
    XMStoreFloat3(&result.position, XMVectorAdd(rayStart, XMVectorScale(rayDirection, result.distance)));
    XMVECTOR closestPoint = XMVectorAdd(startCylinder, XMVectorScale(d, md / dd));
    XMVECTOR hitPoint = XMVectorAdd(m, XMVectorScale(n, t));
    XMVECTOR projection = XMVectorScale(d, XMVectorGetX(XMVector3Dot(hitPoint, d)) / dd);
    XMStoreFloat3(&result.normal, XMVector3Normalize(XMVectorSubtract(hitPoint, projection)));

    if (onCenterLinPos)
    {
        *onCenterLinPos = XMVectorAdd(startCylinder, projection);
    }

    return true;
}

bool Collision::IntersectRayVsBOX(const DirectX::XMVECTOR& rayStart, const DirectX::XMVECTOR& rayDirection, float rayDist, const DirectX::XMMATRIX boxWorldTransform, HitResult& result)
{
    DirectX::XMVECTOR rayEnd = DirectX::XMVectorAdd(rayStart, DirectX::XMVectorScale(rayDirection, rayDist));

    //レイをワールド空間からローカル空間へ変換
    DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, boxWorldTransform);

    DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(rayStart, InverseWorldTransform);
    DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(rayEnd, InverseWorldTransform);
    DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
    DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
    DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
    };
    vertex vertices[24]{
    {{-1, -1,  -1} ,{ 0, 0,-1}}, // 0
    {{-1,  1,  -1} ,{ 0, 0,-1}}, // 1
    {{ 1,  1,  -1} ,{ 0, 0,-1}}, // 2
    {{ 1, -1,  -1} ,{ 0, 0,-1}}, // 3  //手前向き

    {{ 1, -1,  1} ,{ 0, 0, 1}}, // 4
    {{ 1,  1,  1} ,{ 0, 0, 1}}, // 5
    {{-1,  1,  1} ,{ 0, 0, 1}}, // 6
    {{-1, -1,  1} ,{ 0, 0, 1}}, // 7  //奥向き

    {{ 1, -1, -1} ,{ 1, 0, 0}}, // 8
    {{ 1,  1, -1} ,{ 1, 0, 0}}, // 9
    {{ 1,  1,  1} ,{ 1, 0, 0}}, // 10
    {{ 1, -1,  1} ,{ 1, 0, 0}}, // 11  //右向き

    {{-1, -1,  1} ,{-1, 0, 0}}, // 12
    {{-1,  1,  1} ,{-1, 0, 0}}, // 13
    {{-1,  1, -1} ,{-1, 0, 0}}, // 14
    {{-1, -1, -1} ,{-1, 0, 0}}, // 15  //左向き

    {{-1,  1, -1} ,{ 0, 1, 0}}, // 16
    {{-1,  1,  1} ,{ 0, 1, 0}}, // 17
    {{ 1,  1,  1} ,{ 0, 1, 0}}, // 18
    {{ 1,  1, -1} ,{ 0, 1, 0}}, // 19  //上向き

    {{-1, -1,  1} ,{ 0,-1, 0}}, // 20
    {{-1, -1, -1} ,{ 0,-1, 0}}, // 21
    {{ 1, -1, -1} ,{ 0,-1, 0}}, // 22
    {{ 1, -1,  1} ,{ 0,-1, 0}}, // 23  //下向き
    };
    uint32_t indices[36]{
    0,1,2,3,0,2,
    4,5,6,7,4,6,
    8,9,10,11,8,10,
    12,13,14,15,12,14,
    16,17,18,19,16,18,
    20,21,22,23,20,22,
    };

    float neart = rayDist;
    DirectX::XMVECTOR HitPosition;
    DirectX::XMVECTOR HitNormal;
    bool hit = false;

    for (int indexNum = 0; indexNum < 36; indexNum += 3)
    {
        vertex a = vertices[indices[indexNum]];
        vertex b = vertices[indices[indexNum + 1]];
        vertex c = vertices[indices[indexNum + 2]];

        DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
        DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
        DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

        //三角形の三辺ベクトルを算出
        DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
        DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
        DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

        //三角形の法線ベクトルを算出
        DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(AB, BC));

        //内積の結果がプラスならば裏向き
        DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
        float dotLen = DirectX::XMVectorGetX(Dot);
        if (dotLen >= 0) continue;   //裏向きの場合continue
        //レイと平面の交点を算出
        DirectX::XMVECTOR SA = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(A, S), N);

        float x = DirectX::XMVectorGetX(SA) / DirectX::XMVectorGetX(Dot);
        if (x<0.0f || x>neart)continue; //交点までの距離が今までに計算した最近距離より
        //大きい時はスキップ
        DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

        //交点が三角形の内側にあるか判定
        //一つ目
        DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
        DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB); //三角形の外積
        DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);  //法線と外積の内積を求める
        float dot1;
        dot1 = DirectX::XMVectorGetX(Dot1);
        if (dot1 < 0)continue;  //三角形の外側ならcontinue

        //二つ目
        DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
        DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC); //三角形の外積
        DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);  //法線と外積の内積を求める
        float dot2;
        dot2 = DirectX::XMVectorGetX(Dot2);
        if (dot2 < 0)continue;  //三角形の外側ならcontinue

        //三つ目
        DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
        DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA); //三角形の外積
        DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);  //法線と外積の内積を求める
        float dot3;
        dot3 = DirectX::XMVectorGetX(Dot3);
        if (dot3 < 0)continue;  //三角形の外側ならcontinue

        //ローカル空間からワールド空間へ変換
        DirectX::XMVECTOR WorldPosition =
            DirectX::XMVector3TransformCoord(P, boxWorldTransform);
        DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, rayStart);
        DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
        float distance;
        DirectX::XMStoreFloat(&distance, WorldCrossLength);
        if (distance > rayDist)continue;   //レイより長い場合

        //最近距離を更新
        neart = x;

        //交点を法線を更新
        HitPosition = P;
        HitNormal = N;
        hit = true;
    }
    if (hit)
    {
        //ローカル空間からワールド空間へ変換
        DirectX::XMVECTOR WorldPosition =
            DirectX::XMVector3TransformCoord(HitPosition, boxWorldTransform);
        DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, rayStart);
        DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
        float distance;
        DirectX::XMStoreFloat(&distance, WorldCrossLength);

        DirectX::XMVECTOR WorldNormal =
            DirectX::XMVector3TransformNormal(HitNormal, boxWorldTransform);
        result.distance = distance;
        DirectX::XMStoreFloat3(&result.position, WorldPosition);
        DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));

        return true;
    }

    return false;
}

bool Collision::IntersectSphereVsOBB(const DirectX::XMVECTOR spherePos, float radius, DirectX::XMMATRIX boxWorldTransform)
{
    //sphereをboxの子にする
    DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, boxWorldTransform);
    DirectX::XMVECTOR ChildSphereInBox = DirectX::XMVector3TransformCoord(spherePos, InverseWorldTransform);
    DirectX::XMFLOAT3 spherePosFromBox;
    DirectX::XMStoreFloat3(&spherePosFromBox, ChildSphereInBox);

    //スケール取得
    DirectX::XMVECTOR ScaleX = boxWorldTransform.r[0];
    float scaX = DirectX::XMVector3Length(ScaleX).m128_f32[0];
    DirectX::XMVECTOR ScaleY = boxWorldTransform.r[1];
    float scaY = DirectX::XMVector3Length(ScaleY).m128_f32[0];
    DirectX::XMVECTOR ScaleZ = boxWorldTransform.r[2];
    float scaZ = DirectX::XMVector3Length(ScaleZ).m128_f32[0];

    //ボックスと点の最短距離を取得
    auto PointToBoxLengthSq = [](float boxSize, float point)
        {
            float SqLen = 0;   // 長さのべき乗の値を格納
            if (point < -boxSize)
                SqLen += (point + boxSize) * (point + boxSize);
            if (point > boxSize)
                SqLen += (point - boxSize) * (point - boxSize);
            return SqLen;
        };

    //球の中心点とボックスの最短距離
    float nearLengthSq = 0;
    nearLengthSq += PointToBoxLengthSq(scaX, spherePosFromBox.x * scaX);
    nearLengthSq += PointToBoxLengthSq(scaY, spherePosFromBox.y * scaY);
    nearLengthSq += PointToBoxLengthSq(scaZ, spherePosFromBox.z * scaZ);

    //最短点が球の半径以下なら当たっている
    if (nearLengthSq < radius * radius)
        return true;

    return false;
}

bool Collision::IntersectCapsuleVsCylinder(const DirectX::XMVECTOR cap1StartPos, const DirectX::XMVECTOR cap1EndPos, float cap1Radius, const DirectX::XMVECTOR cly2StartPos, const DirectX::XMVECTOR cly2EndPos, float cly2Radius)
{
    DirectX::XMVECTOR dMy = DirectX::XMVectorSubtract(cap1EndPos, cap1StartPos);
    DirectX::XMVECTOR dOther = DirectX::XMVectorSubtract(cly2EndPos, cly2StartPos);
    DirectX::XMVECTOR r = DirectX::XMVectorSubtract(cap1StartPos, cly2StartPos);

    //判定開始
    float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dMy, dMy));
    float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dOther, dOther));
    float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dOther, r));

    bool clynderEdge = false;   //端にある場合

    float t0 = 0.0f, t1 = 0.0f;

    auto Clamp = [](float val, float min, float max)
        {
            if (val < min) return min;
            if (val > max) return max;
            return val;
        };

    if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// 両線分が点に縮退している場合
    {
        t0 = t1 = 0.0f;
    }
    else if (a <= FLT_EPSILON)					// 片方（My）が点に縮退している場合
    {
        t0 = 0.0f;
        t1 = Clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dMy, r));
        if (e <= FLT_EPSILON)					// 片方（Other）が点に縮退している場合
        {
            t1 = 0.0f;
            t0 = Clamp(-c / a, 0.0f, 1.0f);
        }
        else									// 両方が線分
        {
            float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(dMy, dOther));
            float denom = a * e - b * b;

            if (denom != 0.0f)					// 平行確認（平行時は t0 = 0.0f（線分の始端）を仮の初期値として計算をすすめる）
            {
                t0 = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }

            t1 = b * t0 + f;

            if (t1 < 0.0f)						// t1が始端より外側にある場合
            {
                t1 = 0.0f;
                t0 = Clamp(-c / a, 0.0f, 1.0f);
                clynderEdge = true;
            }
            else if (t1 > e)					// t1が終端より外側にある場合
            {
                t1 = 1.0f;
                t0 = Clamp((b - c) / a, 0.0f, 1.0f);
                clynderEdge = true;
            }
            else								// t1が線分上にある場合
            {
                t1 /= e;
            }
        }
    }

    // 最近点算出
    DirectX::XMFLOAT3 p0;
    DirectX::XMFLOAT3 p1;
    DirectX::XMStoreFloat3(&p0, DirectX::XMVectorAdd(cap1StartPos, DirectX::XMVectorScale(dMy, t0)));
    DirectX::XMStoreFloat3(&p1, DirectX::XMVectorAdd(cly2StartPos, DirectX::XMVectorScale(dOther, t1)));

    // 交差判定
    DirectX::XMVECTOR q = { p1.x - p0.x, p1.y - p0.y, p1.z - p0.z };

    //シリンダーの端にある場合
    if (clynderEdge)
    {
        return DirectX::XMVectorGetX(DirectX::XMVector3Dot(q, q)) < cap1Radius * cap1Radius;
    }

    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(q, q)) < (cap1Radius + cly2Radius) * (cap1Radius + cly2Radius);
}

//線分と点の最近点を求める
inline DirectX::XMVECTOR CalcClosestPoint(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& q0)
{
    //ベクトル作成
    DirectX::XMVECTOR p0p1 = DirectX::XMVectorSubtract(p1, p0);	//p0からp1のベクトルを作成
    DirectX::XMVECTOR p0q0 = DirectX::XMVectorSubtract(q0, p0);	//p0からq0のベクトルを作成

    //長さを保存
    float len = DirectX::XMVector3Length(p0p1).m128_f32[0];

    //射影して、p0からp0p1上の,q0に直角な場所までのベクトル(大きさ)を出す
    p0p1 = DirectX::XMVector3Normalize(p0p1);	//射影される側を正規化

    //return DirectX::XMVectorMultiplyAdd(p0p1, DirectX::XMVector3Dot(p0p1, p0q0), p0);

    DirectX::XMVECTOR p0Dot = DirectX::XMVector3Dot(p0p1, p0q0);	//内積を求める(p0q0の長さを求める
    float dist = p0Dot.m128_f32[0];
    if (dist < 0)dist = 0;
    if (dist > len)dist = len;

    DirectX::XMVECTOR pointVec = DirectX::XMVectorScale(p0p1, dist);	//内積分掛ける（p0からq0の直角までの長さを求める

    //最後に原点からもどす
    DirectX::XMVECTOR ansPointVec = DirectX::XMVectorAdd(pointVec, p0);
    return ansPointVec;
}

// 線分と線分の最短距離の二乗を取得する
inline float GetMinDistSq_SegmentSegment(
    const  DirectX::XMVECTOR& point1A,
    const  DirectX::XMVECTOR& point1B,
    const  DirectX::XMVECTOR& point2A,
    const  DirectX::XMVECTOR& point2B,
    DirectX::XMVECTOR* nearPoint1,
    DirectX::XMVECTOR* nearPoint2)
{
    DirectX::XMVECTOR segmentDirection1 = DirectX::XMVectorSubtract(point1B, point1A);
    DirectX::XMVECTOR segmentDirection2 = DirectX::XMVectorSubtract(point2B, point2A);
    DirectX::XMVECTOR r = DirectX::XMVectorSubtract(point1A, point2A);

    float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection1));
    float e = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, segmentDirection2));
    float f = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection2, r));

    float t1 = 0.0f, t2 = 0.0f;

    if (a <= FLT_EPSILON && e <= FLT_EPSILON)	// 両線分が点に縮退している場合
    {
        t1 = t2 = 0.0f;
    }
    else if (a <= FLT_EPSILON)					// 片方（d0）が点に縮退している場合
    {
        t1 = 0.0f;
        t2 = std::clamp(f / e, 0.0f, 1.0f);
    }
    else
    {
        float c = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, r));
        if (e <= FLT_EPSILON)					// 片方（d1）が点に縮退している場合
        {
            t2 = 0.0f;
            t1 = std::clamp(-c / a, 0.0f, 1.0f);
        }
        else									// 両方が線分
        {
            float b = DirectX::XMVectorGetX(DirectX::XMVector3Dot(segmentDirection1, segmentDirection2));
            float denom = a * e - b * b;

            if (denom != 0.0f)					// 平行確認（平行時は t1 = 0.0f（線分の始端）を仮の初期値として計算をすすめる）
            {
                t1 = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            }

            t2 = b * t1 + f;

            if (t2 < 0.0f)						// t1が始端より外側にある場合
            {
                t2 = 0.0f;
                t1 = std::clamp(-c / a, 0.0f, 1.0f);
            }
            else if (t2 > e)					// t1が終端より外側にある場合
            {
                t2 = 1.0f;
                t1 = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
            else								// t1が線分上にある場合
            {
                t2 /= e;
            }
        }
    }

    // 各線分上の最近点算出
    DirectX::XMVECTOR point1 = DirectX::XMVectorAdd(point1A, DirectX::XMVectorScale(segmentDirection1, t1));
    DirectX::XMVECTOR point2 = DirectX::XMVectorAdd(point2A, DirectX::XMVectorScale(segmentDirection2, t2));

    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(point1, point2);

    if (nearPoint1)
    {
        *nearPoint1 = point1;
    }
    if (nearPoint2)
    {
        *nearPoint2 = point2;
    }

    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(vec));
}

bool Collision::IntersectCapsuleVsOBB(const DirectX::XMVECTOR cap1StartPos, const DirectX::XMVECTOR cap1EndPos, float cap1Radius, DirectX::XMMATRIX boxWorldTransform)
{
    DirectX::XMVECTOR BoxPos = boxWorldTransform.r[3];

    DirectX::XMVECTOR BoxFront = boxWorldTransform.r[2];
    DirectX::XMVECTOR BoxRight = boxWorldTransform.r[1];
    DirectX::XMVECTOR BoxUp = boxWorldTransform.r[0];

    DirectX::XMVECTOR BoxFrontM = DirectX::XMVectorScale(boxWorldTransform.r[2], -1);
    DirectX::XMVECTOR BoxRightM = DirectX::XMVectorScale(boxWorldTransform.r[1], -1);
    DirectX::XMVECTOR BoxUpM = DirectX::XMVectorScale(boxWorldTransform.r[0], -1);

    const int size = 12 * 2;

    //各点
    DirectX::XMVECTOR BoxPoint[8] = {   //0:FUR 1:FU-R 2:F-UR 3:F-U-R 4:-FUR 5:-FU-R 6:-F-UR 7:-F-U-R
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFront, DirectX::XMVectorAdd(BoxUp, BoxRight))),
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFront, DirectX::XMVectorAdd(BoxUp, BoxRightM))),
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFront, DirectX::XMVectorAdd(BoxUpM, BoxRight))),
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFront, DirectX::XMVectorAdd(BoxUpM, BoxRightM))),
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFrontM, DirectX::XMVectorAdd(BoxUp, BoxRight))),
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFrontM, DirectX::XMVectorAdd(BoxUp, BoxRightM))),
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFrontM, DirectX::XMVectorAdd(BoxUpM, BoxRight))),
        DirectX::XMVectorAdd(BoxPos, DirectX::XMVectorAdd(BoxFrontM, DirectX::XMVectorAdd(BoxUpM, BoxRightM)))
    };

    DirectX::XMVECTOR BoxEdge[size] = {
        //上
        BoxPoint[0],
        BoxPoint[4],

        BoxPoint[4],
        BoxPoint[5],

        BoxPoint[5],
        BoxPoint[1],

        BoxPoint[1],
        BoxPoint[0],

        //下
        BoxPoint[2],
        BoxPoint[6],

        BoxPoint[6],
        BoxPoint[7],

        BoxPoint[7],
        BoxPoint[3],

        BoxPoint[3],
        BoxPoint[2],

        //側面
        BoxPoint[0],
        BoxPoint[2],

        BoxPoint[4],
        BoxPoint[6],

        BoxPoint[5],
        BoxPoint[7],

        BoxPoint[1],
        BoxPoint[3],
    };

    for (int i = 0; i < size; i += 2)
    {
        DirectX::XMVECTOR nearPos;
        GetMinDistSq_SegmentSegment(cap1StartPos, cap1EndPos, BoxEdge[i], BoxEdge[i + 1], &nearPos, nullptr);

        DirectX::XMFLOAT3 poo;
        DirectX::XMStoreFloat3(&poo, nearPos);
        Graphics::Instance().GetDebugRenderer()->DrawSphere(poo, 1.0f, { 1,0,1,0 });

        if (IntersectSphereVsOBB(nearPos, cap1Radius, boxWorldTransform))
            return true;
    }

    return false;
}