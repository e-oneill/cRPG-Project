// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionSystemTags.h"

//NOT WORKING, DOESNT SEEM TO BE POSSIBLE
FGameplayTagContainer FActionSystemTags::GetActionBlockers()
{
	FGameplayTagContainer ActionBlockers;
	
	ActionBlockers.AddTag(ActionSystemTags.Status_Stunned);


	return ActionBlockers;
}

FActionSystemTags FActionSystemTags::ActionSystemTags;

