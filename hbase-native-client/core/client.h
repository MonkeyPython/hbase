/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include <folly/futures/Future.h>
#include <folly/io/IOBuf.h>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>
#include <wangle/concurrent/IOThreadPoolExecutor.h>

#include <memory>
#include <string>

#include "connection/rpc-client.h"
#include "core/configuration.h"
#include "core/hbase_configuration_loader.h"
#include "core/location-cache.h"
#include "core/table.h"
#include "if/Cell.pb.h"
#include "serde/table-name.h"

using hbase::pb::TableName;

namespace hbase {
class Table;
/**
 * Client.
 *
 * This is the class that provides access to an HBase cluster.
 * It is thread safe and does connection pooling. Current recommendations are to
 * have only one Client per cluster around.
 */
class Client {
 public:
  /**
   * @brief Create a new client.
   * @param quorum_spec Where to connect to get Zookeeper bootstrap information.
   */
  Client();
  explicit Client(const hbase::Configuration &conf);
  ~Client();
  /**
   * @brief Retrieve a Table implementation for accessing a table.
   * @param - table_name
   */
  std::unique_ptr<hbase::Table> Table(const TableName &table_name);

  /**
   * @brief Close the Client connection.
   */
  void Close();

 private:
  void init(const hbase::Configuration &conf);
  const std::string kHBaseZookeeperQuorum_ = "hbase.zookeeper.quorum";
  const std::string kDefHBaseZookeeperQuorum_ = "localhost:2181";
  std::shared_ptr<wangle::CPUThreadPoolExecutor> cpu_executor_;
  std::shared_ptr<wangle::IOThreadPoolExecutor> io_executor_;
  std::shared_ptr<hbase::LocationCache> location_cache_;
  std::shared_ptr<hbase::RpcClient> rpc_client_;
  std::shared_ptr<hbase::Configuration> conf_;
  bool is_closed_ = false;
};

}  // namespace hbase
