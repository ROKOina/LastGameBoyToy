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


void BasicsApplication::run(float elapsedTime)
{
	static JString lastStateString;
	photonLib->update(elapsedTime);
	auto Jstring = photonLib->getStateString();
	if (Jstring.length() && Jstring != lastStateString)
	{
		//PrintLog(WStringToString(Jstring.cstr()).c_str());
	}
}

void BasicsApplication::close()
{
	listenerI->onLibClosed();
	delete listenerI;
	delete photonLib;
}

void BasicsApplication::ImGui()
{
	photonLib->ImGui();
}
