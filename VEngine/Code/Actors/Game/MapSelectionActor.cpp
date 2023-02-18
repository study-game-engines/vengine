#include "vpch.h"
#include "MapSelectionActor.h"
#include "Components/BoxTriggerComponent.h"
#include "Actors/Game/MapScreenSelector.h"
#include "Gameplay/GameInstance.h"

MapSelectionActor::MapSelectionActor()
{
    boxTrigger = CreateComponent<BoxTriggerComponent>("BoxTrigger");
    rootComponent = boxTrigger;
}

void MapSelectionActor::Start()
{
    //Example code: how to activate this actor using global prop.
    /*auto value = GameInstance::GetGlobalProp<bool>("RamielDefeated");
    if (!value)
    {
        SetActive(false);
    }*/
}

void MapSelectionActor::Tick(float deltaTime)
{
    auto mapScreenSelector = MapScreenSelector::system.GetFirstActor();
    if (mapScreenSelector == nullptr) return;

    if (boxTrigger->Contains(mapScreenSelector->GetPositionV()))
    {
        mapScreenSelector->SetLeveInfoWidgets(title, description, levelToLoad);
    }
    else
    {
        mapScreenSelector->RemoveLeveInfoWidgets();
    }
}

Properties MapSelectionActor::GetProps()
{
    auto props = __super::GetProps();
    props.title = GetTypeName();
    props.Add("Title", &title);
    props.Add("Desc.", &description);
    props.Add("Level", &levelToLoad);
    return props;
}