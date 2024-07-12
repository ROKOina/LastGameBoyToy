#pragma once

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
        arr = new T[elems];
        // �z��̊J�n�A�h���X�𖖔��A�h���X�A�J�n�A�h���X�ϐ��ɕۑ�
        tail = head = arr;
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
        //�T�C�Y���ő�Ȃ�擪(head)���폜���Ă�������
        if (size >= cap)
        {
            arr[headIndex] = data;
            tail = &arr[headIndex];
            //�擪��ύX
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

    // �f�[�^�폜
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


    // �o�^�f�[�^����Ԃ�
    size_t GetSize() 
    {
        return size;
    }

    // �w��Index�̃f�[�^��Ԃ�
    T* GetData(int index)
    {

    }

    // �I���ʒu�̃f�[�^���擾
    T* GetTail()
    {
        return tail;
    }
private:
    T* arr = nullptr;
    T* head = nullptr;
    T* tail = nullptr;
    int headIndex = 0;
    size_t cap;     //�ő�T�C�Y
    size_t size;    //���݂̃T�C�Y
};