/* Copyright (C) Siqi.Wu - All Rights Reserved
* Written by Siqi.Wu<lion547016@gmail.com>, September 2019
*/

#pragma once

#include "CoreMinimal.h"

#if WITH_GAMELIFTCLIENTSDK && WITH_CORE

#include "aws/gamelift/model/DescribeFleetUtilizationRequest.h"

#endif


#include "DescribeFleetUtilizationRequest.generated.h"

USTRUCT(BlueprintType)
struct FDescribeFleetUtilizationRequest {
GENERATED_BODY()

    /**
    *  <p>Unique identifier for a fleet(s) to retrieve utilization data for. To request utilization data for all fleets, leave this parameter empty.</p>
    **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameLift Client")
    TArray<FString> fleetIds;

    /**
    *  <p>Maximum number of results to return. Use this parameter with <code>NextToken</code> to get results as a set of sequential pages. This parameter is ignored when the request specifies one or a list of fleet IDs.</p>
    **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameLift Client")
    int limit = 0;

    /**
    *  <p>Token that indicates the start of the next sequential page of results. Use the token that is returned with a previous call to this action. To start at the beginning of the result set, do not specify a value. This parameter is ignored when the request specifies one or a list of fleet IDs.</p>
    **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameLift Client")
    FString nextToken;

#if WITH_GAMELIFTCLIENTSDK && WITH_CORE
public:
    Aws::GameLift::Model::DescribeFleetUtilizationRequest toAWS() const {
        Aws::GameLift::Model::DescribeFleetUtilizationRequest awsDescribeFleetUtilizationRequest;

        for (const FString& elem : this->fleetIds) {
            awsDescribeFleetUtilizationRequest.AddFleetIds(TCHAR_TO_UTF8(*elem));
        }

        if (this->limit != 0) {
            awsDescribeFleetUtilizationRequest.SetLimit(this->limit);
        }

        if (!(this->nextToken.IsEmpty())) {
            awsDescribeFleetUtilizationRequest.SetNextToken(TCHAR_TO_UTF8(*this->nextToken));
        }

        return awsDescribeFleetUtilizationRequest;
    }

    bool IsEmpty() const {
        return this->fleetIds.Num() == 0 && this->limit == 0 && this->nextToken.IsEmpty();
    }
#endif
};
