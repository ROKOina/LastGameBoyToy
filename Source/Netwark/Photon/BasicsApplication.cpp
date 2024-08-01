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
		//PrintLog(WStringToString(Jstring.cstr()).c_str());
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

	ImGui::Separator();

	//ルーム名前
	ImGui::Text(("RoomName : " + photonLib->GetRoomName()).c_str());

	//ルーム人数表示
	int roomCount = photonLib->GetRoomPlayersNum();
	ImGui::InputInt("roomPlayersNum", &roomCount);


	//サーバー時間
	int serverTime = photonLib->GetServerTime();
	int serverTimeOF = photonLib->GetServerTimeOffset();
	ImGui::InputInt("serverTime", &serverTime);

	int A = serverTime / 1000;
	A %= 10;
	ImGui::InputInt("A", &A);


	//時間デバッグ比較用
	//serverTime += serverTimeOF;
	if (serverTime != 0)
	{
		int slTime = 10000;
		static int saveTime = 0;
		static int oldSerTime = serverTime;
		saveTime += serverTime - oldSerTime;
		oldSerTime = serverTime;
		static bool reverseB = true;
		if (saveTime > slTime)
		{
			saveTime = 0;
			reverseB = !reverseB;
		}
		if (saveTime < 0)saveTime = 0;
		ImGui::InputInt("saveTime", &saveTime);
		ImGui::Checkbox("SerVV", &reverseB);
	}


	ImGui::InputInt("serverTimeOFF", &serverTimeOF);

	int roundTime = photonLib->GetRoundTripTime();
	int roundTimeV = photonLib->GetRoundTripTimeVariance();
	ImGui::InputInt("roundTime", &roundTime);

	std::vector<int> trips = photonLib->GetTrips();
	for (int i = 0; i < trips.size(); ++i)
	{
		ImGui::InputInt(("trip" + std::to_string(i)).c_str(), &trips[i]);
	}

	int sendMs = photonLib->SendMs();
	ImGui::InputInt("sendMS", &sendMs);


	ImGui::End();
}
