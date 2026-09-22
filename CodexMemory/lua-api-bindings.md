# Lua API 绑定维护

## 项目背景

LuaSTG-x-TUO 是 Fork 自 LuaSTG-x 的游戏引擎。

## 主要维护位置与职责

目前主要维护位置为 `frameworks/LuaBindings/`，负责为应用侧提供 Lua API 绑定。

## 代码导航与注册链

下列路径均相对于仓库根目录。

| 位置 | 作用 |
| --- | --- |
| `frameworks/LuaBindings/lua_<模块>_auto.cpp` / `.hpp` | 包装 C++ 调用、注册 Lua 名称；头文件汇总模块注册入口 |
| `frameworks/LuaBindings/LuaBindings.cpp` | 包含绑定头文件，以 `LUA_REGISTER_MODULE` 将模块加入注册表；也有手写补充绑定 |
| `frameworks/LuaBindings/LuaBindings.h` | 引入转换设施，并声明项目专用转换，如 `RenderMode` |
| `frameworks/LuaBindings/lua_conversion/lua_conversion.hpp` | 调用与注册宏、Cocos 对象和结构体转换 |
| `frameworks/LuaBindings/lua_conversion/lua_conversion_tolua.hpp` | 基础类型、STL 容器、函数调用和回调转换 |
| `frameworks/Classes/XLuaModuleRegistry.h` / `.cpp` | 保存注册函数，启动时执行并检查 Lua 栈高度 |
| `frameworks/LSTG/AppFrame.cpp` | 初始化 Lua 后调用 `registerModules`、`registerFunctions` |
| `CMakeSources.cmake` | 显式列出绑定及实现的头文件、源文件与包含目录 |

典型注册链：`LuaBindings.cpp` 中的 `LUA_REGISTER_MODULE(x_SteamHelper, luaReg_SteamHelper)` → 头文件中的 `luaReg_SteamHelper` → `.cpp` 中的 `luaReg_SteamHelper_lstgSteamHelper` → Lua 的 `lstg.SteamHelper`。

注册类时通常依次使用 `LUA_ENTRY("lstg")`、`LUA_CLS_DEF` 或 `LUA_CLS_DEF_BASE`、`LUA_METHOD`、`LUA_CLS_END()`、`LUA_ENTRY_END(1)`。`LUA_CLS_END` 同时登记 C++ 类型与 Lua 类型名的映射。注册函数必须保持进出栈平衡；返回 `0` 本身不会清理已压入的模块表。

模块按 priority 排序，同优先级来自 `unordered_map`，不能依赖 `LuaBindings.cpp` 的书写顺序。手写扩展若依赖其他模块，应检查注册时序。

## 包装函数与类型转换约定

- 普通成员函数使用 `LUA_INVOKE_HEADER` / `LUA_INVOKE_FOOTER`；静态函数常使用 `LUA_SINVOKE_HEADER` / `LUA_SINVOKE_FOOTER`。两者都以 `lua_gettop(lua_S) - 1` 计算参数数目，预留接收者，因此静态接口也通常写成 `lstg.Class:method(...)`。全局函数的 `LUA_GINVOKE_HEADER` 才直接使用栈参数总数。
- 宏不会把 C++ 成员函数变成静态函数，也不会自动取得单例。`invoke_native` 按实际函数指针签名转换参数；非静态成员函数还需要正确类型的对象指针。
- `LUA_TRY_INVOKE_R(n, fn)` 用于返回一个转换后的 Lua 值；`LUA_TRY_INVOKE(n, fn)` 保留并返回第一个栈值，常用于无返回值的成员函数，实现返回接收者的链式调用。不要把后者记录为“Lua 无返回值”。
- `LUA_TRY_CTOR` 会登记 Lua GC；普通单例 `getInstance` 不应照搬构造函数的 GC/删除逻辑。`lua_StopWatch_auto.cpp` 可作为普通构造、析构和成员方法的对照。
- 重载或默认参数需要显式列出可接受的参数数目，必要时用 lambda 适配；参考 `lua_Random_auto.cpp`。footer 的期望参数数目和 header 的报错函数名必须与实际接口一致。
- 优先复用 `lua::to_native<T>`、`lua::to_lua<T>` 和 `invoke_native`。容器及 pair 转换由模板递归完成；特殊类型先核对已有特化及其 Lua 表达方式。
- `cocos2d::Ref` 派生对象和普通指针有不同的转换与生命周期处理。普通指针输出依赖已注册的类型映射；未注册时可能得到 `nil`。Lua 声明的继承关系必须核对真实 C++ 类型，不能仅为复用模板而声明 `cc.Ref`。
- `_auto` 是现有文件命名，不能仅凭后缀认定文件可重新生成或禁止手改。本次未确认可用的绑定生成命令；后续若要生成，应先定位生成来源和配置。

## SteamHelper：多 C++ 类、统一 Lua 接口

这是用户明确指定的维护约定：Steam 的 C++ 实现按职能分到不同类中，Lua 侧统一通过 `lstg.SteamHelper` 暴露。新增职能应延续这一入口，不能因为新增 C++ 类就自行增加公开的 Lua 类。

主要参考 `frameworks/LuaBindings/lua_SteamHelper_auto.cpp` 及 `.hpp`，并联读 `frameworks/Classes/SteamAchievementHelper.hpp/.cpp`、`SteamConfigHelper.hpp/.cpp`。

| Lua 注册名称 | 当前 C++ 目标 | 职责 |
| --- | --- | --- |
| `getInstance` | `SteamAchievementHelper::getInstance` | 返回成就单例，注册为 `lstg.SteamHelper` |
| `getSteamAchievementList` | `SteamAchievementHelper::getSteamAchievementList` | 成就列表及状态 |
| `unlockAchievement` / `resetAchievement` / `getAchievementStatus` | `SteamAchievementHelper` 的对应成员 | 解锁、重置和状态查询 |
| `getSteamLanguage` / `getSteamID` / `getUserName` | `SteamConfigHelper` 的对应成员 | 配置和用户信息 |

跨类聚合必须显式适配实际接收者。例如包装配置类的成员函数时，可用 lambda 内部调用 `SteamConfigHelper::getInstance()->...`，同时保留 Lua 的统一入口及调用约定；不要将成就单例当成配置类实例传入。

## 后续修改的工作顺序

1. 先确认 Lua 名称、冒号/点号调用方式、参数及返回值，再核对 C++ 声明、实现、对象所有权和必要的 SDK 初始化条件。
2. 在相应 `.cpp` 增加包装及 `LUA_METHOD`；新增模块时同步 `.hpp` 注册入口、`LuaBindings.cpp` 和 `CMakeSources.cmake`。已有模块新增方法通常无需新增模块注册。
3. 核对宏的参数数目、错误信息、转换特化、栈平衡、生命周期；Steam 跨类调用额外核对实际接收者。
4. 代码变更后进行对应构建及 Lua 调用验证，覆盖正常参数、错误参数、返回值形状与精度；涉及 Steam 时检查 SDK 可用和不可用情形。构建配置有 `LSTGX_USE_STEAM`，但不能仅凭该选项推定所有绑定已正确受条件保护。
