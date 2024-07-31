#pragma once

#include <deque>

// �����O�o�b�t�@�N���X
template<class T>
class RingBuffer {
public:
    // �R���X�g���N�^
    RingBuffer() {};
    RingBuffer(const size_t elems)
    {
        // �ő�T�C�Y��ۑ�
        cap = elems;
        // �T�C�Y���Ŕz��쐬
        arr.resize(elems);
        //arr = new T[elems];
        // �z��̊J�n�A�h���X�𖖔��A�h���X�A�J�n�A�h���X�ϐ��ɕۑ�
        head = tail = &arr[0];
        // �J�n�C���f�b�N�X�ԍ�
        headIndex = 0;
        // �ۑ��T�C�Y
        size = 0;
    }
    // �f�X�g���N�^
    ~RingBuffer()
    {
        //// ���������
        //delete[] arr;
    }

    // �f�[�^�o�^
    int Enqueue(const T& data)
    {
        headIndex++;
        headIndex %= cap;

        arr[headIndex] = data;
        head = &arr[headIndex];

        //�T�C�Y���ő�Ȃ疖��(tail)���폜���Ă�������
        if (size >= cap)
        {
            int s = headIndex + 1;
            s %= cap;

            //������ύX
            tail = &arr[s];
        }
        else
        {
            size++;
        }
        return 0;
    }

    // �f�[�^�폜
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


    // �o�^�f�[�^����Ԃ�
    size_t GetSize() 
    {
        return size;
    }

    // �I���ʒu�̃f�[�^���擾
    const T& GetTail()
    {
        if (size <= 0)return T();
        return *tail;
    }

    // �J�n�ʒu�̃f�[�^���擾
    const T& GetHead()
    {
        if (size <= 0)return T();
        return *head;
    }

    //�ŏ��ʒu��������T�C�Y����vetor��Ԃ�
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
    size_t cap;     //�ő�T�C�Y
    size_t size;    //���݂̃T�C�Y
};