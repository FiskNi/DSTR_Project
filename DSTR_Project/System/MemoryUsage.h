#ifndef _MEMORY_USAGE_H
#define _MEMORY_USAGE_H
#include <Pch/Pch.h>

class MemoryUsage {
public:
	void updateVramUsage();
	void updateRamUsage();
	void updateBoth();
	void printBoth(std::string string = "");

	const float& getCurrentVramUsage() const;
	const float& getCurrentRamUsage() const;

	const float& getHighestVramUsage() const;
	const float& getHighestRamUsage() const;

private:
	float m_vramUsage = 0.0f;;
	float m_ramUsage = 0.0f;
	float m_highestVramUsage = 0.0f;
	float m_highestRamUsage = 0.0;

};

#endif
