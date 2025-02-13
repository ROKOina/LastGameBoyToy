#pragma once

#include "EventMoveBase.h"
#include "ImGuiRender.h"
#include "ImCurveEdit.h"

// イベントのベース（派生させてゲームオブジェクトなどのイベント制御アイテムを作成する）
class EventItemBase
{
public:
	EventItemBase() {};
	virtual ~EventItemBase() {}

	//! @brief  イベントの開始点を秒数で返します
	float GetEventStartSec() const { return static_cast<float>(_Start) / 60.0f; }

	//! @brief  イベントの終了点を秒数で返します
	float GetEventEndSec() const { return static_cast<float>(_End) / 60.0f; }
public:
	int _Start{};
	int _End{};
#ifdef DEBUG
	// シーケンサーで使用する変数
	bool expanded{};
	int color = 0xFFAA8080;// デフォルトカラー
	void SetEroorColor() { color = 0x0000FF; }
#endif
	VARIABLE_TYPE _Type = VARIABLE_TYPE::INT;
	Any _Value;
	// 関数保存用変数
	std::string _Function = "";
	bool _FunctionDecisionEnable = false;

public:
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& archive, const uint32_t version)
	{
		if (version == 0)
		{
			archive(
				_Type,
				_Start,
				_End,
				_Value.i,
				_Function
			);
		}
	}
};
CEREAL_CLASS_VERSION(EventItemBase, 0)

// 動きのコンポーネントのイベント
class EventMoveParameterBase : public EventItemBase
{
public:
	EventMoveParameterBase() {};
	virtual ~EventMoveParameterBase() {};
public:
	std::weak_ptr<EventMoveParameterBehaviorBase> _event;

public:
	//TODO: ポインターでの保存をしないように
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& archive, const uint32_t version)
	{
		if (version == 0)
		{
			archive(
				cereal::base_class<EventItemBase>(this),
				_event
			);
		}
	}
};
CEREAL_REGISTER_TYPE(EventMoveParameterBase)
CEREAL_CLASS_VERSION(EventMoveParameterBase, 0)

struct EventDirect : public ImSequencer::MySequenceInterface
{
	// シーケンサーで使用するゲッター関数
	int GetFrameMin() const override { return 0; }
	int GetFrameMax() const override { return _FlameLength; }
	int GetItemCount() const override { return static_cast<int>(_EventItems.size()); }

	size_t GetCustomHeight(int index) override
	{
#ifdef DEBUG
		return _EventItems[EventNames_[index]]->expanded ? 300 : 0;
#endif // DEBUG
		return  0;
	}

	const char* GetItemTypeName(int index) const override
	{
		static char tmps[512];
		sprintf_s(tmps, EventNames_[index].c_str());
		return tmps;
	}
	virtual const char* GetItemLabel(int index) const override
	{
		static char tmps[512];
		sprintf_s(tmps, "[%02d] %s", index, EventNames_[index].c_str());
		return tmps;
	}

	void Get(int index, int** start, int** end, unsigned int* color) override;

	void Add(int index) override
	{
#ifdef DEBUG
		std::string& name = EventNames_.emplace_back();
		name = "new event " + std::to_string(index);
		std::shared_ptr<EventItemBase> eventItemBase = std::make_shared<EventItemBase>();
		eventItemBase->expanded = false;
		eventItemBase->_Start = 0;
		eventItemBase->_End = 10;
		_EventItems.emplace(name, std::move(eventItemBase));
#endif // DEBUG
	};

	void AddEventMoveParameterBehaviorBaseEvent(std::shared_ptr<EventMoveParameterBase> eventComponent)
	{
		std::string& name = EventNames_.emplace_back();

		// クラス名の取得
		std::string className = eventComponent->_event.lock()->GetClassName_();
		name = className + std::to_string(EventNames_.size());
		_EventItems.emplace(name, eventComponent);
	}

	void Del(int index) override
	{
		std::string& key = EventNames_[index];
		auto it = _EventItems.find(key);
		if (it != _EventItems.end())
		{
			if (auto inflexionEvent = std::dynamic_pointer_cast<EventMoveParameterBase>(it->second))
			{
				// イベントの管理から解放させる
				if (inflexionEvent->_event.lock())
				{
					inflexionEvent->_event.lock()->m_eventUpdateEnable = false;
				}
			}

			_EventItems.erase(it);
			EventNames_.erase(EventNames_.begin() + index);
		}
	}

	void Duplicate(int index) override
	{
		std::string key = EventNames_[index];
		const std::shared_ptr<EventItemBase>& event = _EventItems[key];
		key += "_COPY";
		_EventItems.emplace(key, event);
		EventNames_.emplace_back(key);
	}
	//#endif

	float GetFlameLengthSec() const { return static_cast<float>(_FlameLength) / 60.0f; }

public:
	std::vector<std::string> EventNames_;
	std::unordered_map<std::string, std::shared_ptr<EventItemBase>> _EventItems{};

	int _FlameLength{};

	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& archive, const uint32_t version)
	{
		if (version == 0)
		{
			archive(
				EventNames_,
				_EventItems,
				_FlameLength
			);
		}
	}
};
CEREAL_CLASS_VERSION(EventDirect, 0)