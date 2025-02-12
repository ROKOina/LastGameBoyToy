// pch.h: プリコンパイル済みヘッダー ファイルです。
// 次のファイルは、その後のビルドのビルド パフォーマンスを向上させるため 1 回だけコンパイルされます。
// コード補完や多くのコード参照機能などの IntelliSense パフォーマンスにも影響します。
// ただし、ここに一覧表示されているファイルは、ビルド間でいずれかが更新されると、すべてが再コンパイルされます。
// 頻繁に更新するファイルをここに追加しないでください。追加すると、パフォーマンス上の利点がなくなります。

#ifndef PCH_H
#define PCH_H

// プリコンパイルするヘッダーをここに追加します

#if 0
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif
#endif

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#define _USE_MATH_DEFINES
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>

#include <algorithm>

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>

#include <span>
#include <ranges>

#include <exception>
#include <stdexcept>

#include <any>
#include <bit>
#include <bitset>
#include <tuple>
#include <optional>
#include <variant>
#include <utility>

#include <new>
#include <memory>

#include <functional>

#include <fstream>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>

#include <locale>

#include <complex>
#include <limits>
#include <numeric>
#include <random>
#include <ratio>
#include <valarray>

#include <atomic>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include <regex>

#include <string>
#include <string_view>
using namespace std::string_literals;

#include <chrono>

#define NOMINMAX
#include <winsock2.h>
#include <Windows.h>
#include <windows.h>
#include <tchar.h>

#include <propvarutil.h>

#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>
#include <wmcodecdsp.h>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

#include <wrl/client.h>

#include <dxgi1_6.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <xaudio2.h>
#include <xinput.h>

#include <codeanalysis\warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS) // 外部ライブラリの警告抑制

// https://github.com/microsoft/DirectXTex
//#include <DDSTextureLoader11.h>
//#include <WICTextureLoader11.h>

// https://github.com/microsoft/DirectXTK
#include <BufferHelpers.h>
#include <CommonStates.h>
#include <DDSTextureLoader.h>
#include <DirectXHelpers.h>
#include <Effects.h>
#include <GamePad.h>
#include <GeometricPrimitive.h>
#include <GraphicsMemory.h>
#include <Keyboard.h>
#include <Model.h>
#include <Mouse.h>
#include <PostProcess.h>
#include <PrimitiveBatch.h>
#include <ScreenGrab.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>

// https://github.com/microsoft/DirectXMath
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

// https://github.com/nlohmann/json
#include <nlohmann/json.hpp>

// https://github.com/USCiLab/cereal
#include <cereal/macros.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/atomic.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/functional.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>

// https://github.com/NVIDIA-Omniverse/PhysX
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include <PxPhysicsAPI.h>

// https://github.com/Neargye/magic_enum
#include <magic_enum.hpp>

//#ifdef DEBUG
// https://github.com/ocornut/imgui
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

// https://github.com/CedricGuillemet/ImGuizmo
#include <ImGuizmo.h>
#include <ImCurveEdit.h>
#include <GraphEditor.h>

#pragma warning(pop) // 外部ライブラリの警告抑制

// ゲーム関連で使うインクルード
#include "Math/Define.h"
#include "Math/Vector2.h"
#include "SystemStruct/Logger.h"
#include "Utility/Utility_.h"
#include "SystemStruct/Dialog.h"
#include "SystemStruct/TimeManager.h"
#include "Component/System/Component.h"
#include "Component/System/GameObject.h"
#include "Component/System/TransformCom.h"

#endif //PCH_H
