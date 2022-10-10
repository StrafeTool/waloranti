#include "utilities.hpp"
#include "skcrypt.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

bool utilities::is_pressed( int key )
{
	return ( GetAsyncKeyState( key ) & 0x8000);
}

std::mt19937 mt{ std::random_device{ }( ) };
int utilities::get_random_int( int min, int max ) 
{
	std::uniform_int_distribution number{ min, max };
	return number( mt );
}

std::string utilities::get_random_process_name( )
{
	std::vector< std::string > process_names
	{
		"Microsoft Store",
		"Photos",
		"Calculator",
		"Snipping Tool",
		"Paint",
		"Spotify",
		"Discord",
		"Medal",
		"Steam",
		"Disk Management",
		"Logitech G HUB",
		"NVIDIA Settings",
		"Battle.net",
		"File Explorer"
	};

	return process_names.at( utilities::get_random_int( 0, process_names.size( ) - 1 ) );
}

void utilities::rename_executable( std::string file_name )
{
	if ( file_name.find( ".exe" ) == std::string::npos ) 
	{ 
		file_name.append( ".exe" );
	}
	std::string process_name{ get_random_process_name( ) };
	SetConsoleTitleA( process_name.c_str() );
	if ( std::rename( file_name.c_str(), process_name.append( ".exe" ).c_str() ) )
	{ 
		utilities::print_log( _crypt_str( "utilities::rename_executable failed." ) );
	}
}

void utilities::set_timer_resolution( )
{
	static NTSTATUS( NTAPI * nt_set_timer_resolution )
		( IN ULONG desired_resolution, IN BOOLEAN set_resolution, OUT PULONG current_resolution ) =
		( NTSTATUS( NTAPI* )( ULONG, BOOLEAN, PULONG ) )
		::GetProcAddress( GetModuleHandle( L"ntdll.dll" ), "NtSetTimerResolution" );

	ULONG desired_resolution{ 5000UL }, current_resolution{ };
	if ( nt_set_timer_resolution( desired_resolution, TRUE, &current_resolution ) )
	{
		utilities::print_log( _crypt_str( "utilities::set_timer_resolution failed." ) );
	}
}

void utilities::set_thread_priority( int priority )
{
	if ( !SetThreadPriority( ::GetCurrentThread( ), priority ) )
	{
		utilities::print_log( _crypt_str( "utilities::set_thread_priority failed." ) );
	}
}

void utilities::set_process_priority( DWORD priority_class )
{
	if ( !SetPriorityClass( ::GetCurrentProcess( ), priority_class ) )
	{
		utilities::print_log( _crypt_str( "utilities::set_process_priority failed." ) );
	}
}

void utilities::print_log( std::string_view message )
{
	HANDLE h_console{ GetStdHandle( STD_OUTPUT_HANDLE ) };
	struct tm new_time{ };
	time_t now{ ::time( 0 ) };
	::localtime_s( &new_time, &now );

	::SetConsoleTextAttribute( h_console, 12 );
	std::cout << '[';
	::SetConsoleTextAttribute( h_console, 8 );
	std::cout << std::setfill( '0' ) << std::setw( 2 ) << new_time.tm_hour << ':' << std::setw( 2 ) << new_time.tm_min << ':' << std::setw( 2 ) << new_time.tm_sec;
	::SetConsoleTextAttribute( h_console, 12 );
	std::cout << "] ";
	::SetConsoleTextAttribute( h_console, 15 );
	std::cout << message << '\n';
}