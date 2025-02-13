#pragma once

class EventDirectCom;
class EventDirect;
// イベントエディタウインドウ
class EventDirectEditor
{
public:
	EventDirectEditor();
	~EventDirectEditor() = default;

public:
	// 描画
	void OnDraw(std::weak_ptr<GameObject> selectionGameObject);

private:
	// ドロップしたオブジェクトの更新処理
	void DropUpdate(std::shared_ptr<EventDirect>eventDirect);

private:
	std::weak_ptr<EventDirectCom> eventDirectBehavior;

	int selectedEntry = -1;
	bool changeEnable = false;
	bool dragFloatEnable = false;
	bool sequencerEbable = false;
	bool debugUpdateEnable = true;
	bool playEnable = false;
	bool selectLockEnable = false;
};