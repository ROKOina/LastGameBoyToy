#pragma once
#include "../System\Component.h"
#include "Audio\Audio.h"
#include "Audio\AudioSource.h"

class AudioObj
{
public:
    void Update();

    void Play() { source->Play(loop, volume); };
    void Stop() { source->Stop(); }

    void FeedStart(float targetValue, float add);
    bool Feed();

    std::shared_ptr<AudioSource> GetSource() { return source; }
    void SetSource(std::shared_ptr<AudioSource> s) { source = s; }

    void SetVolume(float value) { volume = value; }
    void SetPitch(float value) { pitch = value; }
    void SetLoopFlag(bool flag) { loop = flag; }

    bool GetIsSource() { return source != nullptr; }
    bool GetFeedFlag() { return feedFlag; }

private:
    std::shared_ptr<AudioSource> source;

    float volume = 1.0f;
    float pitch = 1.0f;
    bool loop = true;

    //���ʕύX�p�ϐ�
    float feedAddValue = 0.0f;
    float feedTargetValue = 0.0f;
    float feedTime = 0.0f;
    bool feedFlag = false;
};

class AudioCom : public Component
{
public:
    AudioCom() {};
    AudioCom(AUDIOID id, std::string name);
    ~AudioCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Audio"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    //Audio�̓o�^
    void RegisterSource(AUDIOID id, std::string name);
    void RegisterSource(std::string name, std::shared_ptr<AudioSource> source) { audioSources[name].SetSource(source); }

    void Play(std::string name) { audioSources[name].Play(); }
    void Play(std::string name, bool loop, float value)
    {
        audioSources[name].SetVolume(value);
        audioSources[name].SetLoopFlag(loop);
        audioSources[name].Play();
    }

    void Stop(std::string name) { audioSources[name].Stop(); }
    void FeedStart(std::string name, float target, float add) { audioSources[name].FeedStart(target, add); }

    AudioSource* GetAudioSource(std::string name) { return audioSources[name].GetSource().get(); }
    AudioObj& GetAudioObj(std::string name) { return audioSources[name]; }

private:
    std::map<std::string, AudioObj> audioSources;
};
