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
 * @file LogInitializer.h
 * @author: octopus
 * @date 2021-10-27
 */
#pragma once
#include <bcos-boostssl/utilities/BoostLogInitializer.h>
#include <exception>
#include <mutex>
#include <string>

namespace bcos
{
namespace cppsdk
{
class LogInitializer
{
public:
    static void initLog(const std::string& _configPath = "./clog.ini")
    {
        boost::property_tree::ptree pt;
        try
        {
            boost::property_tree::read_ini(_configPath, pt);
        }
        catch (const std::exception& e)
        {
            try
            {
                std::string defaultPath = "conf/clog.ini";
                boost::property_tree::read_ini(defaultPath, pt);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Not found available log config(./clog.ini or ./conf/clog.ini), use "
                             "the default configuration items"
                          << std::endl;
            }
        }

        initLog(pt);
    }

    static void initLog(const boost::property_tree::ptree& _pt)
    {
        std::call_once(m_flag, [_pt]() {
            m_logInitializer = new bcos::boostssl::utilities::BoostLogInitializer();
            m_logInitializer->initLog(_pt);
        });
    }

private:
    static std::once_flag m_flag;
    static bcos::boostssl::utilities::BoostLogInitializer* m_logInitializer;
};
}  // namespace cppsdk
}  // namespace bcos