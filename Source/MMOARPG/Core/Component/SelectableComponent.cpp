#include "SelectableComponent.h"
#include "GameFramework/Actor.h"
#include "Components/MeshComponent.h"

USelectableComponent::USelectableComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USelectableComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USelectableComponent::OnSelected()
{
    bIsSelected = true;
    ApplyHighlight(true);

	OnSelectedEvent.Broadcast();
}

void USelectableComponent::OnSelectionEnd()
{
    bIsSelected = false;
    ApplyHighlight(false);

	OnSelectionEndEvent.Broadcast();
}

void USelectableComponent::ApplyHighlight(bool bEnable)
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // 示例：给所有 Mesh 开启/关闭 CustomDepth 用于轮廓高亮
    TArray<UMeshComponent*> Meshes;
	Owner->GetComponents(Meshes);
	
    for (UMeshComponent* Mesh : Meshes)
    {
		Mesh->SetRenderCustomDepth(bEnable);
		Mesh->SetCustomDepthStencilValue(1); // 你项目里设置的值
    }
}
