Introduction of DebugUtils HAL

1. Perfetto APIs

1.1 startPerfettoTracing(uint64_t duration, const hidl_string& debugTag)

Invoke the perfetto command with user written config and output location
if duration > 0 , add the detach option to perfetto and invoke the perfetto. This will helpful for when client want to explicity stop the perfetto.
if duration == 0, Invoke the perfetto and will exit once duration in the config file reaches.
Ex:
     sp<IDebugUtils> HalObject = IDebugUtils::getService();
     HalObject->startPerfettoTracing(0, "DebugUtils");
Note: We are disabling the tracing(/sys/kernel/debug/tracing/tracing_on) to make perfetto works smoother due to tracing enabled by default on builds.

1.2 stopPerfettoTracing(const hidl_string& debugTag)

Will check whether any Perfetto session with duration is present or not.
if present, Invoke perfetto command with --stop flag. Trace will be stored in /data/misc/perfetto-traces/ based on client invokes it.
For opening the trace, go to https://ui.perfetto.dev/#!/

Ex:
     sp<IDebugUtils> HalObject = IDebugUtils::getService();
     HalObject->startPerfettoTracing(1, "DebugUtils");
     <Perform use-case>
     HalObject->stopPerfettoTracing("DebugUtils");

For command success/failure, Please check here /data/local/tmp/perfettoDir

#References
Docs - https://perfetto.dev/ and code lies on external/perfetto