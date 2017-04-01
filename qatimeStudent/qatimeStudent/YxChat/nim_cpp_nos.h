﻿/** @file nim_cpp_nos.h
  * @brief NIM SDK提供的NOS云存储服务接口
  * @copyright (c) 2015-2016, NetEase Inc. All rights reserved
  * @author towik, Oleg
  * @date 2015/2/1
  */

#ifndef _NIM_SDK_CPP_NOS_H_
#define _NIM_SDK_CPP_NOS_H_

#include <string>
#include <functional>
#include "nim_cpp_talk.h"

/**
* @namespace nim
* @brief namespace nim
*/
namespace nim
{

#include "nim_nos_def.h"
#include "nim_res_code_def.h"

/** @class NOS
  * @brief NIM SDK提供的NOS云存储服务接口
  */
class NOS
{

public:
	typedef std::function<void(nim::NIMResCode res_code, const std::string& file_path, const std::string& call_id, const std::string& res_id)> DownloadMediaCallback; /**< 下载回调模板 */
	typedef std::function<void(nim::NIMResCode res_code, const std::string& url)> UploadMediaCallback; /**< 上传回调模板 */
	typedef std::function<void(__int64 completed_size, __int64 file_size)> ProgressCallback; /**< 过程回调模板 */

	/** @fn static void RegDownloadCb(const DownloadMediaCallback& cb)
	* 注册下载回调，通过注册回调获得http下载结果通知，刷新资源
	* @param[in] cb 下载的回调函数
	* @return void 无返回值
	*/
	static void RegDownloadCb(const DownloadMediaCallback& cb);

	/** @fn static bool FetchMedia(const IMMessage& msg, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress)
	* 获取资源
	* @param[in] msg 消息内容
	* @param[in] callback_result 下载的回调函数
	* @param[in] callback_progress 下载进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool FetchMedia(const IMMessage& msg/*, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress*/);

	/** @fn static bool StopFetchMedia(const IMMessage& msg)
	* 停止获取资源（目前仅对文件消息类型有效）
	* @param[in] msg 消息内容
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool StopFetchMedia(const IMMessage& msg);

	/** @fn static bool UploadResource(const std::string& local_file, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback())
	* 上传资源
	* @param[in] local_file 本地文件的完整路径，路径名必须是utf-8
	* @param[in] callback_result 上传的回调函数
	* @param[in] callback_progress 上传进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool UploadResource(const std::string& local_file, const UploadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback());

	/** @fn static bool DownloadResource(const std::string& nos_url, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback())
	* 下载资源
	* @param[in] nos_url 下载资源的URL
	* @param[in] callback_result 下载的回调函数
	* @param[in] callback_progress 下载进度的回调函数
	* @return bool 检查参数如果不符合要求则返回失败
	*/
	static bool DownloadResource(const std::string& nos_url, const DownloadMediaCallback& callback_result, const ProgressCallback& callback_progress = ProgressCallback());
};

} 

#endif //_NIM_SDK_CPP_NOS_H_