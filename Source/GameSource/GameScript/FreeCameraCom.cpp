#include "FreeCameraCom.h"

#include "Components/TransformCom.h"
#include "SystemStruct/QuaternionStruct.h"

#include <windows.h>

// �J�n����
void FreeCameraCom::Start()
{
	DirectX::XMFLOAT3 front = GetGameObject()->transform_->GetWorldFront();
	DirectX::XMFLOAT3 eye;
	eye.x = focusPos.x - front.x * distance;
	eye.y = focusPos.y - front.y * distance;
	eye.z = focusPos.z - front.z * distance;
	GetGameObject()->transform_->SetWorldPosition(eye);

	//���N���X�̃J�������Ă�
	CameraCom::Start();
}

// �X�V����
void FreeCameraCom::Update(float elapsedTime)
{
	//���N���X�̃J�������Ă�
	CameraCom::Update(elapsedTime);

	if (!isActiveCamera)return;

    POINT cursor;
    ::GetCursorPos(&cursor);

	oldCursor = newCursor;
	newCursor = DirectX::XMFLOAT2(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	float moveX = (newCursor.x - oldCursor.x) * 0.02f;
	float moveY = (newCursor.y - oldCursor.y) * 0.02f;

	// Alt�L�[
	if (::GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		//��]
		if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			DirectX::XMFLOAT3 euler= GetGameObject()->transform_->GetEulerRotation();

			euler.y += moveX * 8.0f;
			euler.x += moveY * 8.0f;
			GetGameObject()->transform_->SetEulerRotation(euler);

			GetGameObject()->transform_->UpdateTransform();
		}
		else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
		{
			// ���s�ړ�
			DirectX::XMFLOAT3 right = GetRight();
			DirectX::XMFLOAT3 up = GetUp();

			float x = moveX * 2;
			float y = moveY * 2;

			focusPos.x -= right.x * x;
			focusPos.y -= right.y * x;
			focusPos.z -= right.z * x;

			focusPos.x += up.x * y;
			focusPos.y += up.y * y;
			focusPos.z += up.z * y;
		}
		else if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			// �Y�[��
			distance += (-moveY - moveX) * distance * 0.1f;
			if (distance < 0.1f)
				distance = 0.1f;
		}

		DirectX::XMFLOAT3 front = GetGameObject()->transform_->GetWorldFront();

		DirectX::XMFLOAT3 eye;
		eye.x = focusPos.x - front.x * distance;
		eye.y = focusPos.y - front.y * distance;
		eye.z = focusPos.z - front.z * distance;

		GetGameObject()->transform_->SetWorldPosition(eye);
	}
}

// GUI�`��
void FreeCameraCom::OnGUI()
{
	//���N���X�̃J�������Ă�
	CameraCom::OnGUI();
}
