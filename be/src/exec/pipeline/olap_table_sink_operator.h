// This file is licensed under the Elastic License 2.0. Copyright 2021-present, StarRocks Inc.

#pragma once

#include <utility>

#include "exec/pipeline/fragment_context.h"
#include "exec/pipeline/operator.h"
#include "gen_cpp/InternalService_types.h"

namespace starrocks {

namespace stream_load {
class OlapTableSink;
}

namespace pipeline {
class OlapTableSinkOperator final : public Operator {
public:
    OlapTableSinkOperator(OperatorFactory* factory, int32_t id, int32_t plan_node_id, int32_t driver_sequence,
                          starrocks::stream_load::OlapTableSink* sink, FragmentContext* const fragment_ctx)
            : Operator(factory, id, "olap_table_sink", plan_node_id, driver_sequence),
              _sink(sink),
              _fragment_ctx(fragment_ctx) {}

    ~OlapTableSinkOperator() override = default;

    Status prepare(RuntimeState* state) override;

    void close(RuntimeState* state) override;

    bool has_output() const override { return false; }

    bool need_input() const override;

    bool is_finished() const override;

    bool pending_finish() const override;

    Status set_finishing(RuntimeState* state) override;

    Status set_cancelled(RuntimeState* state) override;

    StatusOr<vectorized::ChunkPtr> pull_chunk(RuntimeState* state) override;

    Status push_chunk(RuntimeState* state, const vectorized::ChunkPtr& chunk) override;

private:
    starrocks::stream_load::OlapTableSink* _sink;
    FragmentContext* const _fragment_ctx;

    bool _is_finished = false;
    mutable bool _is_open_done = false;
};

class OlapTableSinkOperatorFactory final : public OperatorFactory {
public:
    OlapTableSinkOperatorFactory(int32_t id, std::unique_ptr<starrocks::DataSink>& sink,
                                 FragmentContext* const fragment_ctx)
            : OperatorFactory(id, "olap_table_sink", Operator::s_pseudo_plan_node_id_for_olap_table_sink),
              _data_sink(std::move(sink)),
              _sink(down_cast<starrocks::stream_load::OlapTableSink*>(_data_sink.get())),
              _fragment_ctx(fragment_ctx) {}

    ~OlapTableSinkOperatorFactory() override = default;

    OperatorPtr create(int32_t degree_of_parallelism, int32_t driver_sequence) override;

    Status prepare(RuntimeState* state) override;

    void close(RuntimeState* state) override;

private:
    std::unique_ptr<starrocks::DataSink> _data_sink;
    starrocks::stream_load::OlapTableSink* _sink;
    FragmentContext* const _fragment_ctx;
};

} // namespace pipeline
} // namespace starrocks
