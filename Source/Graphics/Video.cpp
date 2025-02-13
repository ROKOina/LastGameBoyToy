#include "Video.h"

#define CHECK_HR(x, msg) hr = x; if( hr != S_OK ) { return false; }
#define SAFE_RELEASE(x) if(x) { (x)->Release(); x = nullptr; }

bool Texture::create(uint32_t new_xres, uint32_t new_yres, DXGI_FORMAT new_format, bool is_dynamic)
{
    xres = new_xres;
    yres = new_yres;
    format = new_format;
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = xres;
    desc.Height = yres;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    if (is_dynamic)
    {
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    ID3D11Texture2D* tex2d = nullptr;
    HRESULT hr = Graphics::Instance().GetDevice()->CreateTexture2D(&desc, nullptr, (ID3D11Texture2D**)&texture);
    if (FAILED(hr))
        return false;

    // Create a resource view so we can use the data in a shader
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = new_format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = desc.MipLevels;
    hr = Graphics::Instance().GetDevice()->CreateShaderResourceView(texture, &srv_desc, &shader_resource_view);
    if (FAILED(hr))
        return false;

    return true;
}

void Texture::destroy()
{
    SAFE_RELEASE(texture);
    SAFE_RELEASE(shader_resource_view);
}

void Texture::activate(int slot) const
{
    Graphics::Instance().GetDeviceContext()->PSSetShaderResources(slot, 1, &shader_resource_view);
}

bool Texture::updateFromIYUV(const uint8_t* data, size_t data_size)
{
    assert(data);
    D3D11_MAPPED_SUBRESOURCE ms;
    HRESULT hr = Graphics::Instance().GetDeviceContext()->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
    if (FAILED(hr))
        return false;

    uint32_t bytes_per_texel = 1;
    assert(format == DXGI_FORMAT_R8_UNORM);
    assert(data_size == xres * yres * 3 / 4);

    const uint8_t* src = data;
    uint8_t* dst = (uint8_t*)ms.pData;

    // Copy the Y lines
    uint32_t nlines = yres / 2;
    uint32_t bytes_per_row = xres * bytes_per_texel;
    for (uint32_t y = 0; y < nlines; ++y)
    {
        memcpy(dst, src, bytes_per_row);
        src += bytes_per_row;
        dst += ms.RowPitch;
    }

    // Now the U and V lines, need to add Width/2 pixels of padding between each line
    uint32_t uv_bytes_per_row = bytes_per_row / 2;
    for (uint32_t y = 0; y < nlines; ++y)
    {
        memcpy(dst, src, uv_bytes_per_row);
        src += uv_bytes_per_row;
        dst += ms.RowPitch;
    }

    Graphics::Instance().GetDeviceContext()->Unmap(texture, 0);
    return true;
}

// ---------------------------------------------------------------------------
#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return #val
#endif

LPCSTR GetGUIDNameConst(const GUID& guid)
{
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
    IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
    IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
    IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
    IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
    IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
    IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
    IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
    IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
    IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
    IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
    IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
    IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
    IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
    IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

    // Media types

    IF_EQUAL_RETURN(guid, MFMediaType_Audio);
    IF_EQUAL_RETURN(guid, MFMediaType_Video);
    IF_EQUAL_RETURN(guid, MFMediaType_Protected);
    IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
    IF_EQUAL_RETURN(guid, MFMediaType_Script);
    IF_EQUAL_RETURN(guid, MFMediaType_Image);
    IF_EQUAL_RETURN(guid, MFMediaType_HTML);
    IF_EQUAL_RETURN(guid, MFMediaType_Binary);
    IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

    IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
    IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8
    IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
    IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
    IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
    IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
    IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
    IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

    IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM
    IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT
    IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS
    IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF
    IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF
    IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9
    IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3
    IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG
    IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC
    IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC

    return NULL;
}

#define CHECKHR_GOTO(x,y) if(x != S_OK) goto y;

// -------------------------------------------------------------------
struct VideoTexture::InternalData
{
    IMFSourceResolver* pSourceResolver = NULL;
    IUnknown* uSource = NULL;
    IMFMediaSource* mediaFileSource = NULL;
    IMFAttributes* pVideoReaderAttributes = NULL;
    IMFSourceReader* pSourceReader = NULL;
    IMFMediaType* pReaderOutputType = NULL, * pFirstOutputType = NULL;
    MF_OBJECT_TYPE     ObjectType = MF_OBJECT_INVALID;

    // Start processing frames.
    IMFSample* pVideoSample = NULL;
    DWORD      streamIndex = 0, flags = 0;
    LONGLONG   llVideoTimeStamp = 0, llSampleDuration = 0;
    int        sampleCount = 0;
    DWORD      sampleFlags = 0;

    Texture* target_texture = nullptr;
    uint32_t  width = 0;
    uint32_t  height = 0;
    float     fps = 0.0f;

    float     clock_time = 0.0f;
    LONGLONG  video_time = 0;
    float     timeScale = 1.0f;
    bool      finished = false;
    bool      paused = false;
    bool      autoloop = true;

    bool readOutputMediaFormat() {
        assert(pSourceReader);
        HRESULT hr;

        IMFMediaType* pNativeType = nullptr;
        CHECK_HR(pSourceReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, MF_SOURCE_READER_CURRENT_TYPE_INDEX, &pNativeType),
            "Error retrieving GetNativeMediaType.");

        SAFE_RELEASE(pFirstOutputType);
        CHECK_HR(pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pFirstOutputType), "Error reading GetCurrentMediaType");

        // The sample uses the pNativeType here, but we want the resolution required to 'render' the texture as if the height is not x16 the conversion
        // YUV to RGB has problems
        CHECK_HR(MFGetAttributeSize(pFirstOutputType, MF_MT_FRAME_SIZE, &width, &height), "Read input resolution");

        // Read fps
        uint64_t Val;
        hr = pNativeType->GetUINT64(MF_MT_FRAME_RATE, &Val);
        fps = (float)HI32(Val) / (float)LO32(Val);

        SAFE_RELEASE(pNativeType);

        return true;
    }

public:

    ~InternalData() {
        if (target_texture) {
            target_texture->destroy();
            delete target_texture;
            target_texture = nullptr;
        }
        SAFE_RELEASE(pSourceResolver);
        SAFE_RELEASE(uSource);
        SAFE_RELEASE(mediaFileSource);
        SAFE_RELEASE(pVideoReaderAttributes);
        SAFE_RELEASE(pSourceReader);
        SAFE_RELEASE(pReaderOutputType);
        SAFE_RELEASE(pFirstOutputType);
        SAFE_RELEASE(pVideoSample);
    }

    bool open(const char* filename) {
        finished = false;

        wchar_t wfilename[256];
        mbstowcs_s(NULL, wfilename, sizeof(wfilename) / sizeof(wchar_t), filename, _TRUNCATE);

        HRESULT hr = S_OK;

        // Set up the reader for the file.
        CHECK_HR(MFCreateSourceResolver(&pSourceResolver),
            "MFCreateSourceResolver failed.");

        CHECK_HR(pSourceResolver->CreateObjectFromURL(
            wfilename,		        // URL of the source.
            MF_RESOLUTION_MEDIASOURCE,  // Create a source object.
            NULL,                       // Optional property store.
            &ObjectType,				        // Receives the created object type.
            &uSource					          // Receives a pointer to the media source.
        ),
            "Failed to create media source resolver for file.");

        CHECK_HR(uSource->QueryInterface(IID_PPV_ARGS(&mediaFileSource)),
            "Failed to create media file source.");

        CHECK_HR(MFCreateAttributes(&pVideoReaderAttributes, 2),
            "Failed to create attributes object for video reader.");

        CHECK_HR(pVideoReaderAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID),
            "Failed to set dev source attribute type for reader config.");

        CHECK_HR(pVideoReaderAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 1),
            "Failed to set enable video processing attribute type for reader config.");

        CHECK_HR(pVideoReaderAttributes->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV),
            "Failed to set media sub type on source reader output media type.");

        CHECK_HR(MFCreateSourceReaderFromMediaSource(mediaFileSource, pVideoReaderAttributes, &pSourceReader),
            "Error creating media source reader.");

        CHECK_HR(MFCreateMediaType(&pReaderOutputType), "Failed to create source reader output media type.");
        CHECK_HR(pReaderOutputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video), "Failed to set major type on source reader output media type.");
        CHECK_HR(pReaderOutputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_IYUV), "Failed to set media sub type on source reader output media type.");

        CHECK_HR(pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pReaderOutputType),
            "Failed to set output media type on source reader.");

        if (!readOutputMediaFormat())
            return false;

        CHECK_HR(pSourceReader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, FALSE), "Disable All Stream");
        CHECK_HR(pSourceReader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE), "Enable Video Stream");

        clock_time = 0.0f;

        if (target_texture)
            target_texture->destroy();

        // Just to ensure we have something as the first frame, and also ensure we have the real required size (height % 16 should be 0)
        update(0.0f);

        return true;
    }

    void update(float elapsed)
    {
        if (paused)
            return;

        if (finished)
            return;

        clock_time += elapsed * timeScale;

        LONGLONG uct = (LONGLONG)(clock_time * 10000000);
        if (uct < video_time)
            return;

        assert(pSourceReader);
        flags = 0;

        HRESULT hr;
        hr = pSourceReader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,                              // Flags.
            &streamIndex,                   // Receives the actual stream index.
            &flags,                         // Receives status flags.
            &llVideoTimeStamp,              // Receives the time stamp.
            &pVideoSample                   // Receives the sample or NULL.
        );

        if (!SUCCEEDED(hr))
        {
            finished = true;
            return;
        }

        video_time = llVideoTimeStamp;

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            if (autoloop)
            {
                PROPVARIANT var = { 0 };
                var.vt = VT_I8;
                hr = pSourceReader->SetCurrentPosition(GUID_NULL, var);
                clock_time = 0.0f;
            }
            else
                finished = true;
        }
        if (flags & MF_SOURCE_READERF_NEWSTREAM)
        {
            finished = true;
        }
        if (flags & MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED)
        {
            finished = true;
        }
        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
            if (!readOutputMediaFormat())
            {
                finished = true;
                return;
            }
        }

        if (pVideoSample)
        {
            hr = pVideoSample->SetSampleTime(llVideoTimeStamp);
            assert(SUCCEEDED(hr));

            hr = pVideoSample->GetSampleDuration(&llSampleDuration);
            assert(SUCCEEDED(hr));

            hr = pVideoSample->GetSampleFlags(&sampleFlags);
            assert(SUCCEEDED(hr));

            //
            IMFMediaBuffer* buf = NULL;
            DWORD bufLength;

            hr = pVideoSample->ConvertToContiguousBuffer(&buf);
            hr = buf->GetCurrentLength(&bufLength);

            byte* byteBuffer = NULL;
            DWORD buffMaxLen = 0, buffCurrLen = 0;
            hr = buf->Lock(&byteBuffer, &buffMaxLen, &buffCurrLen);
            assert(SUCCEEDED(hr));

            // Some videos report one resolution and after the first frame change the height to the next multiple of 16 (using the event MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
            if (!target_texture)
            {
                target_texture = new Texture();
                int texture_height = height * 2;
                if (!target_texture->create(width, texture_height, DXGI_FORMAT_R8_UNORM, true))
                    return;
            }

            target_texture->updateFromIYUV(byteBuffer, buffCurrLen);
            float elapsed = TimeManager::Instance().GetElapsedTime();

            hr = buf->Unlock();
            assert(SUCCEEDED(hr));

            SAFE_RELEASE(buf);
            sampleCount++;

            SAFE_RELEASE(pVideoSample);
        }
    }
};

bool VideoTexture::createAPI()
{
    HRESULT hr = MFStartup(MF_VERSION);
    return SUCCEEDED(hr);
}

void VideoTexture::destroyAPI()
{
    MFShutdown();
}

bool VideoTexture::create(const char* filename)
{

    assert(!internal_data);
    internal_data = new InternalData();
    return internal_data->open(filename);
}

void VideoTexture::destroy()
{
    if (internal_data)
        delete internal_data;
    internal_data = nullptr;
}

bool VideoTexture::update(float dt)
{
    internal_data->update(dt);
    return internal_data->finished;
}

void VideoTexture::pause()
{
    assert(internal_data);
    internal_data->paused = true;
}

void VideoTexture::resume()
{
    assert(internal_data);
    internal_data->paused = false;
}

bool VideoTexture::hasFinished()
{
    assert(internal_data);
    return internal_data->finished;
}

Texture* VideoTexture::getTexture()
{
    assert(internal_data);
    return internal_data->target_texture;
}

float VideoTexture::getAspectRatio() const
{
    return (float)internal_data->width / (float)internal_data->height;
}

void VideoTexture::SetRestart()
{
    if (internal_data->pSourceReader)
    {
        PROPVARIANT var = { 0 };
        var.vt = VT_I8;
        var.hVal.QuadPart = 0;
        HRESULT hr = internal_data->pSourceReader->SetCurrentPosition(GUID_NULL, var);
        if (FAILED(hr))
        {
        }
        internal_data->clock_time = 0.0f;
        internal_data->video_time = 0;
        internal_data->finished = false;
    }
}

void VideoTexture::SetTimeScale(float scale)
{
    internal_data->timeScale = scale;
}

void VideoTexture::SetisLoop(bool isloop)
{
    internal_data->autoloop = isloop;
}


