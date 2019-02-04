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
#include <folly/futures/Unit.h>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include "core/async-batch-rpc-retrying-caller.h"
#include "core/async-connection.h"
#include "core/async-rpc-retrying-caller-factory.h"
#include "core/async-rpc-retrying-caller.h"
#include "core/connection-configuration.h"
#include "core/get.h"
#include "core/put.h"
#include "core/result.h"

namespace hbase {

/**
 * A low level asynchronous table that should not be used by user applications.The implementation
 * is required to be thread safe.
 */
class RawAsyncTable {
 public:
  RawAsyncTable(std::shared_ptr<pb::TableName> table_name,
                std::shared_ptr<AsyncConnection> connection)
      : connection_(connection),
        connection_conf_(connection->connection_conf()),
        table_name_(table_name),
        rpc_client_(connection->rpc_client()) {}
  virtual ~RawAsyncTable() = default;

  folly::Future<std::shared_ptr<Result>> Get(const hbase::Get& get);

  folly::Future<folly::Unit> Put(const hbase::Put& put);
  void Close() {}

  folly::Future<std::vector<folly::Try<std::shared_ptr<Result>>>> Get(
      const std::vector<hbase::Get>& gets);
  folly::Future<std::vector<folly::Try<std::shared_ptr<Result>>>> Batch(
      const std::vector<hbase::Get>& gets);

 private:
  /* Data */
  std::shared_ptr<AsyncConnection> connection_;
  std::shared_ptr<ConnectionConfiguration> connection_conf_;
  std::shared_ptr<pb::TableName> table_name_;
  std::shared_ptr<RpcClient> rpc_client_;

  /* Methods */
  template <typename REQ, typename PREQ, typename PRESP, typename RESP>
  folly::Future<RESP> Call(
      std::shared_ptr<RpcClient> rpc_client, std::shared_ptr<HBaseRpcController> controller,
      std::shared_ptr<RegionLocation> loc, const REQ& req,
      const ReqConverter<std::unique_ptr<PREQ>, REQ, std::string> req_converter,
      const RespConverter<RESP, PRESP> resp_converter);

  template <typename RESP>
  std::shared_ptr<SingleRequestCallerBuilder<RESP>> CreateCallerBuilder(
      std::string row, std::chrono::nanoseconds rpc_timeout);
};
}  // namespace hbase
