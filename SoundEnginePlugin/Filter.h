#pragma once
#include <AK/SoundEngine/Common/IAkPlugin.h>
#include "LookUpTable.h"
class Filter
{
public:

	static AkReal32 SampleRate;
	AkReal32 CutoffFrequence =  0.f;
	AkReal32 Q = 0.f;

private:
	AkReal32* InputSamples =  nullptr;
	AkReal32* OutputSamples = nullptr;
	SinCosTanLookupTable LookupTable;
	AkUInt16 channels = 0;

public:

	Filter()
	{

	}

	~Filter() {
		delete[] InputSamples;
		delete[] OutputSamples;
	}

	void SetChannel(AkUInt16 numChannels)
	{
		channels = numChannels;
		InputSamples	= new AkReal32[2 * numChannels]();
		OutputSamples	= new AkReal32[2 * numChannels]();

		for (AkUInt16 i = 0; i < 2 * channels; ++i) {
			InputSamples[i] = 0.0f;
			OutputSamples[i] = 0.0f;
		}
	}

	AkSampleType LowpassFilter(AkSampleType Sample, AkUInt16 channel)
	{
		AkReal32 Cutoff_Frequence = CutoffFrequence / SampleRate;
		AkReal32 Beta = 0.5f * (1.0f - LookupTable.sin(Cutoff_Frequence) / 2.0f * Q) / (1.0f + LookupTable.sin(Cutoff_Frequence) / 2.0f * Q);
		AkReal32 Gamma = (0.5f + Beta) * LookupTable.cos(Cutoff_Frequence);

		AkReal32 Common_Coeff = 0.5f + Beta - Gamma;

		AkReal32 a0 = Common_Coeff / 2.0f;
		AkReal32 a1 = Common_Coeff;
		AkReal32 a2 = a0;

		AkReal32 b1 = -2.0f * Gamma;
		AkReal32 b2 = 2.0f * Beta;

		AkSampleType outputSample = a0 * Sample + a1 * InputSamples[2 * channel] + a2 * InputSamples[2 * channel + 1] -
			b1 * OutputSamples[2 * channel] - b2 * OutputSamples[2 * channel + 1];

		InputSamples[2 * channel + 1] = InputSamples[2 * channel];
		InputSamples[2 * channel] = Sample;

		OutputSamples[2 * channel + 1] = OutputSamples[0];
		OutputSamples[2 * channel] = outputSample;

		return outputSample;
	}

	AkSampleType HighpassFilter(AkSampleType Sample,  AkUInt16 channel)
	{
		AkReal32 Cutoff_Frequence = CutoffFrequence / SampleRate;
		AkReal32 Beta = 0.5f * (1.0f - LookupTable.sin(Cutoff_Frequence) / 2.0f * Q) / (1.0f + LookupTable.sin(Cutoff_Frequence) / 2.0f * Q);
		AkReal32 Gamma = (0.5f + Beta) * LookupTable.cos(Cutoff_Frequence);

		AkReal32 Common_Coeff = 0.5f + Beta + Gamma;

		AkReal32 a0 = Common_Coeff / 2.0f;
		AkReal32 a1 = -Common_Coeff;
		AkReal32 a2 = a0;

		AkReal32 b1 = -2.0f * Gamma;
		AkReal32 b2 = 2.0f * Beta;

		AkSampleType outputSample = a0 * Sample + a1 * InputSamples[2 * channel] + a2 * InputSamples[2 * channel + 1] -
			b1 * OutputSamples[2 * channel] - b2 * OutputSamples[2 * channel + 1];

		InputSamples[2 * channel + 1] = InputSamples[2 * channel];
		InputSamples[2 * channel] = Sample;

		OutputSamples[2 * channel + 1] = OutputSamples[2 * channel];
		OutputSamples[2 * channel] = outputSample;

		return outputSample;
	}

	AkSampleType BandPassFilter(AkSampleType Sample , AkUInt16 channel)
	{
		AkReal32 K = LookupTable.tan(CutoffFrequence * (0.5f * SampleRate));
		AkReal32 Beta = 1.0f / (K * K * Q + K + Q);


		AkReal32 a0 = K * Beta;
		//AkReal32 a1 = 0.0f;
		AkReal32 a2 = -a0;

		AkReal32 b1 = 2.0f * Q * (K * K - 1.0f) * Beta;
		AkReal32 b2 = (K * K * Q - K + Q) * Beta;

		AkSampleType outputSample = a0 * Sample /* + a1 * InputSamples[2 * channel]*/ + a2 * InputSamples[2 * channel + 1] -
			b1 * OutputSamples[2 * channel] - b2 * OutputSamples[2 * channel + 1];

		InputSamples[2 * channel + 1] = InputSamples[2 * channel];
		InputSamples[2 * channel] = Sample;

		OutputSamples[2 * channel + 1] = OutputSamples[2 * channel];
		OutputSamples[2 * channel] = outputSample;

		return outputSample;
	}

	Filter& operator=(Filter&& other) noexcept {
		if (this != &other) {
			delete[] InputSamples;
			delete[] OutputSamples;

			channels = other.channels;
			InputSamples = other.InputSamples;
			OutputSamples = other.OutputSamples;

			other.InputSamples = nullptr;
			other.OutputSamples = nullptr;
		}
		return *this;
	}
};

AkReal32 Filter::SampleRate = 48000.0f;


//Filter(AkUInt16 numChannels)
//	: channels(numChannels), InputSamples(new AkReal32[2 * numChannels]()), OutputSamples(new AkReal32[2 * numChannels]()) {
//	// Initialize arrays with zeros
//	for (AkUInt16 i = 0; i < 2 * channels; ++i) {
//		InputSamples[i] = 0.0f;
//		OutputSamples[i] = 0.0f;
//	}
//}

