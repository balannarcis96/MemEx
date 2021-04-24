#pragma once

#include "Delegate.h"

namespace MemEx {
	template<typename ReturnType, typename ...Params>
	using Delegate = SA::delegate<ReturnType(Params...)>;
}

#include "Core.h"
#include "Tunning.h"
#include "Memory.h"
#include "Ptr.h"
#include "TObjectPool.h"
#include "MemoryManager.h"