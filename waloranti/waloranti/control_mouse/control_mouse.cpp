#include "control_mouse.hpp"
#include "../utilities/skcrypt.hpp"
#include "../utilities/utilities.hpp"
#include "../utilities/config.hpp"
#include <iostream>
#include <chrono>
#include <thread>

control_mouse::control_mouse( const char* com_port, DWORD baud_rate )
{
	this->m_connected = false;
	this->m_handler = CreateFileA( static_cast<LPCSTR>(com_port),
		GENERIC_READ | GENERIC_WRITE, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	DWORD error_message{ ::GetLastError( ) };

	if ( error_message == ERROR_FILE_NOT_FOUND)
	{
		utilities::print_log(_crypt_str("Handle was not attached, COM port not available"));
		CloseHandle( this->m_handler );
	}
	else if ( error_message == ERROR_ACCESS_DENIED ) 
	{
		utilities::print_log(_crypt_str("Another application is already using the COM port"));
		CloseHandle( this->m_handler );
	}
	else if ( error_message == ERROR_SUCCESS ) 
	{
		DCB serialParameters = { 0 };

		if ( !GetCommState( this->m_handler, &serialParameters ) ) 
		{
			utilities::print_log(_crypt_str("Failed to get current COM port parameters"));
			CloseHandle( this->m_handler );
		}
		else 
		{
			serialParameters.BaudRate = baud_rate;
			serialParameters.ByteSize = 8;
			serialParameters.StopBits = ONESTOPBIT;
			serialParameters.Parity = NOPARITY;
			serialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if ( !SetCommState( this->m_handler, &serialParameters ) ) 
			{
				utilities::print_log(_crypt_str("Failed to set COM port parameters"));
				CloseHandle( this->m_handler );
			}

			COMMTIMEOUTS cto = { 0 };
			cto.ReadIntervalTimeout = 50;
			cto.ReadTotalTimeoutConstant = 50;
			cto.ReadTotalTimeoutMultiplier = 10;
			cto.WriteTotalTimeoutConstant = 50;
			cto.WriteTotalTimeoutMultiplier = 10;
			if ( !SetCommTimeouts( this->m_handler, &cto ) )
			{
				utilities::print_log(_crypt_str("Failed to set COM time-out parameters"));
				CloseHandle( this->m_handler );
			}
			else 
			{
				this->m_connected = true;
				PurgeComm( this->m_handler, PURGE_RXCLEAR | PURGE_TXCLEAR );
				utilities::print_log(_crypt_str("Connected to COM port"));
			}
		}
	}
}

control_mouse::~control_mouse( )
{
	if ( this->m_connected ) 
	{
		this->m_connected = false;
		CloseHandle( this->m_handler );
	}
}

bool control_mouse::is_connected( ) 
{
	if (!ClearCommError(this->m_handler, &this->m_errors, &this->m_status)) 
	{
		this->m_connected = false;
	}
	return this->m_connected;
}

bool control_mouse::write_port( const char* buffer ) 
{
	DWORD sentBytes{ };

	if ( !WriteFile( this->m_handler, ( void* )buffer, strlen( buffer ), &sentBytes, NULL ) ) 
	{
		ClearCommError( this->m_handler, &this->m_errors, &this->m_status );
		return false;
	}
	return true;
}

bool control_mouse::send_coordinates(int x, int y)
{
	if (!this->m_connected)
		return false;

	char data[255] = "";

	char bufferX[100];
	sprintf_s(bufferX, "%d", x);

	char bufferY[100];
	sprintf_s(bufferY, "%d", y);

	strcat_s(data, bufferX);
	strcat_s(data, ",");
	strcat_s(data, bufferY);

	return this->write_port(data);
}

void add_overflow(double& Input, double& Overflow)
{
	Overflow = std::modf(Input, &Input) + Overflow;

	if (Overflow > 1.0)
	{
		double Integral{ 0.0 };
		Overflow = std::modf(Overflow, &Integral);
		Input += Integral;
	}
}

void control_mouse::move( double x, double y, double smoothing ) 
{
	double x_{ 0.0 }, y_{ 0.0 }, overflow_x{ 0.0 }, overflow_y{ 0.0 };

	double u_x{ x / smoothing };
	double u_y{ y / smoothing };

	for ( int i{ 1 }; i <= smoothing; ++i )
	{
		if ( !utilities::is_pressed( cfg::aimbot_key ) && !utilities::is_pressed( cfg::magnet_key ) && !utilities::is_pressed( cfg::flick_key ) ) { break; }

		double xI{ i * u_x };
		double yI{ i * u_y };

		add_overflow( xI, overflow_x );
		add_overflow( yI, overflow_y );

		int final_x{ static_cast< int >( xI - x_ ) };
		int final_y{ static_cast< int >( yI - y_ ) };

		/* Only try to move mouse if we're actually moving more than 0 pixels */
		if (final_x != 0 || final_y != 0)
		{
			this->send_coordinates( final_x, final_y );
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		x_ = xI; y_ = yI;
	}
}

bool control_mouse::click( )
{
	if ( !this->m_connected ) { return false; }

	if ( utilities::is_pressed( VK_LBUTTON ) ) { return false;  }

	return this->send_coordinates( 0, 0 );
}
