#include "SteamAchievementHelper.hpp"

lstg::SteamAchievementHelper::SteamAchievementHelper()
{
	// 根据SDK的注释，现在Steam客户端将自动完成成就数据的获取
	// 程序内无需手动调用RequestCurrentStats拉取

}

lstg::SteamAchievementHelper::~SteamAchievementHelper()
{
	if (isSubmitting)
	{
		// 等待提交线程结束
		std::unique_lock<std::mutex> lock(submitMutex);
		submitCondition.wait(lock, [this] { return !isSubmitting; });
	}
	// 销毁前检查是否存在缓存值，最后向服务器提交一次
	if (isDataCached)
	{
		LINFO("[Steam] Submit achievement data:");
		int maxRetryCount = 5;
		while (!SteamUserStats()->StoreStats())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			if (maxRetryCount-- <= 0)
			{
				LINFO("[Steam] Submit achievement data failed.Data is cached to local disk and will be submitted on next startup.");
				this->isDataCached = true;
				break;
			}
		}
	}
}

/// @brief 从Steam客户端拉取成就信息与状态
/// @return 
std::vector<SteamAchievementStatus> lstg::SteamAchievementHelper::getSteamAchievementList()
{
	auto res = std::vector<SteamAchievementStatus>();
	
	for (auto ach_name : AchievementList)
	{
		bool status = false;
		if (SteamUserStats()->GetAchievement(ach_name.c_str(), &status))
		{
			res.push_back({ ach_name,status });
		}
		else
		{
			CC_LOG_ERROR("[Steam] Achievement %s not exists,Please connect your Boss.",ach_name.c_str());
		}
	}

	return res;
}

void lstg::SteamAchievementHelper::unlockAchievement(std::string ach_name)
{
	// 校验成就是否已经解锁
	if (getAchievementStatus(ach_name))
	{
		CC_LOG_INFO("[Steam] Achievement %s already unlock.Do nothing.",ach_name.c_str());
		return;
	}
	
	if (SteamUserStats()->SetAchievement(ach_name.c_str()))
	{
		SubmitChangeAsync();
	}
	else
		CC_LOG_ERROR("[Steam] Achievement %s not exists,Please connect your Boss.", ach_name.c_str());
}

void lstg::SteamAchievementHelper::resetAchievement(std::string ach_name)
{
	SteamUserStats()->ClearAchievement(ach_name.c_str());
	CC_LOG_INFO("[Steam] Reset achievement %s.", ach_name.c_str());
	SubmitChangeAsync();
}

bool lstg::SteamAchievementHelper::getAchievementStatus(std::string ach_name)
{
	bool status = false;
	
	if (SteamUserStats()->GetAchievement(ach_name.c_str(), &status))
	{
		return status;
	}
	else
	{
		CC_LOG_ERROR("[Steam] Achievement %s not exists,Please connect your Boss.", ach_name.c_str());
		return false;
	}
}

void lstg::SteamAchievementHelper::SubmitChangeAsync()
{
	if (isSubmitting)
	{
		// 已经有一个提交线程在运行
		// 直接阻塞该函数，直到提交线程结束
		std::unique_lock<std::mutex> lock(submitMutex);
		submitCondition.wait(lock, [this] { return !isSubmitting; });
	}
	isSubmitting = true;
	submitThread = std::thread([this]()
		{
			int maxRetryCount = 5;
			while (!SteamUserStats()->StoreStats())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
				if (maxRetryCount-- <= 0)
				{
					LINFO("[Steam] Submit achievement data failed.Data will be cached to Steam client and submitted later.");
					this->isDataCached = true;
					break;
				}
			}
			this->isDataCached = false;
			this->isSubmitting = false;
			this->submitCondition.notify_all();
		});
	submitThread.detach();
	return;
}