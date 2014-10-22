#ifndef __SESWRAPPER_TYPES_H__
#define __SESWRAPPER_TYPES_H__

/*! \namespace SESWrapperNS
 * This is a namespace that wraps the global constants for SESWrapper.
 */
namespace SESWrapperNS
{
  /*! \brief Contains information about the detector.
  *
  * The SES software currently supports two-dimensional detectors. The channel/slice members are usually
  * given in pixels.
  *
  * \see WRP_GetDetectorInfo()
  */
  typedef struct WDetectorInfo
  {
    bool timerControlled_;/*!< Specifies whether the detector is controlled by a timer (\c true) or frame rate (\c false) */
    int xChannels_; /*!< Specifies the number of X channels currently shown on the detector. */
    int yChannels_; /*!< Specifies the number of Y channels (slices) currently shown on the detector. */
    int maxSlices_; /*!< Specifies the maximum number of Y channels (slices). */
    int maxChannels_; /*!< Specifies the maximum number of X channels. */
    int frameRate_; /*!< Specifies the frame rate (frames/s) */
    bool adcPresent_; /*!< Specifies whether the detector contains an ADC (adcPresent_ = \c true). */
    bool discPresent_; /*!< Specifies whether the detector contains a discriminator. */
  } DetectorInfo;

  /*! \brief Pointer to a DetectorInfo struct. */
  typedef DetectorInfo *PDetectorInfo;

  /*! \brief Contains information about the current detector region (a.k.a ROI).
  *
  * \see WRP_GetDetectorRegion(), WRP_SetDetectorRegion()
  */
  typedef struct WDetectorRegion
  {
    int firstXChannel_; /*!< Specifies the first X channel to be used on the detector (range: <code>[0...\ref WDetectorInfo::xChannels_-1]</code>. )*/
    int lastXChannel_; /*!< Specifies the last X channel to be used on the detector. */
    int firstYChannel_; /*!< Specifies the first Y channel to be used on the detector (range: <code>[0...\ref WDetectorInfo::yChannels_-1]</code>. )*/
    int lastYChannel_; /*!< Specifies the last Y channel to be used on the detector. */
    int slices_; /*!< Specifies the current number of Y channels (slices). */
    char adcMode_; /*!< Specifies whether the detector is running in ADC mode, or Pulse Counting mode */
  } DetectorRegion;

  /*! \brief Pointer to a DetectorRegion struct. */
  typedef DetectorRegion *PDetectorRegion;

  /*! \brief Defines a region to be used for data acquisition
  *
  * An analyzer region must have been set through WRP_SetAnalyzerRegion() before starting an acquisition.
  * The previously prepared region can be obtained through the WRP_CheckAnalyzerRegion() function.
  * If the \c use_binding_energy variable has been set to \c true, all energies in this struct are assumed to be given as
  * binding energies, otherwise they are kinetic energies. They default to kinetic energies. If binding energies are to be used,
  * the excitation energy must also be set before using this struct.
  *
  * \see WRP_CheckAnalyzerRegion(), WRP_SetAnalyzerRegion(), WRP_SetExcitationEnergy()
  */
  typedef struct WAnalyzerRegion
  {
    bool fixed_; /*!< Determines if the region will be measured in fixed (\c true) or swept (\c false) mode. This is a one-byte boolean. The value can be set to 1 (for \c true) or 0 (for \c false). */
    double highEnergy_; /*!< Specifies the high-end energy (eV) for swept mode acquisition. */
    double lowEnergy_; /*!< Specifies the low-end energy (eV) for swept mode acquisition. */
    double centerEnergy_; /*!< Specifies the center energy (eV) for fixed mode acquisition (the low and high end energies is calculated from this value and the current pass energy). */
    double energyStep_; /*!< Specifies the energy step size (eV) for swept mode acquisition. */
    int dwellTime_; /*!< Specifies the dwell time (ms) for fixed or swept mode acquisition. */
  } AnalyzerRegion;

  /*! \brief Pointer to a Region struct. */
  typedef AnalyzerRegion *PAnalyzerRegion;
}

#endif
