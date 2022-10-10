#ifndef RENDER_MENU_HPP
#define RENDER_MENU_HPP

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include <dxsdk-d3dx/D3DX11.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

namespace render_menu
{
	constexpr int width{ 300 };
	constexpr int height{ 210 };
	inline bool is_running{ true };

	inline ImFont* icon_font{ nullptr };
	inline HWND window{ nullptr };
	inline WNDCLASSEX window_class = { };

	static ID3D11Device* device{ NULL };
	static ID3D11DeviceContext* device_context{ NULL };
	static IDXGISwapChain* swap_chain{ NULL };
	static ID3D11RenderTargetView* render_target_view{ NULL };
	static ID3D11ShaderResourceView* background{ NULL };

	void create_window(LPCWSTR window_name);
	void destroy_window();

	bool create_device();
	void destroy_device();

	void create_render_target();
	void destory_render_target();

	void create_imgui();
	void destroy_imgui();

	void begin_render();
	void end_render();
	void render();
}

#endif // !RENDER_MENU_HPP