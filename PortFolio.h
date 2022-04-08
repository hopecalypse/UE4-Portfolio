// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOG_SPOOK 카테고리 / LOG_AUTO 매크로

// 카테고리 등록
DECLARE_LOG_CATEGORY_EXTERN(LogCustom, Log, All);

// 함수, 라인 정보
#define CALLINFOTEXT (FString(__FUNCTION__) + TEXT("(line:") + FString::FromInt(__LINE__) + TEXT(")"))

// 자동 로그
#define LOGAUTO_WARN UE_LOG(LogCustom, Warning, TEXT("%s"), *CALLINFOTEXT)
#define LOGAUTO_ERROR UE_LOG(LogCustom, Error, TEXT("%s"), *CALLINFOTEXT)
#define LOGAUTO_LOG UE_LOG(LogCustom, Log, TEXT("%s"), *CALLINFOTEXT)

// 텍스트 입력 로그
#define LOGTEXT_WARN(Format, ...) UE_LOG(LogCustom, Warning, TEXT("%s from: %s"), *FString::Printf(Format, ##__VA_ARGS__), *CALLINFOTEXT)
#define LOGTEXT_LOG(Format, ...) UE_LOG(LogCustom, Log, TEXT("%s from: %s"), *FString::Printf(Format, ##__VA_ARGS__), *CALLINFOTEXT)
#define LOGTEXT_ERROR(Format, ...) UE_LOG(LogCustom, Error, TEXT("%s from: %s"), *FString::Printf(Format, ##__VA_ARGS__), *CALLINFOTEXT)

// 에디터 위에 로그
#define	LOG_SCREEN(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(Format, ##__VA_ARGS__))
#define LOGSCREEN_AUTO GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AUTOLOG from: %s"), *CALLINFOTEXT))