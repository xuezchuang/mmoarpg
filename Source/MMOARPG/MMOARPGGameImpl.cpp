// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMOARPGGameImpl.h"
#include "Modules/ModuleManager.h"

FClientDataStore MMOARPGGameImpl::ClientDataStore;
 
MMOARPGGameImpl::MMOARPGGameImpl() : FDefaultGameModuleImpl()
{
}

void MMOARPGGameImpl::StartupModule()
{

}

void MMOARPGGameImpl::ShutdownModule()
{

}

FClientDataStore& MMOARPGGameImpl::GetClientDataStore()
{
	return ClientDataStore;
}