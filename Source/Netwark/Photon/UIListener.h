#pragma once

#include <memory>
#include "Common-cpp/inc/Common.h"

static std::string WStringToString
(
    std::wstring oWString
)
{
    // wstring �� SJIS
    int iBufferSize = WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str()
        , -1, (char*)NULL, 0, NULL, NULL);

    // �o�b�t�@�̎擾
    CHAR* cpMultiByte = new CHAR[iBufferSize];

    // wstring �� SJIS
    WideCharToMultiByte(CP_OEMCP, 0, oWString.c_str(), -1, cpMultiByte
        , iBufferSize, NULL, NULL);

    // string�̐���
    std::string oRet(cpMultiByte, cpMultiByte + iBufferSize - 1);

    // �o�b�t�@�̔j��
    delete[] cpMultiByte;

    // �ϊ����ʂ�Ԃ�
    return(oRet);
}

class UIListener
{
public:
	virtual ~UIListener(void){};
	virtual void writeString(const ExitGames::Common::JString& str) = 0;
	virtual bool anyKeyPressed(void) const = 0;
	virtual void onLibClosed(void) = 0;
};