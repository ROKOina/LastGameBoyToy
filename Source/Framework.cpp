#include <memory>
#include <sstream>
#include <tchar.h>

#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Framework.h"
#include "GameSource\Scene\SceneGame.h"
#include "GameSource\Scene\SceneManager.h"
#include "ImGuiRender.h"
#include <imgui.h>
#include <ImGuizmo.h>

// ���������Ԋu�ݒ�
static const int syncInterval = 1;

// �R���X�g���N�^
Framework::Framework(HWND hWnd)
    : hWnd_(hWnd)
    , input_(hWnd)
    , graphics_(hWnd)
{
    //sceneGame.Initialize();
    SceneManager::Instance().ChangeScene(new SceneGame);
    //SceneManager::Instance().ChangeScene(new SceneTitle);
    //SceneManager::Instance().ChangeScene(new SceneResult);

    //IMGUI������
    IMGUI_CTRL_INITIALIZE(hWnd_, graphics_.GetDevice(), graphics_.GetDeviceContext());
}

// �f�X�g���N�^
Framework::~Framework()
{
    //sceneGame.Finalize();
    SceneManager::Instance().Clear();

    //IMGUI�I����
    IMGUI_CTRL_UNINITIALZE();
}

#include "Logger.h"
clock_t startTime1 = 0, endTime1 = 0;

// �X�V����
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/, float fps_)
{
    // ���͍X�V����
    input_.Update();

    // �V�[���X�V����
    SceneManager::Instance().Update(elapsedTime);

    //IMGUI�X�V
    IMGUI_CTRL_CLEAR_FRAME();
}

// �`�揈��
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
    //�ʃX���b�h���Ƀf�o�C�X�R���e�L�X�g���g���Ă����ꍇ��
    //�����A�N�Z�X���Ȃ��悤�ɔr�����䂷��
    std::lock_guard<std::mutex> lock(graphics_.GetMutex());

    ID3D11DeviceContext* dc = graphics_.GetDeviceContext();

    //imguiguizmo
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    RECT r;
    ::GetWindowRect(hWnd_, &r);
    ImGuizmo::SetRect(static_cast<float>(r.left), static_cast<float>(r.top), static_cast<float>(r.right) - static_cast<float>(r.left), static_cast<float>(r.bottom) - static_cast<float>(r.top));

    // �V�[���`�揈��
    SceneManager::Instance().Render(elapsedTime);

    //IMGUI�`��
    IMGUI_CTRL_DISPLAY();

    // �o�b�N�o�b�t�@�ɕ`�悵�������ʂɕ\������B
    graphics_.GetSwapChain()->Present(syncInterval, 0);
}

// �t���[�����[�g�v�Z
void Framework::CalculateFrameStats()
{
    // Code computes the average frames per second, and also the
    // average time it takes to render one frame.  These stats
    // are appended to the window caption bar.
    static int frames = 0;
    static float time_tlapsed = 0.0f;

    frames++;

    // Compute averages over one second period.
    if ((timer_.TimeStamp() - time_tlapsed) >= 1.0f)
    {
        float fps = static_cast<float>(frames); // fps = frameCnt / 1
        float mspf = 1000.0f / fps;
        std::ostringstream outs;
        outs.precision(6);
        outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
        SetWindowTextA(hWnd_, outs.str().c_str());

        // Reset for next average.
        frames = 0;
        time_tlapsed += 1.0f;

        //FPS���Z�b�g
        Graphics::Instance().SetFPSFramework(fps);
    }
}

// �A�v���P�[�V�������[�v
int Framework::Run()
{
    MSG msg = {};

    //�I���R�[�h�Ȃ�while�ʂ���@
    while (WM_QUIT != msg.message)
    {
        //PeekMessage = ���b�Z�[�W��M
        //���b�Z�[�W���擾���Ȃ������ꍇ�A0 ���Ԃ�
        //��3,4�����A����0�̏ꍇ���ׂẴ��b�Z�[�W��Ԃ�
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	//PM_REMOVE = PeekMessage �֐������b�Z�[�W������������A���̃��b�Z�[�W���L���[����폜
        {
            TranslateMessage(&msg);	//���b�Z�[�W�ϊ�
            DispatchMessage(&msg);	//�v���V�[�W���փR�[���o�b�N
        }
        else
        {
            timer_.Tick();
            CalculateFrameStats();

            float elapsedTime = syncInterval == 0
                ? timer_.TimeInterval()
                : syncInterval / 60.0f
                //: syncInterval / fps_
                ;

            startTime1 = clock();

            Update(elapsedTime, fps_);
            Render(elapsedTime);

            endTime1 = clock();
            //Logger::Print((std::string("Frame Time : ") + std::to_string(endTime1 - startTime1) + "\n\n").c_str());
        }
    }

    //// ComPtr�p���[�N�^���\���@�쐬
    //typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
    //HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
    //fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");

    //DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&debugGI_);

    //debugGI_->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_DETAIL);

    //Graphics::Instance().GetDevice()->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugID_);
    //debugID_->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
    //debugID_->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

    return static_cast<int>(msg.wParam);
}

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
        timer_.Stop();
        break;
    case WM_EXITSIZEMOVE:
        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
        // Here we reset everything based on the new window dimensions.
        timer_.Start();
        break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}