#include "Common-cpp/inc/Common.h"
#include "BasicsApplication.h"
#include "UIListener.h"
#include "Photon_lib.h"
#include "imgui.h"

using namespace ExitGames::Common;

// ���O�o��
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
	//�l�b�g���[�N���艼�{�^��
	ImGui::SetNextWindowPos(ImVec2(30, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	ImGui::Begin("PhotonNet", nullptr, ImGuiWindowFlags_None);

	//ID�\��
	int photonID = photonLib->GetPlayerNum();
	ImGui::InputInt("photonID", &photonID);

	//��ԕ\��
	std::string nowState = photonLib->stateStr[photonLib->GetPhotonState()];
	ImGui::Text(("State : " + nowState).c_str());

	ImGui::Separator();

	//���[�����O
	ImGui::Text(("RoomName : " + photonLib->GetRoomName()).c_str());

	//���[���l���\��
	int roomCount = photonLib->GetRoomPlayersNum();
	ImGui::InputInt("roomPlayersNum", &roomCount);


	//�T�[�o�[����
	int serverTime = photonLib->GetServerTime();
	int serverTimeOF = photonLib->GetServerTimeOffset();
	ImGui::InputInt("serverTime", &serverTime);

	int A = serverTime / 1000;
	A %= 10;
	ImGui::InputInt("A", &A);


	//���ԃf�o�b�O��r�p
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
