#pragma once

#include "../core/interpreter.h"

namespace mantra::stdlib {

MantraValue builtinAdd(const std::vector<MantraValue>& args);
MantraValue builtinSubtract(const std::vector<MantraValue>& args);
MantraValue builtinMultiply(const std::vector<MantraValue>& args);
MantraValue builtinDivide(const std::vector<MantraValue>& args);
MantraValue builtinRemainder(const std::vector<MantraValue>& args);
MantraValue builtinMean(const std::vector<MantraValue>& args);
MantraValue builtinMedian(const std::vector<MantraValue>& args);
MantraValue builtinMode(const std::vector<MantraValue>& args);
MantraValue builtinRange(const std::vector<MantraValue>& args);
MantraValue builtinStdDeviation(const std::vector<MantraValue>& args);
MantraValue builtinSum(const std::vector<MantraValue>& args);
MantraValue builtinMaximum(const std::vector<MantraValue>& args);
MantraValue builtinMinimum(const std::vector<MantraValue>& args);

MantraValue builtinSin(const std::vector<MantraValue>& args);
MantraValue builtinCos(const std::vector<MantraValue>& args);
MantraValue builtinTan(const std::vector<MantraValue>& args);
MantraValue builtinAsin(const std::vector<MantraValue>& args);
MantraValue builtinAcos(const std::vector<MantraValue>& args);
MantraValue builtinAtan(const std::vector<MantraValue>& args);
MantraValue builtinLog(const std::vector<MantraValue>& args);
MantraValue builtinLog10(const std::vector<MantraValue>& args);
MantraValue builtinExp(const std::vector<MantraValue>& args);
MantraValue builtinPow(const std::vector<MantraValue>& args);
MantraValue builtinAbs(const std::vector<MantraValue>& args);
MantraValue builtinSqrt(const std::vector<MantraValue>& args);
MantraValue builtinRound(const std::vector<MantraValue>& args);
MantraValue builtinCeil(const std::vector<MantraValue>& args);
MantraValue builtinFloor(const std::vector<MantraValue>& args);
MantraValue builtinRandom(const std::vector<MantraValue>& args);
MantraValue builtinPi(const std::vector<MantraValue>& args);
MantraValue builtinE(const std::vector<MantraValue>& args);
MantraValue builtinInfinity(const std::vector<MantraValue>& args);
MantraValue builtinNaN(const std::vector<MantraValue>& args);
MantraValue builtinIsInfinity(const std::vector<MantraValue>& args);
MantraValue builtinIsNaN(const std::vector<MantraValue>& args);

} // namespace mantra::stdlib
