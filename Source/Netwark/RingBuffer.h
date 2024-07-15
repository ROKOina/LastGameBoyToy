#pragma once

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
        arr = new T[elems];
        // 配列の開始アドレスを末尾アドレス、開始アドレス変数に保存
        tail = head = arr;
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
        //サイズが最大なら先頭(head)を削除してから入れる
        if (size >= cap)
        {
            arr[headIndex] = data;
            tail = &arr[headIndex];
            //先頭を変更
            headIndex ++;
            headIndex %= cap;
            head = &arr[headIndex];
        }
        else
        {
            int s = (headIndex + size) % cap;

            arr[s] = data;
            tail = &arr[s];
            size++;
        }
        return 0;
    }

    // データ削除
    T* Dequeue()
    {
        if (size > 0)
        {
            headIndex++;
            headIndex %= cap;
            head = &arr[headIndex];
            size--;
        }
        return nullptr;
    }


    // 登録データ数を返す
    size_t GetSize() 
    {
        return size;
    }

    // 指定Indexのデータを返す
    T* GetData(int index)
    {

    }

    // 終了位置のデータを取得
    T* GetTail()
    {
        return tail;
    }
private:
    T* arr = nullptr;
    T* head = nullptr;
    T* tail = nullptr;
    int headIndex = 0;
    size_t cap;     //最大サイズ
    size_t size;    //現在のサイズ
};