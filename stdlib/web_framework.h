#pragma once
#include "../include/mantra/core/types.h"
#include <string>
#include <functional>

namespace mantra {
namespace stdlib {

using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;
using Middleware = std::function<void(HttpRequest&, HttpResponse&)>;

class WebApp;
WebApp* getWebApp();

MantraValue builtinWebGet(const std::vector<MantraValue>& args);
MantraValue builtinWebPost(const std::vector<MantraValue>& args);
MantraValue builtinWebStatic(const std::vector<MantraValue>& args);
MantraValue builtinWebStart(const std::vector<MantraValue>& args);
MantraValue builtinWebStop(const std::vector<MantraValue>& args);
MantraValue builtinJsonResponse(const std::vector<MantraValue>& args);
MantraValue builtinHtmlResponse(const std::vector<MantraValue>& args);

} // namespace stdlib
} // namespace mantra