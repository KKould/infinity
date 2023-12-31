// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <boost/bind.hpp>
#include <thread>
#include "thrift_server.h"

module db_server;
import infinity_context;
import stl;
import boost;
import third_party;
import infinity_exception;

import connection;

namespace infinity {

void DBServer::Run() {
    if (initialized) {
        return;
    }

    initialized = true;

    u16 pg_port = InfinityContext::instance().config()->pg_port();
    const String &pg_listen_addr = InfinityContext::instance().config()->listen_address();

    BoostErrorCode error;
    AsioIpAddr address = asio_make_address(pg_listen_addr, error);
    if (error) {
        Printf("{} isn't a valid IPv4 address.\n", pg_listen_addr);
        infinity::InfinityContext::instance().UnInit();
        return ;
    }

    acceptor_ptr_ = MakeUnique<AsioAcceptor>(io_service_, AsioEndPoint(address, pg_port));
    CreateConnection();

    Printf("Run 'psql -h {} -p {}' to connect to the server.\n", pg_listen_addr, pg_port);

    io_service_.run();
}

void DBServer::Shutdown() {
    Printf("Shutdown infinity server ...\n");
    while (running_connection_count_ > 0) {
        // Running connection exists.
        std::this_thread::yield();
    }

    io_service_.stop();
    initialized = false;
    acceptor_ptr_->close();

    infinity::InfinityContext::instance().UnInit();
    Printf("Shutdown infinity server successfully\n");
}

void DBServer::CreateConnection() {
    SharedPtr<Connection> connection_ptr = MakeShared<Connection>(io_service_);
    acceptor_ptr_->async_accept(*(connection_ptr->socket()), boost::bind(&DBServer::StartConnection, this, connection_ptr));
}

void DBServer::StartConnection(SharedPtr<Connection> &connection) {
    Thread connection_thread([connection = connection, &num_running_connections = this->running_connection_count_]() mutable {
        ++num_running_connections;
        connection->Run();

        // User disconnected
        connection.reset();
        --num_running_connections;
    });

    connection_thread.detach();
    CreateConnection();
}

} // namespace infinity
