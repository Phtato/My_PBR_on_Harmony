


#include "include/vulkanBase/vulkan_base.h"
#include "napi/native_api.h"
#include <iostream>

#include "include/Application.h"


/*
 * 这里作为入口吧
*/
extern "C" napi_value entry(napi_env env, napi_callback_info info)
{
    auto app = std::make_unique<Application>();



    return nullptr;
}