﻿/** @file nim_friend.h
  * @brief NIM SDK提供的friend接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author Oleg
  * @date 2015/08/13
  */

#ifndef NIM_SDK_DLL_API_NIM_FRIEND_H_
#define NIM_SDK_DLL_API_NIM_FRIEND_H_

#include "nim_sdk_dll.h"
#include "nim_friend_def.h"

#ifdef __cplusplus
extern"C"
{
#endif

/** @fn void nim_friend_reg_changed_cb(const char *json_extension, nim_friend_change_cb_func cb, const void *user_data)
  * 统一注册好友变更通知回调函数（多端同步添加、删除、更新，好友列表同步）
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			好友变更通知回调函数, nim_friend_change_cb_func回调函数定义见nim_friend_def.h
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_friend_reg_changed_cb(const char *json_extension, nim_friend_change_cb_func cb, const void *user_data);

/** @fn void nim_friend_request(const char *accid, NIMVerifyType verify_type, const char *msg, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data)
  * 添加、验证好友
  * @param[in] accid		对方帐号
  * @param[in] verify_type	好友验证类型
  * @param[in] msg			接口扩展字段
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			操作结果回调
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_friend_request(const char *accid, NIMVerifyType verify_type, const char *msg, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);

/** @fn void nim_friend_delete(const char *accid, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data)
  * 删除好友
  * @param[in] accid		对方帐号
  * @param[in] json_extension json扩展参数（备用，目前不需要）
  * @param[in] cb			操作结果回调
  * @param[in] user_data	APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_friend_delete(const char *accid, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);

/** @fn void nim_friend_update(const char *friend_json, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data)
  * 更新好友资料（自己）
  * @param[in] friend_json		好友资料json字符串
  * @param[in] json_extension	json扩展参数（备用，目前不需要）
  * @param[in] cb				操作结果回调
  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_friend_update(const char *friend_json, const char *json_extension, nim_friend_opt_cb_func cb, const void *user_data);

/** @fn void nim_friend_get_list(const char *json_extension, nim_friend_get_list_cb_func cb, const void *user_data)
  * 获取缓存好友列表
  * @param[in] json_extension	json扩展参数（备用，目前不需要）
  * @param[in] cb				操作结果回调
  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_friend_get_list(const char *json_extension, nim_friend_get_list_cb_func cb, const void *user_data);

/** @fn void nim_friend_get_profile(const char *accid, const char *json_extension, nim_friend_get_profile_cb_func cb, const void *user_data)
  * 获取缓存好友信息
  * @param[in] accid		对方帐号
  * @param[in] json_extension	json扩展参数（备用，目前不需要）
  * @param[in] cb				操作结果回调
  * @param[in] user_data		APP的自定义用户数据，SDK只负责传回给回调函数cb，不做任何处理！
  * @return void 无返回值
  */
NIM_SDK_DLL_API void nim_friend_get_profile(const char *accid, const char *json_extension, nim_friend_get_profile_cb_func cb, const void *user_data);

#ifdef __cplusplus
}
#endif

#endif //NIM_SDK_DLL_API_NIM_FRIEND_H_