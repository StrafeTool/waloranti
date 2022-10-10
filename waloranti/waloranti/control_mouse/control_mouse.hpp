#ifndef CONTROL_MOUSE_HPP
#define CONTROL_MOUSE_HPP

#include <Windows.h>

class control_mouse
{
private:
	HANDLE m_handler{ nullptr };
	COMSTAT m_status{ };
	DWORD m_errors{ 0 };
	bool m_connected{ false };
	bool write_port( const char* buffer );
public:
	bool send_coordinates( int x, int y );
	control_mouse( const char* com_port, DWORD baud_rate );
	~control_mouse( );
	bool is_connected( );
	void move( double x, double y, double smoothing );
	bool click( );
};

#endif // !CONTROL_MOUSE_HPP