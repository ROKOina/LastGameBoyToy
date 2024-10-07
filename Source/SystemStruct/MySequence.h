#pragma once
#include "ImGuiRender.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include "ImCurveEdit.h"
#include <vector>
#include <string>

struct MySequence : public ImSequencer::SequenceInterface
{
    // interface with sequencer

    virtual int GetFrameMin() const {
        return mFrameMin;
    }
    virtual int GetFrameMax() const {
        return mFrameMax;
    }
    virtual int GetItemCount() const { return (int)myItems.size(); }

    virtual void IntGet(int index, int** start, int** end, int* type, unsigned int* color) 
    {
        MySequenceItem& item = myItems[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.mFrameStart;
        if (end)
            *end = &item.mFrameEnd;
        if (type)
            *type = item.mType;
    }
    virtual void FloatGet(int index, float** start, float** end, int* type, unsigned int* color) 
    {
        MySequenceItem& item = myItems[index];
        if (color)
            *color = 0x00000000; // same color for everyone, return color based on type
        if (start)
        {
            float s = (float)item.mFrameStart;
            *start = &s;
        }
        if (end)
        {
            float e = (float)item.mFrameEnd;
            *end = &e;
        }
        if (type)
            *type = item.mType;
    }
    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
    {
        MySequenceItem& item = myItems[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.mFrameStart;
        if (end)
            *end = &item.mFrameEnd;
        if (type)
            *type = item.mType;
    }
    virtual void Add(int type) { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
    virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
    int Delete(int index) 
    { 
        int typeId = myItems[index].mType;
        myItems.erase(myItems.begin() + index); 
        return typeId;
    }
    virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

    virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

    virtual int GetItemTypeCount() const { return SequencerItemTypeNames.size(); }
    virtual const char* GetItemTypeName(int typeIndex) const 
    {
        auto& it=SequencerItemTypeNames.find(typeIndex);
        
        return it->second.c_str();
    }
    virtual const char* GetItemLabel(int index) const
    {
        
        static char tmps[512];
        snprintf(tmps, 512, GetItemTypeName(myItems[index].mType));
        return tmps;
    }
    //const int GetItemTypeIndex(int typeIndex) const
    //{
    //    int index = 0;
    //    for (auto& itemName : SequencerItemTypeNames)
    //    {
    //        if (itemName.first == typeIndex)
    //        {
    //            return index;
    //        }
    //        index++;
    //    }
    //    return -1;
    //}

    void DeleteItem(int index)
    {
        SequencerItemTypeNames.erase(index);
    }

    void AllDeleteItem()
    {
        int index = myItems.size() - 1;
        for (int i = index; i >= 0; --i)
        {
            DeleteItem(myItems[i].mType);
            Delete(i);
        }
    }

    // my datas
    MySequence() : mFrameMin(0), mFrameMax(0) {}
    int mFrameMin, mFrameMax;

    virtual void DoubleClick(int index) {
        if (myItems[index].mExpanded)
        {
            myItems[index].mExpanded = false;
            return;
        }
        for (auto& item : myItems)
            item.mExpanded = false;
        myItems[index].mExpanded = !myItems[index].mExpanded;
    }

    //—v‘f‚Ì–¼‘O
    void AddTypeName(int typeID,std::string name)
    {
        SequencerItemTypeNames[typeID] = name;
    }

    std::map<int, std::string> SequencerItemTypeNames;


    struct MySequenceItem
    {
        int mType;
        int mFrameStart, mFrameEnd;
        bool mExpanded;
    };
    std::vector<MySequenceItem> myItems;

};
