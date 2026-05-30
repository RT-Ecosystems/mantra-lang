#pragma once
#include "../core/interpreter.h"
namespace mantra::stdlib {
MantraValue builtinAudioLoad(const std::vector<MantraValue>&);
MantraValue builtinAudioPlay(const std::vector<MantraValue>&);
MantraValue builtinAudioInfo(const std::vector<MantraValue>&);
}
