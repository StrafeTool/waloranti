#ifndef DDA_IMPL_HPP
#define DDA_IMPL_HPP

#include <dxgi1_2.h>
#include <d3d11_2.h>

class dda_impl 
{
private:
    IDXGIOutputDuplication* m_duplication{ nullptr };
    ID3D11Device* m_d3d_device{ nullptr };
    ID3D11DeviceContext* m_device_context{ nullptr };
    IDXGIResource* m_resource{ nullptr };
    DWORD m_width{ 0 };
    DWORD m_height{ 0 };
    dda_impl( ) { }
public:
    HRESULT initialize( );
    HRESULT get_captured_frame( ID3D11Texture2D** texture_2d, int wait, int& accumulated_frames );
    int cleanup( );
    inline DWORD get_width( ) { return m_width; }
    inline DWORD get_height( ) { return m_height; }

    dda_impl( ID3D11Device* device, ID3D11DeviceContext* device_context )
        : m_d3d_device( device ), m_device_context( device_context )
    {
        m_d3d_device->AddRef( );
        m_device_context->AddRef( );
    }

    ~dda_impl( ) { cleanup( ); }
};

#endif // !DDA_IMPL_HPP
