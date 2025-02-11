#include "Utility_.h"

    bool LeftHasRight(const char * const leftWard, const char * const rightWard)
    {
        std::string search = rightWard;
        std::string src = leftWard;

        // è¨ï∂éöÇëÂï∂éöÇ÷ïœä∑ÇµãÊï ÇÇ»Ç≠Ç∑
        ToUpper(search);
        ToUpper(src);

        int i = 0, j = 0;
        while (src[j] != '\0')
        {
            int offset = 0;
            while (search[i] != '\0')
            {
                if (search[i] != src[j + offset])
                {
                    break;
                }
                ++offset;
                ++i;
            }
            if (search[i] == '\0')
            {
                return true;
            }

            i = 0;
            ++j;
        }
        return false;
    }
    bool CopyOnClipboard(const char* text)
    {
        int	bufSize = static_cast<int>(strlen(text)) + 1;
        char* buf;
        // secure global alloc
        HANDLE	hMem = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, bufSize);
        if (!hMem) return false;

        // be can access
        buf = static_cast<char*>(GlobalLock(hMem));
        if (buf)
        {
            strcpy_s(buf, bufSize, text);
            GlobalUnlock(hMem);

            if (OpenClipboard(NULL))
            {
                EmptyClipboard(); // relearse old data
                SetClipboardData(CF_TEXT, hMem);

                CloseClipboard();
                return true;
            }
        }
        return false;
    }
    bool PasteFromClipboard(std::string& text)
    {
        size_t clipboardBufSize = 0;
        if (OpenClipboard(nullptr))
        {
            // get clip board data
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData != nullptr)
            {
                // get buf size
                clipboardBufSize = GlobalSize(hData);
                text.resize(clipboardBufSize);
                char* clipboardText = static_cast<char*>(GlobalLock(hData));
                if (clipboardText == nullptr)
                {
                    CloseClipboard();
                    return false;
                }
                text = clipboardText;
                GlobalUnlock(hData);
                CloseClipboard();
                return true;
            }
        }

        return false;
    }
    void ToLower(std::string& text)
    {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](char c) { return std::tolower(c); }
        );
    }
    void ToUpper(std::string& text)
    {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](char c) { return std::toupper(c); }
        );
    }
    std::string GetTextBetweenBrackets(const char* str, char start, char end)
    {
        // '[' Ç∆ ']' ÇÃà íuÇåüçı
        const char* s = strchr(str, start);
        const char* e = strchr(str, end);

        // óºï˚ÇÃäáå Ç™å©Ç¬Ç©Ç¡ÇΩèÍçá
        if (s != nullptr && e != nullptr && s < e) {
            // start ÇÃéüÇÃï∂éöÇ©ÇÁÅAend ÇÃà íuÇ‹Ç≈ÇÃïîï™ï∂éöóÒÇï‘Ç∑
            return std::string(s + 1, e - s - 1);
        }

        // å©Ç¬Ç©ÇÁÇ»Ç¢èÍçáÇÕãÛï∂éöóÒÇï‘Ç∑
        return "";
    }