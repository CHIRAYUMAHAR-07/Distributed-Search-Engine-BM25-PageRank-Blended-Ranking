#include <opentelemetry/trace/provider.h>

auto tracer = opentelemetry::trace::Provider::GetTracerProvider()
                  ->GetTracer("search-engine");

auto span = tracer->StartSpan("IndexDocument");
