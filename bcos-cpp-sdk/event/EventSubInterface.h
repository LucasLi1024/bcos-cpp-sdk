/*
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @file EvenPushInterface.h
 * @author: octopus
 * @date 2021-09-01
 */

#pragma once
#include <bcos-boostssl/utilities/Common.h>
#include <bcos-boostssl/utilities/Error.h>
#include <bcos-cpp-sdk/event/EventSubParams.h>

namespace bcos
{
namespace cppsdk
{
namespace event
{
using Callback = std::function<void(bcos::boostssl::utilities::Error::Ptr, const std::string&)>;

class EventSubInterface
{
public:
    using Ptr = std::shared_ptr<EventSubInterface>;
    using UniquePtr = std::unique_ptr<EventSubInterface>;

    virtual ~EventSubInterface() {}

public:
    virtual void start() = 0;
    virtual void stop() = 0;

public:
    virtual std::string subscribeEvent(
        const std::string& _group, const std::string& _params, Callback _callback) = 0;
    virtual std::string subscribeEvent(
        const std::string& _group, EventSubParams::Ptr _params, Callback _callback) = 0;
    virtual void unsubscribeEvent(const std::string& _id) = 0;
};
}  // namespace event
}  // namespace cppsdk
}  // namespace bcos