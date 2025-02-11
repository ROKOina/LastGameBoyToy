#pragma once
#define U(str) reinterpret_cast<const char*>(u8##str)
#define CLASS_NAME(CLASS) virtual const char* ClassName() { return #CLASS; }\
                          static const char* StaticClassName() { return #CLASS; }
#define GET_CLASS_NAME(CLASS) #CLASS

    template<size_t N>
    class String
    {
    public:
        std::string string = std::string(N, '\0');
    };
    /**************************************************************************//**
        @brief	任意の文字列が任意の文字列を含むかを調べる
        @param  [in]    leftWard    rightWardを含む可能性のある文字列
        @param  [in]    rightWard   含まれるか調べたい文字列
        @return leftWardの中に rightWardと一致する文字列が見つかった場合 true
        @return leftWardの中に rightWardと一致する文字列が見つからなかった場合 false
        @par    [詳細]
                この関数は大文字と小文字を区別しません。
                LeftHasRight("ABC", "abc") はtrueです。
    *//***************************************************************************/
    bool LeftHasRight(const char* const leftWard, const char* const rightWard);

    /**************************************************************************//**
        @brief	クリップボードにテキストをコピーする
        @param  [in]    text    コピーする文字列
        @return コピーに成功したら true,失敗したら false
    *//***************************************************************************/
    bool CopyOnClipboard(const char* text);

    /**************************************************************************//**
        @brief	クリップボードからテキストをペーストする
        @param  [in]    text    ペーストした結果を保存するバッファー
        @return コピーに成功したら true,失敗したら false
    *//***************************************************************************/
    bool PasteFromClipboard(std::string& text);

    /**************************************************************************//**
        @brief	文字列をすべて小文字に置き換える
        @param  [inout] text    置き換えを行うテキスト
    *//***************************************************************************/
    void ToLower(std::string& text);

    /**************************************************************************//**
        @brief	文字列をすべて大文字に置き換える
        @param  [inout] text    置き換えを行うテキスト
    *//***************************************************************************/
    void ToUpper(std::string& text);


    template <class T>
    bool IsContain(const std::vector<std::weak_ptr<T>>& vector, const std::shared_ptr<T> searchObject)
    {
        for (const std::weak_ptr<T>& e : vector)
        {
            if (e.lock() == searchObject)
            {
                return true;
            }
        }
        return false;
    }
    template <class T>
    bool IsContain(const std::list<std::weak_ptr<T>>& list, const std::shared_ptr<T> searchObject)
    {
        for (const std::weak_ptr<T>& e : list)
        {
            if (e.lock() == searchObject)
            {
                return true;
            }
        }
        return false;
    }

    template <class T>
    bool IsContain(const std::vector<T>& vector, const T& searchObject)
    {
        for (const T& e : vector)
        {
            if (e == searchObject)
            {
                return true;
            }
        }
        return false;
    }
    template <class T>
    bool IsContain(const std::list<T>& list, const T& searchObject)
    {
        for (const T& e : list)
        {
            if (e == searchObject)
            {
                return true;
            }
        }
        return false;
    }

    std::string GetTextBetweenBrackets(const char* str, char start, char end);
    enum class VARIABLE_TYPE
    {
        INT,
        FLOAT,
        DOUBLE,
        LONG_LONG,
        FUNCTION,

        MAX,
    }; 
    union Any
    {
        int                 i = 0;
        long long           ll;
        float               f;
        double              d;
    };
