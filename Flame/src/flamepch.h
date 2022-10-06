#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <array>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>


#include "Flame/Log.h"
#ifdef FLAME_PLATFORM_WINDOWS
	#include <Windows.h>
#endif


//使用pch时，include的操作一定要在第一行
//而且，如果勾选了使用pch文件
//好像所有的cpp文件都必须include该文件