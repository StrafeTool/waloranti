#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Windows.h>

namespace cfg
{
	constexpr int aimbot_key{ VK_LBUTTON };
	constexpr int magnet_key{ VK_XBUTTON2 };
	constexpr int flick_key{ VK_XBUTTON1 };
	constexpr int head_offset_x{ 2 };
	constexpr int head_offset_y{ 4 };
	inline int magnet_fov{ 9 };
	inline int magnet_smooth{ 8 };
	inline int magnet_delay_between_shots{ 200 };
	inline int aimbot_fov{ 9 };
	inline int aimbot_smooth{ 14 };
	inline int recoil_length{ 44 };
	inline double recoil_offset{ 0 };
}

inline int RED_RANGE[] = { 200, 255 };
inline int GREEN_RANGE[] = { 0, 200 };
inline int BLUE_RANGE[] = { 200, 255 };

inline float RED_DOMINANCE_THRES[] = { 0.8f, 1.0f };
inline float GREEN_DOMINANCE_THRES[] = { 0.0f, 0.80f };
inline float BLUE_DOMINANCE_THRES[] = { 0.5f, 1.0f };

#endif // !CONFIG_HPP
