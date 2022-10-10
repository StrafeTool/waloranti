#ifndef CAPTURE_SCREEN_HPP
#define CAPTURE_SCREEN_HPP

#include "dda_impl.hpp"
#include "definitions.hpp"
#include "../utilities/utilities.hpp"
#include <vector>

class capture_screen 
{
private:
	dda_impl* m_dda_wrapper{ nullptr };
    ID3D11Device* m_device{ nullptr };
    ID3D11Texture2D* m_output_texture{ nullptr };
    ID3D11Texture2D* m_duplication_texture{ nullptr };
    ID3D11DeviceContext* m_device_context{ nullptr };
    int m_duplication_width{ 0 };
    int m_duplication_height{ 0 };
    int m_screen_width{ 0 };
    int m_screen_height{ 0 };

    HRESULT initialize( );
    HRESULT initialize_dxgi( );
    HRESULT initialize_duplication( );
    HRESULT initialize_output_texture( );
    HRESULT capture_frame( int wait, int& accumulated_frames );
    HRESULT save_frame( );

    void cleanup( bool end = true );
    ~capture_screen( ) { cleanup( true ); }

    struct vec2
    {
        double x{ 0.0 };
        double y{ 0.0 };
    };
public:
    capture_screen() { initialize( ); }
    void screenshot( int timeout = 500 );
    std::vector<int> get_rgb(int x, int y);
    int get_width( ) { return m_screen_width; }
    int get_height( ) { return m_screen_height; }
};

#endif // !CAPTURE_SCREEN_HPP