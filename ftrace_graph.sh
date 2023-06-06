#!/bin/bash
tracefs=/sys/kernel/debug/tracing
echo nop > $tracefs/current_tracer
echo > $tracefs/trace
echo 0 > $tracefs/tracing_on
#echo > $tracefs/set_ftrace_filter

echo 0 > $tracefs/max_graph_depth

echo $$ > $tracefs/set_ftrace_pid
#echo ':mod:mxgpu' > $trace/set_ftrace_filter
#echo 'drm*' >> $trace/set_ftrace_filter
echo 1 > $tracefs/events/kmem/mm_page_alloc/enable
#echo 'ttm*' >> $trace/set_ftrace_filter
#
#echo function_graph > $tracefs/current_tracer
#echo 'mxgpu_mxcd_xvm_alloc_memory_of_gpu' > $tracefs/set_graph_function
#echo 'mxgpu_mxcd_xvm_map_memory_to_gpu' > $tracefs/set_graph_function
echo 1 > $tracefs/options/funcgraph-tail

echo 1 > $tracefs/tracing_on
exec "$@"
