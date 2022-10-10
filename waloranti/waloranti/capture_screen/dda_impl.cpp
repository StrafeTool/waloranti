#include "dda_impl.hpp"
#include "definitions.hpp"

HRESULT dda_impl::initialize( )
{
    IDXGIOutput* output{ nullptr };
    IDXGIDevice2* device{ nullptr };
    IDXGIFactory1* factory{ nullptr };
    IDXGIAdapter* adapter{ nullptr };
    IDXGIOutput1* output1{ nullptr };

    /// Release all temporary refs before exit
    #define CLEAN_RETURN( x ) \
    SAFE_RELEASE( device );\
    SAFE_RELEASE( factory );\
    SAFE_RELEASE( output );\
    SAFE_RELEASE( output1 );\
    SAFE_RELEASE( adapter );\
    return x;

    HRESULT hr{ S_OK };
    /// To create a DDA object given a D3D11 device, we must first get to the DXGI Adapter associated with that device
    if ( FAILED( hr = this->m_d3d_device->QueryInterface( __uuidof( IDXGIDevice2 ), ( void** ) &device ) ) )
    {
        CLEAN_RETURN( hr );
    }

    if ( FAILED( hr = device->GetParent( __uuidof( IDXGIAdapter ), ( void** ) &adapter ) ) )
    {
        CLEAN_RETURN( hr );
    }
    /// Once we have the DXGI Adapter, we enumerate the attached display outputs, and select which one we want to capture
    /// This sample application always captures the primary display output, enumerated at index 0.
    if ( FAILED ( hr = adapter->EnumOutputs( 0, &output ) ) )
    {
        CLEAN_RETURN( hr );
    }

    if ( FAILED( hr = output->QueryInterface( __uuidof( IDXGIOutput1 ), ( void** ) &output1 ) ) )
    {
        CLEAN_RETURN( hr );
    }
    /// Ask DXGI to create an instance of IDXGIOutputDuplication for the selected output. We can now capture this display output
    if ( FAILED( hr = output1->DuplicateOutput( device, &this->m_duplication ) ) )
    {
        CLEAN_RETURN( hr );
    }

    DXGI_OUTDUPL_DESC outDesc;
    ZeroMemory( &outDesc, sizeof( outDesc ) );
    this->m_duplication->GetDesc( &outDesc );

    this->m_height = outDesc.ModeDesc.Height;
    this->m_width = outDesc.ModeDesc.Width;
    CLEAN_RETURN( hr );
}

/// Acquire a new frame from DDA, and return it as a Texture2D object.
/// 'wait' specifies the time in milliseconds that DDA shoulo wait for a new screen update.
HRESULT dda_impl::get_captured_frame( ID3D11Texture2D** texture_2d, int wait, int& accumulated_frames ) {
    HRESULT hr{ S_OK };
    DXGI_OUTDUPL_FRAME_INFO frame_info;
    ZeroMemory( &frame_info, sizeof( frame_info ) );
    accumulated_frames = 0;

    if ( this->m_resource )
    {
        this->m_duplication->ReleaseFrame( );
        this->m_resource->Release( );
        this->m_resource = nullptr;
    }

    hr = this->m_duplication->AcquireNextFrame( wait, &frame_info, &this->m_resource );
    returnIfError( hr );

    accumulated_frames = frame_info.AccumulatedFrames;
    if ( frame_info.AccumulatedFrames == 0 || frame_info.LastPresentTime.QuadPart == 0 )
    {
        return hr;
    }

    if ( !this->m_resource )
    {
        return E_UNEXPECTED;
    }

    if ( FAILED( hr = this->m_resource->QueryInterface( __uuidof( ID3D11Texture2D ), ( void** ) texture_2d ) ) )
    {
        return hr;
    }

    return hr;
}

/// Release all resources
int dda_impl::cleanup( )
{ 
    if ( this->m_resource )
    {
        this->m_duplication->ReleaseFrame( );
        SAFE_RELEASE( this->m_resource );
    }

    this->m_width = this->m_height = 0;

    SAFE_RELEASE( this->m_duplication );
    SAFE_RELEASE( this->m_device_context );
    SAFE_RELEASE( this->m_d3d_device );

    return 0;
}