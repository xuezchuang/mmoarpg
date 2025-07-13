// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "FItemData.h"

class FClientDataStore
{
public:
	FClientDataStore();

	//Use only reference variable.
	FClientDataStore(FClientDataStore const&) = delete;
	FClientDataStore& operator=(FClientDataStore const&) = delete;

public:
	void InitializeAccountInfo();
};