#include "Common-cpp/inc/Common.h"
#include "BasicsApplication.h"
#include "UIListener.h"
#include "Photon_lib.h"
#include "imgui.h"

using namespace ExitGames::Common;

// ログ出力
void PrintLog(const char* format, ...)
{
	char message[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);

	::OutputDebugStringA(message);
}

PhotonLib* photonLib;
UIListener* listenerI;
BasicsApplication::BasicsApplication(UIListener* listener)
{
	photonLib = new PhotonLib(listener);
	listenerI = listener;
}


void BasicsApplication::run(/*UIListener* listener*/)
{
	//while(!listener->anyKeyPressed())
	//{
	static JString lastStateString;
	photonLib->update();
	auto Jstring = photonLib->getStateString();
	if (Jstring.length() && Jstring != lastStateString)
	{
		PrintLog(WStringToString(Jstring.cstr()).c_str());
	}

	//listener->writeString(lastStateString=lib.getStateString());
	//SLEEP(100);
	//}
	//listener->onLibClosed();
}

void BasicsApplication::close()
{
	listenerI->onLibClosed();
	delete listenerI;
	delete photonLib;
}

void BasicsApplication::ImGui()
{
	//ネットワーク決定仮ボタン
	ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	ImGui::Begin("PhotonNet", nullptr, ImGuiWindowFlags_None);

	//ID表示
	int photonID = photonLib->GetPlayerNum();
	ImGui::InputInt("photonID", &photonID);

	//状態表示
	std::string nowState = photonLib->stateStr[photonLib->GetPhotonState()];
	ImGui::Text(("State : " + nowState).c_str());


	ImGui::End();
}
