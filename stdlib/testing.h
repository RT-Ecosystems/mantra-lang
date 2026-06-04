#pragma once
#include "../include/mantra/core/types.h"
#include <vector>

namespace mantra {
namespace stdlib {

MantraValue builtinAssert(const std::vector<MantraValue>& args);
MantraValue builtinAssertEqual(const std::vector<MantraValue>& args);
MantraValue builtinAssertNotEqual(const std::vector<MantraValue>& args);
MantraValue builtinAssertTrue(const std::vector<MantraValue>& args);
MantraValue builtinAssertFalse(const std::vector<MantraValue>& args);
MantraValue builtinAssertApprox(const std::vector<MantraValue>& args);
MantraValue builtinTest(const std::vector<MantraValue>& args);
MantraValue builtinTestResults(const std::vector<MantraValue>& args);
MantraValue builtinTestReset(const std::vector<MantraValue>& args);

} // namespace stdlib
} // namespace mantra