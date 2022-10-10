#include "enemy_scanner.hpp"
#include "../utilities/config.hpp"

bool enemy_scanner::is_enemy_outline( const std::vector<int>& pixel ) const
{
    float max = static_cast< float >( max( pixel[0], max( pixel[1], pixel[2] ) ) );
    float dominance[3] = { pixel[0] / max, pixel[1] / max, pixel[2] / max };
    if (pixel[0] >= RED_RANGE[0] && pixel[0] <= RED_RANGE[1] &&
        pixel[1] >= GREEN_RANGE[0] && pixel[1] <= GREEN_RANGE[1] &&
        pixel[2] >= BLUE_RANGE[0] && pixel[2] <= BLUE_RANGE[1] &&
        dominance[0] >= RED_DOMINANCE_THRES[0] && dominance[0] <= RED_DOMINANCE_THRES[1] &&
        dominance[1] >= GREEN_DOMINANCE_THRES[0] && dominance[1] <= GREEN_DOMINANCE_THRES[1] &&
        dominance[2] >= BLUE_DOMINANCE_THRES[0] && dominance[2] <= BLUE_DOMINANCE_THRES[1])
        return true;

    return false;
}

bool enemy_scanner::is_enemy_outline_old( const std::vector< int >& pixel ) const
{
    if (pixel[1] >= 170) {
        return false;
    }

    if (pixel[1] >= 120) {
        return abs(pixel[0] - pixel[2]) <= 8 &&
            pixel[0] - pixel[1] >= 50 &&
            pixel[2] - pixel[1] >= 50 &&
            pixel[0] >= 105 &&
            pixel[2] >= 105;
    }

    return abs(pixel[0] - pixel[2]) <= 13 &&
        pixel[0] - pixel[1] >= 60 &&
        pixel[2] - pixel[1] >= 60 &&
        pixel[0] >= 110 &&
        pixel[2] >= 100;
}

bool enemy_scanner::is_enemy_in_crosshair( ) const
{
    const int center_x{ this->capture->get_width( ) / 2 };
    const int center_y{ this->capture->get_height( ) / 2 };

    bool found_left_side{ false };
    bool center_aligned{ false };
    bool found_right_side{ false };

    double avg_point{ 0.0 };
    int idx{ 0 };

    std::vector< vec2 > vects;

    for ( int x{ center_x - cfg::magnet_fov }; x < center_x + cfg::magnet_fov; ++x )
    {
        for ( int y{ center_y - cfg::magnet_fov }; y < center_y + cfg::magnet_fov; ++y )
        {
            if ( !is_enemy_outline_old( this->capture->get_rgb( x, y ) ) ) { continue; }
               
            if ( x < center_x ) { found_left_side = true; }       
            else if ( x > center_x ) { found_right_side = true; }
                
            vects.push_back( vec2( x - center_x, y - center_y ) );

            for ( auto& p : vects )
            {
                ++idx;

                avg_point += sqrt( pow( p.x, 2.0 ) + pow( p.y, 2.0 ) );

                if ( !( avg_point / idx < cfg::magnet_fov ) || idx < 4 ) { continue; }
                    
                center_aligned = true;

                break;
            }

            if ( !found_right_side || !found_left_side || !center_aligned ) { continue; }          

            return true;
        }
    }

    return false;
}

std::vector< int > enemy_scanner::find_closest_enemy_head( int fov_type ) const
{
    const int center_x{ this->capture->get_width( ) / 2 };
    const int center_y{ this->capture->get_height( ) / 2 };

    for ( int y{ center_y - fov_type }; y < center_y + fov_type; ++y )
    {
        for ( int x{ center_x - (fov_type + static_cast<int>(cfg::recoil_offset)) }; x < center_x + (fov_type - static_cast<int>(cfg::recoil_offset)); ++x )
        {
            if ( !this->is_enemy_outline_old( this->capture->get_rgb( x, y ) ) ) { continue; }

            return { x - center_x + cfg::head_offset_x, y - center_y + cfg::head_offset_y + static_cast< int >( cfg::recoil_offset ) };
        }
    }
    return { 0, 0 };
}
