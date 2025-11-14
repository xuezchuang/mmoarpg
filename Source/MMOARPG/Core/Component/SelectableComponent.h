#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SelectableComponent.generated.h"

// SelectableComponent.h 顶部
UENUM(BlueprintType)
enum class ESelectableType : uint8
{
    Player  UMETA(DisplayName="Player"),
    Monster UMETA(DisplayName="Monster"),
    NPC     UMETA(DisplayName="NPC"),
    Ore     UMETA(DisplayName="Ore"),
    Other   UMETA(DisplayName="Other")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectableSelected);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectableSelectionEnd);



//class ARBaseCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MMOARPG_API USelectableComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USelectableComponent();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(BlueprintAssignable)
    FOnSelectableSelected OnSelectedEvent;

    UPROPERTY(BlueprintAssignable)
    FOnSelectableSelectionEnd OnSelectionEndEvent;

    /** 这个对象是什么类型：怪物 / NPC / 矿石 / 玩家 / 其他 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Selectable")
    ESelectableType SelectableType = ESelectableType::Other;

    /** 当前是否处于“被选中”状态 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Selectable")
    bool bIsSelected = false;

    /** 被选中时调用（PC 那边点中了） */
    UFUNCTION(BlueprintCallable, Category="Selectable")
    void OnSelected();

    /** 选中结束时调用（被点掉 / 点到别处） */
    UFUNCTION(BlueprintCallable, Category="Selectable")
    void OnSelectionEnd();

    /** 简单高亮示例，你可以换成自己的逻辑（材质、Outline、UI 等） */
    void ApplyHighlight(bool bEnable);
};
