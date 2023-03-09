#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <thread>
#include <regex>

#include <string>
#include <array>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <concepts>
#include <type_traits>

#include "Runtime/Core/Log.h"

#include "Runtime/Debug/Instrumentor.h"

#ifdef FLAME_PLATFORM_WINDOWS
	#include <Windows.h>
#endif


//ʹ��pchʱ��include�Ĳ���һ��Ҫ�ڵ�һ��
//���ң������ѡ��ʹ��pch�ļ�
//�������е�cpp�ļ�������include���ļ�