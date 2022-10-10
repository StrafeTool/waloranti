#ifndef ENEMY_SCANNER_HPP
#define ENEMY_SCANNER_HPP

#include "../capture_screen/capture_screen.hpp"

class enemy_scanner
{
private:
	capture_screen* capture = new capture_screen( );
	struct vec2 
	{
		double x{ 0.0 };
		double y{ 0.0 };
	};

public:
	enemy_scanner( ) { };

	void update()
	{
		if ( !this->capture) { return; }
		this->capture->screenshot( );
	}

	bool is_enemy_outline( const std::vector<int>& pixel ) const;
	bool is_enemy_outline_old( const std::vector< int >& pixel ) const;
	bool is_enemy_in_crosshair( ) const;
	std::vector< int > find_closest_enemy_head( int fov_type ) const;
};

#endif // !ENEMY_SCANNER_HPP
