#pragma once

#include <deque>

// リングバッファクラス
template<class T>
class RingBuffer {
public:
    // コンストラクタ
    RingBuffer() {};
    RingBuffer(const size_t elems)
    {
        // 最大サイズを保存
        cap = elems;
        // サイズ分で配列作成
        arr.resize(elems);
        //arr = new T[elems];
        // 配列の開始アドレスを末尾アドレス、開始アドレス変数に保存
        head = tail = &arr[0];
        // 開始インデックス番号
        headIndex = 0;
        // 保存サイズ
        size = 0;
    }
    // デストラクタ
    ~RingBuffer()
    {
        //// メモリ解放
        //delete[] arr;
    }

    // データ登録
    int Enqueue(const T& data)
    {
        headIndex++;
        headIndex %= cap;

        arr[headIndex] = data;
        head = &arr[headIndex];

        //サイズが最大なら末尾(tail)を削除してから入れる
        if (size >= cap)
        {
            int s = headIndex + 1;
            s %= cap;

            //末尾を変更
            tail = &arr[s];
        }
        else
        {
            size++;
        }
        return 0;
    }

    // データ削除
    const T* Dequeue()
    {
        if (size > 0)
        {
            int s = cap + headIndex - size + 2;
            s %= cap;

            tail = &arr[s];
            size--;
        }
        return nullptr;
    }


    // 登録データ数を返す
    size_t GetSize() 
    {
        return size;
    }

    // 終了位置のデータを取得
    const T& GetTail()
    {
        if (size <= 0)return T();
        return *tail;
    }

    // 開始位置のデータを取得
    const T& GetHead()
    {
        if (size <= 0)return T();
        return *head;
    }

    //最初位置から引数サイズ文のvetorを返す
    std::vector<T> GetHeadFromSize(int size)
    {
        std::vector<T> v;
        for (int i = 0; i < size; ++i)
        {
            if (this->size <= i)break;
            int index = this->cap + headIndex - i;
            index %= this->cap;
            v.emplace_back(arr[index]);
        }
        return v;
    }

private:
    std::deque<T> arr;
    T* tail;
    T* head;
    //T* arr = nullptr;
    //T* head = nullptr;
    //T* tail = nullptr;
    int headIndex = 0;
    size_t cap;     //最大サイズ
    size_t size;    //現在のサイズ
};