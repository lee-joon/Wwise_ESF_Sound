/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#include "ESF_of_SoundFX.h"
#include "../ESF_of_SoundConfig.h"


#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreateESF_of_SoundFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, ESF_of_SoundFX());
}

AK::IAkPluginParam* CreateESF_of_SoundFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, ESF_of_SoundFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(ESF_of_SoundFX, AkPluginTypeEffect, ESF_of_SoundConfig::CompanyID, ESF_of_SoundConfig::PluginID)

ESF_of_SoundFX::ESF_of_SoundFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

ESF_of_SoundFX::~ESF_of_SoundFX()
{
}

AKRESULT ESF_of_SoundFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (ESF_of_SoundFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;
    Filter::SampleRate = 1.0f / static_cast<AkReal32>(in_rFormat.uSampleRate);

    BandPassFIlter.SetChannel(in_rFormat.channelConfig.uNumChannels);
    BandPassFIlter.CutoffFrequence = m_pParams->RTPC.Frequence;
    BandPassFIlter.Q = m_pParams->RTPC.Q;

    return AK_Success;
}

AKRESULT ESF_of_SoundFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT ESF_of_SoundFX::Reset()
{
    return AK_Success;
}

AKRESULT ESF_of_SoundFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    //out_rPluginInfo.eType = AkPluginTypeMixer;
    out_rPluginInfo.bIsInPlace = true;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    //out_rPluginInfo.bIsDeviceEffect = true;
    return AK_Success;
}

void ESF_of_SoundFX::Execute(AkAudioBuffer* io_pBuffer)
{
    const AkUInt32 uNumChannels = io_pBuffer->NumChannels();

    AkUInt16 uFramesProcessed;
    for (AkUInt32 i = 0; i < uNumChannels; ++i)
    {
        AkReal32* AK_RESTRICT pBuf = (AkReal32* AK_RESTRICT)io_pBuffer->GetChannel(i);

        uFramesProcessed = 0;
        while (uFramesProcessed < io_pBuffer->uValidFrames)
        {
            pBuf[uFramesProcessed] = pBuf[uFramesProcessed] + BandPassFIlter.BandPassFilter(pBuf[uFramesProcessed], i) * m_pParams->RTPC.plus_minus * m_pParams->RTPC.Multi;
            ++uFramesProcessed;
        }
    }
    //io_pBuffer->eState = AK_UnsupportedChannelConfig;
    //io_pBuffer->eState = AK_DataReady;
}

AKRESULT ESF_of_SoundFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
