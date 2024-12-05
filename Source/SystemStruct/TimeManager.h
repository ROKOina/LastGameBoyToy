#pragma once

//全ての時間を管理しているクラス
class TimeManager
{
private:
    TimeManager() {}

public:

    //インスタンス取得
    static TimeManager& Instance()
    {
        static TimeManager instance;
        return instance;
    }

    //更新処理
    void Update(const float& elapsedTime);

    //セッター
    void SetTimeEffect(const float& scale, const float& time)
    {
        m_timescale = scale;
        m_effecttime = time;
    }

    //ゲッター
    float GetElapsedTime(const float& elapsedTime)
    {
        return elapsedTime * m_timescale;
    }

    float GetElapsedTime() { return m_elapsedtime; }

    // GetDeltaTimeメソッド
    float TimeManager::GetDeltaTime() const
    {
        return m_elapsedtime * m_timescale; // タイムスケールを考慮して返す
    }

private:
    float m_timescale = 1.0f;
    float m_effecttime = 0.0f;
    float m_elapsedtime = 0.0f;
};