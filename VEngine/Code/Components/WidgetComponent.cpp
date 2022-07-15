#include "vpch.h"
#include "WidgetComponent.h"
#include "UI/Widget.h"

void WidgetComponent::Tick(float deltaTime)
{
}

void WidgetComponent::Start()
{
}

void WidgetComponent::Create()
{
}

Properties WidgetComponent::GetProps()
{
    return __super::GetProps();
}

void WidgetComponent::AddToViewport()
{
    widget->AddToViewport();
}

void WidgetComponent::RemoveFromViewport()
{
    widget->RemoveFromViewport();
}

void WidgetComponent::SetPosition(XMVECTOR newPosition)
{
    if (widget)
    {
        widget->pos = newPosition;
    }
}
