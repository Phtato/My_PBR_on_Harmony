# 全局配置管理 - VulkanConfig

## 概述

已经将原来分散在命名空间 `vulkanNdkEnvInfo` 中的全局变量重构为一个专业的单例模式的全局配置管理类 `VulkanConfig`。

## 好处

1. **线程安全** - 所有访问都通过 mutex 保护
2. **单例模式** - 确保全局只有一个配置实例
3. **更清晰的接口** - 提供明确的 getter/setter 方法
4. **易于维护** - 集中管理所有全局配置
5. **更好的封装** - 隐藏实现细节

## 文件结构

### 头文件
- `entry/src/main/cpp/include/VulkanConfig.h` - 配置类的声明

### 实现文件
- `entry/src/main/cpp/VulkanConfig.cpp` - 配置类的实现

### 使用文件
- `entry/src/main/cpp/napi_init.cpp` - 已更新使用新的配置类

## 使用方法

### 基本用法

```cpp
// 获取单例实例
VulkanConfig& config = VulkanConfig::getInstance();

// 设置 OHOS 沙箱路径
config.setOhosPath("/path/to/sandbox");

// 获取路径
std::string path = config.getOhosPath();

// 设置资源管理器
config.setResourceManager(resourceManager);

// 获取资源管理器
NativeResourceManager* rm = config.getResourceManager();

// 设置窗口
config.setWindow(window);

// 获取窗口
OHNativeWindow* win = config.getWindow();

// 设置销毁标志
config.setDestroy(true);

// 检查是否已销毁
if (config.isDestroyed()) {
    // 处理销毁逻辑
}

// 重置所有配置
config.reset();
```

## 旧代码与新代码对比

### 旧代码（命名空间方式）
```cpp
namespace vulkanNdkEnvInfo {
    std::string ohosPath;
    NativeResourceManager *m_aAssetMgr;
    OHNativeWindow *window;
    bool isDestroy;
}

vulkanNdkEnvInfo::ohosPath = path;
std::string p = vulkanNdkEnvInfo::ohosPath;
```

### 新代码（单例模式）
```cpp
VulkanConfig::getInstance().setOhosPath(path);
std::string p = VulkanConfig::getInstance().getOhosPath();
```

## 线程安全性

所有的 getter 和 setter 方法都受到 `std::mutex` 的保护，确保在多线程环境中安全使用：

```cpp
void VulkanConfig::setOhosPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);  // 自动加锁
    ohosPath_ = path;
}  // 自动解锁
```

## 在 CMakeLists.txt 中的配置

确保 `VulkanConfig.cpp` 已添加到编译源文件列表中（通常自动添加）。

## 迁移说明

如果有其他文件使用了旧的 `vulkanNdkEnvInfo` 命名空间，需要按照上面的对比方式进行更新。

搜索并替换所有引用：
- `vulkanNdkEnvInfo::ohosPath` → `VulkanConfig::getInstance().getOhosPath()`
- `vulkanNdkEnvInfo::m_aAssetMgr` → `VulkanConfig::getInstance().getResourceManager()`
- `vulkanNdkEnvInfo::window` → `VulkanConfig::getInstance().getWindow()`
- `vulkanNdkEnvInfo::isDestroy` → `VulkanConfig::getInstance().isDestroyed()`

## 示例：完整的初始化流程

```cpp
// 1. 从 NAPI 接收沙箱路径
VulkanConfig::getInstance().setOhosPath(sandboxPath);

// 2. 初始化资源管理器
NativeResourceManager* rm = OH_ResourceManager_InitNativeResourceManager(env, obj);
VulkanConfig::getInstance().setResourceManager(rm);

// 3. 复制资源文件
auto& config = VulkanConfig::getInstance();
if (config.getResourceManager() && !config.getOhosPath().empty()) {
    copyRawFileRecursive(
        config.getResourceManager(), 
        "data", 
        config.getOhosPath() + "/data"
    );
}

// 4. 在 Surface 创建回调中设置窗口
VulkanConfig::getInstance().setWindow(nativeWindow);

// 5. 在任何地方访问配置
OHNativeWindow* window = VulkanConfig::getInstance().getWindow();
```
