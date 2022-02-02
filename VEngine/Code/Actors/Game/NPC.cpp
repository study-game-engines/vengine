#include "NPC.h"
#include "VString.h"
#include "Timer.h"
#include "UI/DialogueWidget.h"
#include "Components/DialogueComponent.h"

NPC::NPC()
{
    isDestructible = true;
}

void NPC::Start()
{
    __super::Start();

    if (!spawnText.empty())
    {
        if (isQuickDialogueActive) return;

        spawnTextWidget = CreateWidget<DialogueWidget>();
        spawnTextWidget->dialogueText = spawnText;
        spawnTextWidget->AddToViewport(5.0f);
    }
}

void NPC::Tick(float deltaTime)
{
    if (spawnTextWidget)
    {
        spawnTextWidget->pos = GetHomogeneousPositionVector();
    }

    __super::Tick(deltaTime);
}

Properties NPC::GetProps()
{
    Properties props = __super::GetProps();
    props.title = "NPC";
    props.AddProp(spawnText);
    props.AddProp(battleStartText);
    return props;
}

void NPC::QuickTalkTo()
{
    spawnTextWidget->RemoveFromViewport();

    if (isQuickDialogueActive) return;

    isQuickDialogueActive = true;

    dialogueComponent->dialogueWidget->dialogueText = interactText;
    dialogueComponent->AddToViewport();

    Timer::SetTimer(5.0f, std::bind(&NPC::EndQuickTalkTo, this));
}

void NPC::BattleStartDialogue()
{
    if (battleStartText.empty())
    {
        return;
    }

    spawnTextWidget->RemoveFromViewport();

    if (isQuickDialogueActive)
    {
        return;
    }

    isQuickDialogueActive = true;

    dialogueComponent->dialogueWidget->dialogueText = battleStartText;
    dialogueComponent->AddToViewport();

    Timer::SetTimer(5.0f, std::bind(&NPC::EndQuickTalkTo, this));
}

void NPC::EndQuickTalkTo()
{
    isQuickDialogueActive = false;
    dialogueComponent->RemoveFromViewport();
}
