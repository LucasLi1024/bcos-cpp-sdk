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
 * @file SdkFactory.cpp
 * @author: octopus
 * @date 2021-08-21
 */

#include <bcos-boostssl/utilities/Common.h>
#include <bcos-boostssl/websocket/WsConnector.h>
#include <bcos-boostssl/websocket/WsInitializer.h>
#include <bcos-boostssl/websocket/WsMessage.h>
#include <bcos-boostssl/websocket/WsService.h>
#include <bcos-cpp-sdk/LogInitializer.h>
#include <bcos-cpp-sdk/SdkFactory.h>
#include <bcos-cpp-sdk/amop/AMOP.h>
#include <bcos-cpp-sdk/amop/AMOPRequest.h>
#include <bcos-cpp-sdk/amop/Common.h>
#include <bcos-cpp-sdk/rpc/Common.h>
#include <bcos-cpp-sdk/rpc/JsonRpcImpl.h>
#include <bcos-cpp-sdk/ws/Service.h>
#include <memory>
#include <mutex>

using namespace bcos;
using namespace bcos::boostssl;
using namespace bcos::boostssl::ws;
using namespace bcos::boostssl::utilities;
using namespace bcos::cppsdk::amop;
using namespace bcos::cppsdk;
using namespace bcos::cppsdk::jsonrpc;
using namespace bcos::cppsdk::event;
using namespace bcos::cppsdk::service;

bcos::cppsdk::Sdk::UniquePtr SdkFactory::buildSdk()
{
    auto service = buildService();
    auto amop = buildAMOP(service);
    auto jsonRpc = buildJsonRpc(service);
    auto eventSub = buildEventSub(service);

    auto sdk = std::make_unique<bcos::cppsdk::Sdk>();
    sdk->setService(service);
    sdk->setAmop(amop);
    sdk->setEventSub(eventSub);
    sdk->setJsonRpc(jsonRpc);
    return sdk;
}

Service::Ptr SdkFactory::buildService()
{
    // TODO: how to init log in cpp sdk
    LogInitializer::initLog();

    auto service = std::make_shared<Service>();
    auto initializer = std::make_shared<WsInitializer>();

    auto groupInfoFactory = std::make_shared<bcos::group::GroupInfoFactory>();
    auto chainNodeInfoFactory = std::make_shared<bcos::group::ChainNodeInfoFactory>();

    initializer->setConfig(m_config);
    initializer->initWsService(service);
    service->setWaitConnectFinish(true);
    service->setGroupInfoFactory(groupInfoFactory);
    service->setChainNodeInfoFactory(chainNodeInfoFactory);

    service->registerMsgHandler(bcos::cppsdk::jsonrpc::MessageType::BLOCK_NOTIFY,
        [service](std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session) {
            auto blkMsg = std::string(_msg->data()->begin(), _msg->data()->end());

            service->onRecvBlockNotifier(blkMsg);

            BCOS_LOG(INFO) << "[WS]" << LOG_DESC("receive block notify")
                           << LOG_KV("endpoint", _session->endPoint()) << LOG_KV("blk", blkMsg);
        });

    service->registerMsgHandler(bcos::cppsdk::jsonrpc::MessageType::GROUP_NOTIFY,
        [service](std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session) {
            std::string groupInfo = std::string(_msg->data()->begin(), _msg->data()->end());

            service->onNotifyGroupInfo(groupInfo, _session);

            BCOS_LOG(INFO) << "[WS]" << LOG_DESC("receive group info notify")
                           << LOG_KV("endpoint", _session->endPoint())
                           << LOG_KV("groupInfo", groupInfo);
        });

    return service;
}

bcos::cppsdk::jsonrpc::JsonRpcImpl::Ptr SdkFactory::buildJsonRpc(Service::Ptr _service)
{
    auto jsonRpc = std::make_shared<JsonRpcImpl>();
    auto factory = std::make_shared<JsonRpcRequestFactory>();
    jsonRpc->setFactory(factory);
    jsonRpc->setService(_service);

    jsonRpc->setSender([_service](const std::string& _group, const std::string& _node,
                           const std::string& _request, bcos::cppsdk::jsonrpc::RespFunc _respFunc) {
        auto data = std::make_shared<bytes>(_request.begin(), _request.end());
        auto msg = _service->messageFactory()->buildMessage();
        msg->setType(bcos::cppsdk::jsonrpc::MessageType::RPC_REQUEST);
        msg->setData(data);

        _service->asyncSendMessageByGroupAndNode(_group, _node, msg, Options(),
            [_respFunc](Error::Ptr _error, std::shared_ptr<WsMessage> _msg,
                std::shared_ptr<WsSession> _session) {
                (void)_session;
                _respFunc(_error, _msg ? _msg->data() : nullptr);
            });
    });
    return jsonRpc;
}

bcos::cppsdk::amop::AMOP::Ptr SdkFactory::buildAMOP(bcos::cppsdk::service::Service::Ptr _service)
{
    auto amop = std::make_shared<bcos::cppsdk::amop::AMOP>();

    auto topicManager = std::make_shared<TopicManager>();
    auto requestFactory = std::make_shared<bcos::protocol::AMOPRequestFactory>();
    auto messageFactory = std::make_shared<WsMessageFactory>();

    amop->setTopicManager(topicManager);
    amop->setRequestFactory(requestFactory);
    amop->setMessageFactory(messageFactory);
    amop->setService(_service);

    auto amopWeakPtr = std::weak_ptr<bcos::cppsdk::amop::AMOP>(amop);

    _service->registerMsgHandler(bcos::cppsdk::amop::MessageType::AMOP_REQUEST,
        [amopWeakPtr](std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session) {
            auto amop = amopWeakPtr.lock();
            if (amop)
            {
                amop->onRecvAMOPRequest(_msg, _session);
            }
        });
    _service->registerMsgHandler(bcos::cppsdk::amop::MessageType::AMOP_RESPONSE,
        [amopWeakPtr](std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session) {
            auto amop = amopWeakPtr.lock();
            if (amop)
            {
                amop->onRecvAMOPResponse(_msg, _session);
            }
        });
    _service->registerMsgHandler(bcos::cppsdk::amop::MessageType::AMOP_BROADCAST,
        [amopWeakPtr](std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session) {
            auto amop = amopWeakPtr.lock();
            if (amop)
            {
                amop->onRecvAMOPBroadcast(_msg, _session);
            }
        });
    _service->registerWsHandshakeSucHandler([amopWeakPtr](std::shared_ptr<WsSession> _session) {
        auto amop = amopWeakPtr.lock();
        if (amop)
        {
            // service handshake successfully
            amop->updateTopicsToRemote(_session);
        }
    });
    return amop;
}

bcos::cppsdk::event::EventSub::Ptr SdkFactory::buildEventSub(Service::Ptr _service)
{
    auto eventSub = std::make_shared<event::EventSub>();
    auto messageFactory = std::make_shared<WsMessageFactory>();

    eventSub->setMessageFactory(messageFactory);
    eventSub->setService(_service);
    eventSub->setConfig(_service->config());
    eventSub->setIoc(_service->ioc());

    auto eventWeakPtr = std::weak_ptr<bcos::cppsdk::event::EventSub>(eventSub);
    _service->registerMsgHandler(bcos::cppsdk::event::MessageType::EVENT_LOG_PUSH,
        [eventWeakPtr](std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session) {
            auto eventSub = eventWeakPtr.lock();
            if (eventSub)
            {
                eventSub->onRecvEventSubMessage(_msg, _session);
            }
        });

    _service->registerDisconnectHandler([eventWeakPtr](std::shared_ptr<WsSession> _session) {
        auto eventSub = eventWeakPtr.lock();
        if (eventSub)
        {
            eventSub->suspendTasks(_session);
        }
    });

    return eventSub;
}