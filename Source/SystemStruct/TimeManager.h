#pragma once

//�S�Ă̎��Ԃ��Ǘ����Ă���N���X
class TimeManager
{
private:
    TimeManager() {}

public:

    //�C���X�^���X�擾
    static TimeManager& Instance()
    {
        static TimeManager instance;
        return instance;
    }

    //�X�V����
    void Update(const float& elapsedTime);

    //�Z�b�^�[
    void SetTimeEffect(const float& scale, const float& time)
    {
        m_timescale = scale;
        m_effecttime = time;
    }

    //�Q�b�^�[
    float GetElapsedTime(const float& elapsedTime)
    {
        return elapsedTime * m_timescale;
    }

    float GetElapsedTime() { return m_elapsedtime; }

private:
    float m_timescale = 1.0f;
    float m_effecttime = 0.0f;
    float m_elapsedtime = 0.0f;
};