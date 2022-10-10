#include "capture_screen.hpp"
#include "../utilities/config.hpp"

void capture_screen::screenshot( int timeout )
{
    HRESULT hr{ S_OK };
    int accumulated_frames{ 0 };

    do
    {
        // Try to capture a frame.
        hr = this->capture_frame( timeout, accumulated_frames );
        if ( hr == DXGI_ERROR_WAIT_TIMEOUT ) 
        { 
            return;
        }

        if ( FAILED( hr ) )
        {
            // Re-try with a new DDA.
            this->cleanup( );
            hr = this->initialize( );
            // Failed to initialize DDA.
            if ( FAILED( hr ) ) 
            { 
                return; 
            }

            // Try again to capture a frame.
            hr = this->capture_frame( timeout, accumulated_frames );
            if (FAILED(hr))
            {
                return;
            }
        }
    } while (accumulated_frames <= 0 );

    hr = this->save_frame( );
    if ( FAILED( hr ) ) 
    { 
        return; 
    }
}

HRESULT capture_screen::capture_frame( int wait, int& accumulated_frames )
{
    HRESULT hr{ S_OK };
    hr = this->m_dda_wrapper->get_captured_frame( &this->m_duplication_texture, wait, accumulated_frames );
    returnIfError( hr );
    return hr;
}

std::vector< int > capture_screen::get_rgb( int x, int y )
{
    if ( !utilities::output_texture_data )
        return { 0, 0, 0 };

    return {
        utilities::output_texture_data[ 4 * ( ( y * this->get_width( ) ) + x ) + 2 ],
        utilities::output_texture_data[ 4 * ( ( y * this->get_width( ) ) + x ) + 1 ],
        utilities::output_texture_data[ 4 * ( ( y * this->get_width( ) ) + x ) ]
    };
}


HRESULT capture_screen::save_frame( )
{
    HRESULT hr{ S_OK };   
    hr = this->initialize_output_texture( );
    returnIfError( hr );

    this->m_device_context->CopyResource( this->m_output_texture, this->m_duplication_texture );
    SAFE_RELEASE( this->m_duplication_texture );

    IDXGISurface1* surface;
    hr = this->m_output_texture->QueryInterface( IID_PPV_ARGS( &surface ) );
    returnIfError( hr );

    DXGI_MAPPED_RECT rect;
    hr = surface->Map( &rect, DXGI_MAP_READ );
    returnIfError( hr );

    utilities::output_texture_data = rect.pBits;

    hr = surface->Unmap( );
    returnIfError( hr );

    return hr;
}

HRESULT capture_screen::initialize( ) 
{
	HRESULT hr{ S_OK };

    hr = initialize_dxgi( );
    returnIfError( hr );

    hr = initialize_duplication( );
    returnIfError( hr );

    return hr;
}

HRESULT capture_screen::initialize_dxgi( )
{
    HRESULT hr{ S_OK };

    D3D_DRIVER_TYPE driver_types[ ] = 
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT num_driver_types = ARRAYSIZE( driver_types );

    D3D_FEATURE_LEVEL feature_levels[ ] = 
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };

    UINT num_feature_levels = ARRAYSIZE( feature_levels );
    D3D_FEATURE_LEVEL feature_level{ D3D_FEATURE_LEVEL_11_0 };

    for ( UINT driver_type_index{ 0 }; driver_type_index < num_driver_types; ++driver_type_index ) 
    {
        hr = D3D11CreateDevice( nullptr, driver_types[ driver_type_index ], nullptr, 0, feature_levels, 
            num_feature_levels, D3D11_SDK_VERSION, &this->m_device, &feature_level, &this->m_device_context );

        if ( SUCCEEDED( hr ) ) 
        {
            break;
        }
    }

    return hr;
}

HRESULT capture_screen::initialize_duplication( ) 
{
    HRESULT hr{ S_OK };
    if ( !this->m_dda_wrapper ) 
    {
        this->m_dda_wrapper = new dda_impl( this->m_device, this->m_device_context );
        hr = this->m_dda_wrapper->initialize( );
        this->m_screen_height = this->m_dda_wrapper->get_height();
        this->m_screen_width = this->m_dda_wrapper->get_width();
        returnIfError( hr );
    }

    return hr;
}

HRESULT capture_screen::initialize_output_texture( ) 
{
    HRESULT hr{ S_OK };

    if ( this->m_output_texture == nullptr ) 
    {
        D3D11_TEXTURE2D_DESC duplication_desc;
        this->m_duplication_texture->GetDesc( &duplication_desc );

        this->m_duplication_width = duplication_desc.Width;
        this->m_duplication_height = duplication_desc.Height;

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = this->m_duplication_width;
        desc.Height = this->m_duplication_height;
        desc.Format = duplication_desc.Format;
        desc.ArraySize = 1;
        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.MipLevels = 1;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;

        hr = this->m_device->CreateTexture2D( &desc, NULL, &this->m_output_texture );
        returnIfError(hr);
    }

    return hr;
}

void capture_screen::cleanup( bool end )
{
    if ( this->m_dda_wrapper )
    {
        this->m_dda_wrapper->cleanup( );
        delete this->m_dda_wrapper;
        this->m_dda_wrapper = nullptr;
    }

    SAFE_RELEASE( this->m_duplication_texture );

    if ( end )
    {
        SAFE_RELEASE( this->m_output_texture );
        SAFE_RELEASE( this->m_device );
        SAFE_RELEASE( this->m_device_context );
    }
}
