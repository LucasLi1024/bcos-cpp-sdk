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
 * @file Common.h
 * @author: octopus
 * @date 2021-08-10
 */

#pragma once
#include <bcos-boostssl/utilities/Common.h>

#define RPC_WS_LOG(LEVEL) BCOS_LOG(LEVEL) << "[RPCWS][SERVICE]"
#define RPC_BLOCKNUM_LOG(LEVEL) BCOS_LOG(LEVEL) << "[RPC][BLOCK][NUMBER]"

namespace bcos
{
namespace cppsdk
{
namespace ws
{
/**
 * @brief: jsonrpc message types
 */
enum MessageType
{
    // ------------ ws begin ----------

    HANDESHAKE = 0x100  // 256

    // ------------ ws end ------------
};
}  // namespace ws
}  // namespace cppsdk
}  // namespace bcos