#include "Framework.h"
#include "ImGuiRender.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneTool.h"
#include <sstream>

//���������Ԋu�ݒ�
static const int syncInterval = 1;

Framework* Framework::instance = nullptr;

//�R���X�g���N�^
Framework::Framework(HWND hWnd)
    : hWnd(hWnd)
    , graphics(hWnd)
    , input(hWnd)
{
    // �C���X�^���X�ݒ�
    _ASSERT_EXPR(instance == nullptr, "already instantiated");
    instance = this;

    //�V�[���̏�����
    SceneManager::Instance().ChangeScene(new SceneTool);

    //IMGUI������
    IMGUI_CTRL_INITIALIZE(hWnd, graphics.GetDevice(), graphics.GetDeviceContext());
}

// �f�X�g���N�^
Framework::~Framework()
{
    //IMGUI�I����
    IMGUI_CTRL_UNINITIALZE();

    //�V�[���I����
    SceneManager::Instance().Clear();
}

// �X�V����
void Framework::Update(float elapsedTime)
{
    // ���͍X�V����
    input.Update();

    //�V�[���X�V����
    SceneManager::Instance().Update(elapsedTime);

    //IMGUI�X�V
    IMGUI_CTRL_CLEAR_FRAME();
}

// �`�揈��
void Framework::Render(float elapsedTime)
{
    // �V�[���`�揈��
    SceneManager::Instance().Render();

    //IMGUI�`��
    IMGUI_CTRL_DISPLAY();

    // �o�b�N�o�b�t�@�ɕ`�悵�������ʂɕ\������B
    graphics.GetSwapChain()->Present(syncInterval, 0);
}

void Framework::CalculateFrameStats()
{
    static int frames = 0;
    static float time_tlapsed = 0.0f;

    frames++;

    if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
    {
        float fps = static_cast<float>(frames); // fps = frameCnt / 1
        float mspf = 1000.0f / fps;
        std::ostringstream outs;
        outs.precision(6);
        outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
        outs << "�@MyFrame";
        SetWindowTextA(hWnd, outs.str().c_str());

        // Reset for next average.
        frames = 0;
        time_tlapsed += 1.0f;
    }
}

int Framework::Run()
{
    MSG msg = {};

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            timer.Tick();
            CalculateFrameStats();

            float elapsedTime = syncInterval == 0
                ? timer.TimeInterval()
                : syncInterval / 60.0f
                ;
            Update(elapsedTime);
            Render(elapsedTime);
        }
    }
    return static_cast<int>(msg.wParam);
}

LRESULT Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //imgui�̃n���h��
    IMGUI_CTRL_WND_PRC_HANDLER(hWnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc;
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;
    case WM_ENTERSIZEMOVE:
        timer.Stop();
        break;
    case WM_EXITSIZEMOVE:
        timer.Start();
        break;
    case WM_MOUSEWHEEL:
        Input::Instance().GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}