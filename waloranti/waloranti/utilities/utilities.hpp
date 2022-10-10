#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <Windows.h>
#include <string>
#include <random>

namespace utilities
{
	bool is_pressed( int key );
	void set_timer_resolution( );
	void set_thread_priority( int priority );
	void set_process_priority( DWORD priority_class );
	void print_log( std::string_view message );
	void rename_executable( std::string file_name );
	int get_random_int( int min, int max );
	std::string get_random_process_name( );
	inline BYTE* output_texture_data{ 0 };
}

#endif // !UTILITIES_HPP
