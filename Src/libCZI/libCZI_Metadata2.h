//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************
#pragma once

#include <cstdint>
#include <string>

#include "libCZI.h"

namespace libCZI
{
	enum class DimensionChannelChannelType : std::uint8_t
	{
		Heightmap,
		PalHR,
		PalWidefield,
		SimHR,
		SimWidefield,
		SimDWF,
		AiryScanSum,
		AiryScanRawSr,
		AiryScanRaw,
		AiryScanSr,
		AiryScanVp,
		AiryScanMb,
		AiryScanRingSheppardSum,
		OnlineUnmixing,
		Unspecified
	};

	enum class DimensionChannelAcquisitionMode : std::uint8_t
	{
		WideField,
		LaserScanningConfocalMicroscopy,
		SpinningDiskConfocal,
		SlitScanConfocal,
		MultiPhotonMicroscopy,
		StructuredIllumination,
		SingleMoleculeImaging,
		TotalInternalReflection,
		FluorescenceLifetime,
		SpectralImaging,
		FluorescenceCorrelationSpectroscopy,
		NearFieldScanningOpticalMicroscopy,
		SecondHarmonicGenerationImaging,
		PALM,
		STORM,
		STED,
		TIRF,
		FSM,
		LCM,
		SPIM,
		SEM,
		ApertureCorrelation,
		Other
	};

	enum class DimensionChannelIlluminationType : std::uint8_t
	{
		Transmitted,
		Epifluorescence,
		Oblique,
		NonLinear,
		Other
	};

	enum class DimensionChannelContrastMethod : std::uint8_t
	{
		Brightfield,
		Phase,
		DIC,
		HoffmanModulation,
		ObliqueIllumination,
		PolarizedLight,
		Darkfield,
		Fluorescence,
		MultiPhotonFluorescence,
		Other
	};

	enum class DimensionChannelPinholeGeometry : std::uint8_t
	{
		Circular,
		Rectangular,
		Other
	};

	/// This struct represents either a single value or a range.
	template <typename t>
	struct RangeOrSingleValue
	{
		/// True if this struct represents a single value. In this case, the property end is invalid.
		bool singleValue;

		/// The start value of a range (in the case that singleValue is false) or the single value (if singleValue is true).
		t startOrSingleValue;

		/// The end of the range (only valid if singleValue is false).
		t end;
	};

	struct SpectrumCharacteristics
	{
		enum class InformationType
		{
			SinglePeak,
			Ranges
		};

		InformationType type;
		double singlePeak;
		std::vector<RangeOrSingleValue<double>> ranges;
	};

	class IAttributeId
	{
	public:
		virtual bool TryGetAttributeId(std::wstring* id) = 0;
		virtual ~IAttributeId() = default;
	};

	class IAttributeName
	{
	public:
		virtual bool TryGetAttributeName(std::wstring* name) = 0;
		virtual ~IAttributeName() = default;
	};

	class IDimensionChannelDetectorSettings;
	class IDimensionChannelLightSourcesSettings;
	class IDimensionChannelLightPath;
	class IDimensionChannelLaserScanInfo;
	class IDimensionChannelSPIMIlluminationSettings;
	class IDimensionChannelSPIMDetectionSettings;
	class IDimensionChannelSIMSettings;
	class IDimensionChannelPolarizingSettings;
	class IDimensionChannelAiryscanSettings;
	class IDimensionChannelRatio;

	class IDimensionChannelInfo : public IAttributeId, public IAttributeName
	{
	public:
		virtual bool TryGetChannelType(DimensionChannelChannelType* type) = 0;
		virtual bool TryGetChannelUnit(std::wstring* unit) = 0;
		virtual bool TryGetPixelType(PixelType* pixelType) = 0;
		virtual bool TryGetComponentBitCount(int* bitCount) = 0;
		virtual bool TryGetAcquisitionMode(DimensionChannelAcquisitionMode* acquisitionMode) = 0;
		virtual bool TryGetIlluminationType(DimensionChannelIlluminationType* illuminationType) = 0;
		virtual bool TryGetContrastMethod(DimensionChannelContrastMethod* contrastMethod) = 0;
		virtual bool TryGetIlluminationWavelength(SpectrumCharacteristics* illuminationwavelength) = 0;
		virtual bool TryGetDetectionWavelength(SpectrumCharacteristics* detectionwavelength) = 0;
		virtual bool TryGetExcitationWavelength(double* excitationwavelength) = 0;
		virtual bool TryGetEmissionWavelength(double* emissionwavelength) = 0;
		virtual bool TryGetEffectiveNA(double* na) = 0;
		virtual bool TryGetDyeId(std::wstring* dyeid) = 0;
		virtual bool TryGetDyeDatabaseId(std::wstring* dyedatabaseid) = 0;
		virtual bool TryGetPinholeSize(double* pinholesize) = 0;
		virtual bool TryGetPinholeSizeAiry(double* pinholesizeairy) = 0;
		virtual bool TryGetPinholeGeometry(DimensionChannelPinholeGeometry* pinholegeometry) = 0;
		virtual bool TryGetFluor(std::wstring* fluor) = 0;
		virtual bool TryGetNDFilter(double* ndfilter) = 0;
		virtual bool TryGetPockelCellSetting(int* pocketcellsetting) = 0;
		virtual bool TryGetColor(libCZI::Rgb8Color* color) = 0;
		virtual bool TryGetExposureTime(RangeOrSingleValue<std::uint64_t>* exposuretime) = 0;
		virtual bool TryGetDepthOfFocus(double* depthoffocus) = 0;
		virtual bool TryGetSectionThickness(double* sectionthickness) = 0;
		virtual std::shared_ptr<IDimensionChannelDetectorSettings> GetDetectorSettings() = 0;
		virtual std::shared_ptr<IDimensionChannelLightSourcesSettings> GetLightSourcesSettings() = 0;
		virtual std::shared_ptr<IDimensionChannelLightPath> GetLightPath() = 0;
		virtual std::shared_ptr<IDimensionChannelLaserScanInfo> GetLaserScanInfo() = 0;
		virtual std::shared_ptr<IDimensionChannelSPIMIlluminationSettings> GetSPIMIlluminationSettings() = 0;
		virtual std::shared_ptr<IDimensionChannelSPIMDetectionSettings> GetSPIMDetectionSettings() = 0;
		virtual std::shared_ptr<IDimensionChannelSIMSettings> GetSIMSettings() = 0;
		virtual std::shared_ptr<IDimensionChannelPolarizingSettings> GetPolarizingSettings() = 0;
		virtual std::shared_ptr<IDimensionChannelAiryscanSettings> GetAiryscanSettings() = 0;
		virtual bool TryGetReflector(std::wstring* reflector) = 0;
		virtual bool TryGetCondenserContrast(std::wstring* condensorContrast) = 0;
		virtual bool TryGetNACondenser(double* naCondensor) = 0;
		virtual std::shared_ptr<IDimensionChannelRatio> GetRatio() = 0;

		virtual ~IDimensionChannelInfo() {}
	};

	class IDimensionsChannelsInfo
	{
	public:
		virtual int GetChannelCount() const = 0;
		virtual std::shared_ptr<IDimensionChannelInfo> GetChannel(int i) const = 0;

		virtual ~IDimensionsChannelsInfo() {}
	};

	enum class ChannelDetectorSettingsBinning : std::uint8_t
	{
		_1x1,
		_2x2,
		_4x4,
		_8x8,
		Other
	};

	enum class ChannelDetectorSettingsShutter : std::uint8_t
	{
		Rolling,
		Global,
		Other
	};

	enum class ChannelDetectorCameraProcessingMode :std::uint8_t
	{
		None,
		Widefield,
		Sectioned,
		RawDataMode
	};

	class IDimensionChannelDetectorSettings
	{
	public:
		virtual bool TryGetDetectorRef(std::wstring* detectorRef) = 0;
		virtual bool TryGetBinning(ChannelDetectorSettingsBinning* binning) = 0;
		virtual bool TryGetShutter(ChannelDetectorSettingsShutter* shutter) = 0;
		virtual bool TryGetPhotonConversionFactor(double* conversionFactor) = 0;
		virtual bool TryGetGain(double* gain) = 0;
		virtual bool TryGetDigitalGain(double* digitalgain) = 0;
		virtual bool TryGetOffset(double* offset) = 0;
		virtual bool TryGetEMGain(double* emgain) = 0;
		virtual bool TryGetVoltage(double* voltage) = 0;
		virtual bool TryGetReadOutRate(double* readoutrate) = 0;
		virtual bool TryGetUseBrightnessContrastCorrection(bool* usebrightnesscontrastcorrection) = 0;
		virtual bool TryGetPixelMaximum(double* pixelmaximum) = 0;
		virtual bool TryGetPixelAccuracy(double* pixelaccuracy) = 0;
		virtual bool TryGetPixelScaleFactor(double* pixelscalefactor) = 0;
		virtual bool TryGetCameraProcessingMode(ChannelDetectorCameraProcessingMode* mode) = 0;

		virtual ~IDimensionChannelDetectorSettings() {}
	};

	class IDimensionChannelLightSourcesSetting
	{
	public:
		struct Polarization
		{
			bool stokesParameterValid[4];
			double stokesParameter[4];
		};
	public:
		virtual bool TryGetLightSourceRef(std::wstring* lightsourceref) = 0;
		virtual bool TryGetWavelength(double* wavelength) = 0;
		virtual bool TryGetAttenuation(double* attenuation) = 0;
		virtual bool TryGetTransmission(double* transmission) = 0;
		virtual bool TryGetIntensity(std::wstring* intensity) = 0;
		virtual bool TryGetFlashDuration(double* flashduration) = 0;
		virtual bool TryGetTIRFAngle(double* tirfangle) = 0;
		virtual bool TryGetTIRFDepthOfPenetration(double* tirfdepthofpenetration) = 0;
		virtual bool TryGetIsTIRFCalibrated(bool* isTIRFcalibrated) = 0;
		virtual bool TryGetIsNlo(bool* isnlo) = 0;
		virtual bool TryGetPolarization(Polarization* polarization) = 0;

		virtual ~IDimensionChannelLightSourcesSetting() {};
	};

	class IDimensionChannelLightSourcesSettings
	{
	public:
		virtual int GetLightSourceSettingCount() const = 0;
		virtual std::shared_ptr<IDimensionChannelLightSourcesSetting> GetLightSourceSetting(int i) const = 0;

		virtual ~IDimensionChannelLightSourcesSettings() {};
	};

	class IDimensionChannelLightPath
	{
	public:
		virtual bool TryGetExcitationFilterRef(std::wstring* excitationFilterRef) = 0;
		virtual bool TryGetDichroicFilterRef(std::wstring* dichroicRef) = 0;
		virtual bool TryGetEmissionFilterRef(std::wstring* emissionFilterRef) = 0;

		virtual ~IDimensionChannelLightPath() = default;
	};

	enum class DimensionChannelLaserScanScanningMode : std::uint8_t
	{
		Frame,
		FastFrame,
		LineSequential,
		Line,
		Spot,
		Other
	};

	enum class DimensionChannelLaserScanScanDirection : std::uint8_t
	{
		Bidirectional,
		Unidirectional
	};

	enum class DimensionChannelLaserScanAveragingMethod : std::uint8_t
	{
		Mean,
		Sum,
		Other
	};

	enum class DimensionChannelLaserScanAveragingMode : std::uint8_t
	{
		Frame,
		Line,
		Other
	};

	enum class DimensionChannelLaserScanLaserAttenuatorState : std::uint8_t
	{
		On,
		Off
	};

	class IDimensionChannelLaserScanInfo
	{
	public:
		virtual bool TryGetPixelTime(double* pixelTime) = 0;
		virtual bool TryGetLineTime(double* lineTime) = 0;
		virtual bool TryGetFrameTime(double* frameTime) = 0;
		virtual bool TryGetAveraging(int* averaging) = 0;
		virtual bool TryGetScanningMode(DimensionChannelLaserScanScanningMode* mode) = 0;
		virtual bool TryGetSampleRotation(double* sampleRotation) = 0;
		virtual bool TryGetSampleOffsetX(double* sampleOffsetX) = 0;
		virtual bool TryGetSampleOffsetY(double* sampleOffsetY) = 0;
		virtual bool TryGetScanDirection(DimensionChannelLaserScanScanDirection* scanDirection) = 0;
		virtual bool TryGetZoomX(double* zoomX) = 0;
		virtual bool TryGetZoomY(double* zoomY) = 0;
		virtual bool TryGetZoomZ(double* zoomZ) = 0;
		virtual bool TryGetCropOffset(double* cropOffset) = 0;
		virtual bool TryGetLaserAttenuatorMeas(double* lastAttenuatorMeas) = 0;
		virtual bool TryGetLaserBlanking(double* laserBlanking) = 0;
		virtual bool TryGetLaserAttenuatorBleach(double* laserAttenuatorBleach) = 0;
		virtual bool TryGetLineStep(std::uint32_t* lineStep) = 0;
		virtual bool TryGetScanSpeed(int* scanSpeed) = 0;
		virtual bool TryGetAveragingMethod(DimensionChannelLaserScanAveragingMethod* averagingMethod) = 0;
		virtual bool TryGetAveragingMode(DimensionChannelLaserScanAveragingMode* averagingMode) = 0;
		virtual bool TryGetLaserAttenuatorState(DimensionChannelLaserScanLaserAttenuatorState* attenuatorState) = 0;

		virtual ~IDimensionChannelLaserScanInfo() = default;
	};

	enum class DimensionChannelSPIMIlluminationSettingsBeamType : std::uint8_t
	{
		Gauss,
		Bessel,
		Other
	};

	class IInstrumentObjectiveSettings;
	class ISPIMIlluminationResonanceScannerSettings;
	class ISPIMIlluminationGalvoScannerSettings;

	class IDimensionChannelSPIMIlluminationSettings
	{
	public:
		virtual bool TryGetNALightSheet(double* naLightSheet) = 0;
		virtual bool TryGetBeamType(DimensionChannelSPIMIlluminationSettingsBeamType* beamType) = 0;
		virtual bool TryGetCylinderLensRef(std::wstring* cylinderLensRef) = 0;
		virtual std::shared_ptr<IInstrumentObjectiveSettings> GetIlluminationObjectiveSettings() = 0;
		virtual std::shared_ptr<ISPIMIlluminationResonanceScannerSettings> GetResonanceScannerSettings() = 0;
		virtual std::shared_ptr<ISPIMIlluminationGalvoScannerSettings> GetGalvoScannerSettings() = 0;

		virtual ~IDimensionChannelSPIMIlluminationSettings() = default;
	};

	class IInstrumentObjectiveSettings
	{
	public:
		enum class Medium : std::uint8_t
		{
			Air,
			Oil,
			Water,
			Glycerol,
			Other
		};
	public:
		virtual bool TryGetObjectiveRef(std::wstring* objectiveRef) = 0;
		virtual bool TryGetMedium(Medium* medium) = 0;
		virtual bool TryGetCorrectionCollar(double* correctioncollar) = 0;
		virtual bool TryGetCorrectionCollar2(double* correctioncollar2) = 0;
		virtual bool TryGetCorrectionControllerMode(std::wstring* correctioncontrollermode) = 0;
		virtual bool TryGetCorrectionControllerBottomThickness(double* correctioncontrollerbottomthickness) = 0;
		virtual bool TryGetCorrectionControllerImagingDepth(double* correctioncontrollerimagingdepth) = 0;
		virtual bool TryGetRefractiveIndex(double* refractiveindex) = 0;

		virtual ~IInstrumentObjectiveSettings() = default;
	};

	class ISPIMIlluminationResonanceScannerSettings
	{
	public:
		virtual bool TryGetAmplitude(double* amplitude) = 0;

		virtual ~ISPIMIlluminationResonanceScannerSettings() = default;
	};

	class ISPIMIlluminationGalvoScannerSettings
	{
	public:
		virtual bool TryGetAmplitudeYScan(double* amplitudeYScan) = 0;
		virtual bool TryGetFrequency(double* frequency) = 0;
		virtual bool TryGetOffset1Left(double* offset1Left) = 0;
		virtual bool TryGetOffset1Right(double* offset1Right) = 0;
		virtual bool TryGetOffset2Left(double* offset2Left) = 0;
		virtual bool TryGetOffset2Right(double* offset2Right) = 0;

		virtual ~ISPIMIlluminationGalvoScannerSettings() = default;
	};

	class IDimensionChannelSPIMDetectionSettings
	{
	public:
		virtual bool TryGetNADetection(double* naDetection) = 0;

		virtual ~IDimensionChannelSPIMDetectionSettings() = default;
	};

	class IDimensionChannelSIMSettings
	{
	public:
		virtual bool TryGetSIMDeviceRef(std::wstring* simDeviceRef) = 0;
		virtual bool TryGetGratingPeriod(double* gratingPeriod) = 0;
		virtual bool TryGetGratingRotation(double* gratingRotation) = 0;
		virtual bool TryGetGridDescription(std::wstring* gridDescription) = 0;
		virtual bool TryGetReflector(std::wstring* reflector) = 0;
		virtual bool TryGetFilterSetRef(std::wstring* filterSetRef) = 0;
		virtual bool TryGetDiffractionEfficiency1(double* diffractionEfficiency1) = 0;
		virtual bool TryGetDiffractionEfficiency2(double* diffractionEfficiency2) = 0;
		virtual bool TryGetDiffractionEfficiency3(double* diffractionEfficiency3) = 0;

		virtual ~IDimensionChannelSIMSettings() = default;
	};

	class IDimensionChannelPolarizingSettings
	{
	public:
		virtual bool TryGetPolarizerAngle(double* polarizerAngle) = 0;
		virtual bool TryGetAnalyzerAngle(double* analyzerAngle) = 0;

		virtual ~IDimensionChannelPolarizingSettings() = default;
	};

	class IFastAiryScanSettings
	{
	public:
		struct Vector
		{
			double x, y;
		};
	public:
		virtual bool TryGetIlluminationLinePixels(Vector* illuminationLinePixels) = 0;
		virtual bool TryGetIlluminationLine(Vector* illuminationLine) = 0;
		virtual bool TryGetFiberMapping(std::vector<int>* mapping) = 0;
		virtual bool TryGetEffectiveNAinX(double* effectiveNAinX) = 0;
		virtual bool TryGetEffectiveNAinY(double* effectiveNAinY) = 0;
		
		virtual ~IFastAiryScanSettings() = default;
	};

	class IDimensionChannelAiryscanSettings
	{
	public:
		enum class Mode :std::uint8_t
		{
			SuperResolution,
			VirtualPinhole,
			MultiBeam,
			Off
		};
	public:
		virtual bool TryGetMode(Mode* mode) = 0;
		virtual std::shared_ptr<IFastAiryScanSettings> GetFastAiryScanSettings() = 0;
		virtual bool TryGetVirtualPinholeSize(double* virtualPinholeSize) = 0;
		virtual bool TryGetMagnification(double* magnification) = 0;
		virtual bool TryGetTransformationXX(double* transformationXX) = 0;
		virtual bool TryGetTransformationXY(double* transformationXY) = 0;
		virtual bool TryGetTransformationYX(double* transformationYX) = 0;
		virtual bool TryGetTransformationYY(double* transformationYY) = 0;

		virtual ~IDimensionChannelAiryscanSettings() = default;
	};

	class IDimensionChannelRatio
	{
	public:
		/// The type of an online calculation where the channel is the destination.
		///
		///		The following values are possible :
		///
		///		NoOnlineCalculation - The data channel receives raw scan data.
		///			OnlineRatio - (S1 + C1) / (S2 + C2) * C3 + C4
		///			OnlineSubtraction - (S1 * C1 - S2 * C3) / C2 + C4
		///			OnlineSubtractionRatio - (S1 - S2 * C1) / (S1 + S2 * C2) * C3 + C4
		///			OnlineHillFunction - C3 * (C4 / C5) * (S1 - C1) / (C2 - S2)
		///			OnlineReferenceRatio - (S1 - S2 + C1) / (S2 - C2) * C3 + C4
		///			OnlineLinearUnmixing - The Linear Unmixing processing.
		///
		///			The operands and constants are :
		///
		///		S1 - Source1
		///			S2 - Source2
		///			C1 - RatioConstant1
		///			C2 - RatioConstant2
		///			C3 - RatioConstant3
		///			C4 - RatioConstant4
		///			C5 - RatioConstant5
		enum class Type : std::uint8_t
		{
			NoOnlineCalculation,
			OnlineRatio,
			OnlineSubtraction,
			OnlineSubtractionRatio,
			OnlineHillFunction,
			OnlineReferenceRatio,
			OnlineLinearUnmixing
		};
	public:
		virtual bool TryGetType(Type* type) = 0;
		virtual bool TryGetConst1(double* c1) = 0;
		virtual bool TryGetConst2(double* c2) = 0;
		virtual bool TryGetConst3(double* c3) = 0;
		virtual bool TryGetConst4(double* c4) = 0;
		virtual bool TryGetConst5(double* c5) = 0;
		virtual bool TryGetConst6(double* c6) = 0;

		/// Specifies the source operands S1 for an online calculation. The vector contains
		/// references to channels (ChannelRefs).
		/// \param [in,out] source1 If non-null and the property is available, it will be put here.
		/// \returns True if it succeeds, false otherwise.
		virtual bool TryGetSource1(std::vector<std::wstring>* source1) = 0;

		/// Specifies the source operands S2 for an online calculation. The vector contains
		/// references to channels (ChannelRefs).
		/// \param [in,out] source1 If non-null and the property is available, it will be put here.
		/// \returns True if it succeeds, false otherwise.
		virtual bool TryGetSource2(std::vector<std::wstring>* source2) = 0;

		virtual ~IDimensionChannelRatio() = default;
	};
}