//
//  ldLoopbackAudioDeviceWorker.cpp
//  ldCore
//
//  Created by Sergey Gavrushkin on 09/08/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldLoopbackAudioDeviceWorker.h"

#include <QtDebug>

#include <QtCore/QTimer>
#include <QtMultimedia/QAudioDeviceInfo>


#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>


// ---------------- anonymous helper namespace ----------------

namespace {

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres, withSignal)  \
              if (FAILED(hres)) { \
                qWarning() << "EXIT_ON_ERROR"  <<  __LINE__ << hres; \
                if(withSignal) {\
                    emit error(tr("Can't initialize sound input device. \
                                  \nLine: %1, Res: %2. " \
"                                 \nPlease forward this info to WickedLasers") \
                        .arg(__LINE__) \
                        .arg(QString::number(hres)) \
                        ); \
                } \
                goto Exit; \
              }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

class ReleaseOnExit {
public:
    ReleaseOnExit(IUnknown *p) : m_p(p) {}
    ~ReleaseOnExit() {
        m_p->Release();
    }

private:
    IUnknown *m_p;
};


class PropVariantClearOnExit {
public:
    PropVariantClearOnExit(PROPVARIANT *p) : m_p(p) {}
    ~PropVariantClearOnExit() {
        HRESULT hr = PropVariantClear(m_p);
        if (FAILED(hr)) {
            qWarning() << "PropVariantClear failed: hr = " << hr;
        }
    }

private:
    PROPVARIANT *m_p;
};

HRESULT get_specific_device(QString deviceName, IMMDevice **ppMMDevice) {
    HRESULT hr = S_OK;

    LPCWSTR szLongName = (const wchar_t*) deviceName.utf16();
    *ppMMDevice = NULL;

    // get an enumerator
    IMMDeviceEnumerator *pMMDeviceEnumerator;


   CoInitialize(NULL);
   hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&pMMDeviceEnumerator
    );
    if (FAILED(hr)) {
        qWarning() << "CoCreateInstance(IMMDeviceEnumerator) failed: hr = " << hr;
        return hr;
    }
    ReleaseOnExit releaseMMDeviceEnumerator(pMMDeviceEnumerator);

    IMMDeviceCollection *pMMDeviceCollection;

    // get all the active render endpoints
    hr = pMMDeviceEnumerator->EnumAudioEndpoints(
        eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection
    );
    if (FAILED(hr)) {
        qWarning() << "IMMDeviceEnumerator::EnumAudioEndpoints failed: hr = " << hr;
        return hr;
    }
    ReleaseOnExit releaseMMDeviceCollection(pMMDeviceCollection);

    UINT count;
    hr = pMMDeviceCollection->GetCount(&count);
    if (FAILED(hr)) {
        qWarning() << "IMMDeviceCollection::GetCount failed: hr = " << hr;
        return hr;
    }

    for (UINT i = 0; i < count; i++) {
        IMMDevice *pMMDevice;

        // get the "n"th device
        hr = pMMDeviceCollection->Item(i, &pMMDevice);
        if (FAILED(hr)) {
            qWarning() << "IMMDeviceCollection::Item failed: hr = " << hr;
            return hr;
        }
        ReleaseOnExit releaseMMDevice(pMMDevice);

        // open the property store on that device
        IPropertyStore *pPropertyStore;
        hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
        if (FAILED(hr)) {
            qWarning() << "IMMDevice::OpenPropertyStore failed: hr = " << hr;
            return hr;
        }
        ReleaseOnExit releasePropertyStore(pPropertyStore);

        // get the long name property
        PROPVARIANT pv; PropVariantInit(&pv);
        hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
        if (FAILED(hr)) {
            qWarning() << "IPropertyStore::GetValue failed: hr = " << hr;
            return hr;
        }
        PropVariantClearOnExit clearPv(&pv);

        if (VT_LPWSTR != pv.vt) {
            qWarning() << "PKEY_Device_FriendlyName variant type is" <<pv.vt <<  "- expected VT_LPWSTR";
            return E_UNEXPECTED;
        }

        // is it a match?
        if (0 == _wcsicmp (pv.pwszVal, szLongName)) {
			// did we already find it?onli
            if (NULL == *ppMMDevice) {
                *ppMMDevice = pMMDevice;
                pMMDevice->AddRef();
            } else {
                qWarning() << "Found (at least) two devices named " << szLongName;
                return E_UNEXPECTED;
            }
        }
    }

    if (NULL == *ppMMDevice) {
        qWarning() << "Could not find a device named " << deviceName;
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return S_OK;
}


}

// ---------------- ldLoopbackAudioDeviceWorker ----------------


ldLoopbackAudioDeviceWorker::ldLoopbackAudioDeviceWorker(QString device, QObject *parent)
    : QObject(parent)
    , m_device(device)
    , m_mutex(QMutex::Recursive)
{
}

ldLoopbackAudioDeviceWorker::~ldLoopbackAudioDeviceWorker()
{
}

void ldLoopbackAudioDeviceWorker::stop()
{
    QMutexLocker lock(&m_mutex);
    m_stop = true;
}


void ldLoopbackAudioDeviceWorker::process()
{

	//qDebug() << "loopback process";

    QMutexLocker lock(&m_mutex);

    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = AUDIO_UPDATE_DELTA_S * REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BYTE *pData;
    DWORD flags;


    /// Get WAVEFORMATEX from device
    hr = get_specific_device(m_device, &pDevice);
    if (FAILED(hr)) {
        emit error(tr("Can't find device %1").arg(m_device));
        EXIT_ON_ERROR(hr, false)
    }

    hr = pDevice->Activate(
                    __uuidof(IAudioClient), CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr, true)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr, true)

    /// 16 bit sound coverter

    // coerce int-XX wave format (like int-16 or int-32)
    // can do this in-place since we're not changing the size of the format
    // also, the engine will auto-convert from float to int for us
    switch (pwfx->wFormatTag) {
        case WAVE_FORMAT_IEEE_FLOAT:
            Q_ASSERT(false);// we never get here...I never have anyway...my guess is windows vista+ by default just uses WAVE_FORMAT_EXTENSIBLE
            pwfx->wFormatTag = WAVE_FORMAT_PCM;
            pwfx->wBitsPerSample = 16;
            pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
            pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
            break;

        case WAVE_FORMAT_EXTENSIBLE: // 65534
            {
                // naked scope for case-local variable
                PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
                if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
                    // WE GET HERE!
                    pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                    // convert it to PCM, but let it keep as many bits of precision as it has initially...though it always seems to be 32
                    // comment this out and set wBitsPerSample to  pwfex->wBitsPerSample = getBitsPerSample(); to get an arguably "better" quality 32 bit pcm
                    // unfortunately flash media live encoder basically rejects 32 bit pcm, and it's not a huge gain sound quality-wise, so disabled for now.
                    pwfx->wBitsPerSample = 16;
                    pEx->Samples.wValidBitsPerSample = pwfx->wBitsPerSample;
                    pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
                    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
                    // see also setupPwfex method
                } else {
                    qWarning() << "Don't know how to coerce mix format to int-16";
                    CoTaskMemFree(pwfx);
                    pAudioClient->Release();
                    return;
                }
            }
            break;

        default:
            qWarning() << "Don't know how to coerce WAVEFORMATEX with wFormatTag = 0x%08x to int-16" << pwfx->wFormatTag;
            CoTaskMemFree(pwfx);
            pAudioClient->Release();
            return;
    }

    /// AudioClient initialization
    /// https://msdn.microsoft.com/en-us/library/windows/desktop/dd370875(v=vs.85).aspx
    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         AUDCLNT_STREAMFLAGS_LOOPBACK,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    if(hr == AUDCLNT_E_DEVICE_IN_USE) {
        emit error(tr("Device is used in exclusive mode, can't connect"));
        EXIT_ON_ERROR(hr, false)
    } else {
        EXIT_ON_ERROR(hr, true)
    }

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	qDebug() << "buffer frame count" << bufferFrameCount;
    EXIT_ON_ERROR(hr, true)

    hr = pAudioClient->GetService(
                         __uuidof(IAudioCaptureClient),
                         (void**)&pCaptureClient); // AUDCLNT_E_WRONG_ENDPOINT_TYPE

    EXIT_ON_ERROR(hr, true)

    /// Notify the audio sink which format to use.


    m_format.setSampleRate(pwfx->nSamplesPerSec); // 44100
    m_format.setChannelCount(pwfx->nChannels); // 2
    m_format.setSampleSize(pwfx->wBitsPerSample);
    if(pwfx->wFormatTag = WAVE_FORMAT_PCM) {
        m_format.setSampleType(QAudioFormat::SignedInt);
    }
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setCodec("audio/pcm");

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                     bufferFrameCount / pwfx->nSamplesPerSec;

	qDebug() << "buffer frame count is " << bufferFrameCount;
    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr, true)

	// done with lock?
	lock.unlock();

    // Each loop fills about half of the shared buffer.
    while (!m_stop)
    {
        

        // Sleep for half the buffer duration.
		long stime = hnsActualDuration / REFTIMES_PER_MILLISEC / 2;
		//qDebug() << "sleeping " << stime;
        Sleep(stime);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
		//qDebug() << "packet is " << packetLength;
        // errorcodes: https://msdn.microsoft.com/en-us/library/windows/desktop/dd370860(v=vs.85).aspx
        if(hr == AUDCLNT_E_DEVICE_INVALIDATED) {
            emit error(tr("Device has been disconnected"));
            EXIT_ON_ERROR(hr, false)
        } else {
            EXIT_ON_ERROR(hr, true)
        }

        while (packetLength != 0)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(
                                   &pData,
                                   &numFramesAvailable,
                                   &flags, NULL, NULL);
            EXIT_ON_ERROR(hr, true)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
            }
						
            // Copy the available capture data to the audio sink.
            emit soundUpdated((const char*) pData, numFramesAvailable*4);

            EXIT_ON_ERROR(hr, true)

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr, true)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr, true)
        }
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr, true)

Exit:

    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)

    emit finished();
}


// function to enumerate devices available
// client app uses this to populate list of devices user can select
// and have the proper name string that should be sent when initializing a device

QStringList ldLoopbackAudioDeviceWorker::getAvailableOutputDevices()
{
    QStringList list;
	HRESULT hr = S_OK;

	// get an enumerator
	IMMDeviceEnumerator *pMMDeviceEnumerator;

	CoInitialize(NULL);
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pMMDeviceEnumerator
	);
	if (FAILED(hr)) {
		qWarning() << "CoCreateInstance(IMMDeviceEnumerator) failed: hr = " << hr;
        return list;
	}
	ReleaseOnExit releaseMMDeviceEnumerator(pMMDeviceEnumerator);

	IMMDeviceCollection *pMMDeviceCollection;

	// get all the active render endpoints
	hr = pMMDeviceEnumerator->EnumAudioEndpoints(
		eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection
	);
	if (FAILED(hr)) {
		qWarning() << "IMMDeviceEnumerator::EnumAudioEndpoints failed: hr = " << hr;
        return list;
	}
	ReleaseOnExit releaseMMDeviceCollection(pMMDeviceCollection);

	UINT count;
	hr = pMMDeviceCollection->GetCount(&count);
	if (FAILED(hr)) {
		qWarning() << "IMMDeviceCollection::GetCount failed: hr = " << hr;
        return list;
	}

	for (UINT i = 0; i < count; i++) {
		IMMDevice *pMMDevice;

		// get the "n"th device
		hr = pMMDeviceCollection->Item(i, &pMMDevice);
		if (FAILED(hr)) {
			qWarning() << "IMMDeviceCollection::Item failed: hr = " << hr;
			continue;
		}
		ReleaseOnExit releaseMMDevice(pMMDevice);

		// open the property store on that device
		IPropertyStore *pPropertyStore;
		hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
		if (FAILED(hr)) {
			qWarning() << "IMMDevice::OpenPropertyStore failed: hr = " << hr;
			continue;
		}
		ReleaseOnExit releasePropertyStore(pPropertyStore);

		// get the long name property
		PROPVARIANT pv; PropVariantInit(&pv);
		hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
		if (FAILED(hr)) {
			qWarning() << "IPropertyStore::GetValue failed: hr = " << hr;
			continue;
		}
		PropVariantClearOnExit clearPv(&pv);

		if (VT_LPWSTR != pv.vt) {
			qWarning() << "PKEY_Device_FriendlyName variant type is" << pv.vt << "- expected VT_LPWSTR";
			continue;
		}

		// device works.  add to list
        list.append(QString::fromWCharArray(pv.pwszVal, -1));

	}
	
    return list;
}
