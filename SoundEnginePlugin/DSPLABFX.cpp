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

#include "DSPLABFX.h"
#include "../DSPLABConfig.h"

#include <AK/AkWwiseSDKVersion.h>
#include <iostream>

AK::IAkPlugin* CreateDSPLABFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, DSPLABFX());
}

AK::IAkPluginParam* CreateDSPLABFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, DSPLABFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(DSPLABFX, AkPluginTypeEffect, DSPLABConfig::CompanyID, DSPLABConfig::PluginID)

DSPLABFX::DSPLABFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

DSPLABFX::~DSPLABFX()
{
}

AKRESULT DSPLABFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (DSPLABFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;
    m_SampleRate = in_rFormat.uSampleRate;
    delayBuffer.SetUp(m_SampleRate, m_pParams);

    return AK_Success;
}

AKRESULT DSPLABFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT DSPLABFX::Reset()
{
    return AK_Success;
}

AKRESULT DSPLABFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
	out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void DSPLABFX::Execute(AkAudioBuffer* io_pBuffer)
{
    AkUInt32 totalTailFrames = 100000;
    m_FXTailHandler.HandleTail(io_pBuffer, totalTailFrames);
    AkReal32* AK_RESTRICT pBufL = (AkReal32 * AK_RESTRICT)io_pBuffer->GetChannel(1);
    if (m_pParams->m_paramChangeHandler.HasChanged(PARAM_DELAYTIME_ID) ||
        m_pParams->m_paramChangeHandler.HasChanged(PARAM_FEEDBACK_ID))
    {
        delayBuffer.SetUp(m_SampleRate, m_pParams);
    }
    delayBuffer.Render(pBufL, io_pBuffer->uValidFrames);
}

AKRESULT DSPLABFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}
