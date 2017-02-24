//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Common (finjin-common).
//
//Finjin Common is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


#pragma once 


//Includes---------------------------------------------------------------------
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/StaticVector.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Common {

    class FINJIN_COMMON_LIBRARY_API OperationStatus
    {
    public:
        enum Status
        {
            NONE, //Initial status
            
            STARTED, //Operation has started and is in progress
            SUCCESS, //Operation was successfully finished
           
            FAILURE //Something went critically wrong while the operation was in progress
        };

        OperationStatus() { this->status = NONE; this->progress = -1; }

        void Reset(Status status = NONE) { this->error.Reset(); this->status = status; this->progress = -1; }

        bool IsStarted() const { return this->status == STARTED; }
        bool IsSuccess() const { return this->status == SUCCESS; }
        bool IsStartedOrSuccess() const { return this->status == STARTED || this->status == SUCCESS; }
        bool IsFailure() const { return this->status == FAILURE; }

        Status GetStatus() const { return this->status; }
        void SetStatus(Status value) { this->status = value; }

        float GetProgress() const { return this->progress; }
        void UpdateProgress(float value) { this->progress = std::max(this->progress, value); }

        ErrorState& GetError() { return this->error; }
        const ErrorState& GetError() const { return this->error; }

    private:
        ErrorState error;
        Status status;
        float progress; //Range [0-1] = known progress. < 0 = unknown progress
    };
    
    template <size_t MaxItems>
    class OperationStatuses : public StaticVector<const OperationStatus*, MaxItems>
    {
    public:
        using Super = StaticVector<const OperationStatus*, MaxItems>;

        OperationStatuses() { this->status = OperationStatus::NONE; }

        void Remove(const OperationStatus* status)
        {
            auto foundAt = Super::find(status);
            if (foundAt != Super::end())
                Super::erase(foundAt);
        }

        OperationStatus::Status GetStatus() const
        { 
            if (this->status == OperationStatus::NONE)
                this->status = _GetStatus();

            return this->status; 
        }
        
    private:
        OperationStatus::Status _GetStatus() const
        {
            if (this->count > 0)
            {
                auto allSuccess = true;
                auto anyStarted = false;

                for (auto item : this->items)
                {
                    switch (item->GetStatus())
                    {
                        case OperationStatus::STARTED: 
                        {
                            allSuccess = false;
                            anyStarted = true;
                            break;
                        }
                        case OperationStatus::FAILURE:
                        {
                            //If any status is a failure, then the total status is a failure
                            return OperationStatus::FAILURE;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }

                if (allSuccess)
                    return OperationStatus::SUCCESS;
                if (anyStarted)
                    return OperationStatus::STARTED;
            }

            return OperationStatus::NONE;
        }

    private:
        mutable OperationStatus::Status status;
    };

} }
