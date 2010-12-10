//=============================================================================
//
// file :        ScientaAcquisition.h
//
// description : Include for the ScientaAcquisition class.
//
// project :	null
//
// $Author: abeilleg $
//
// $Revision: 1.12 $
//
// $Log: ScientaAcquisition.h,v $
// Revision 1.12  2010/03/30 16:10:40  abeilleg
// merge with NO MFC
//
// Revision 1.11.2.3  2010/03/30 15:08:51  abeilleg
// merge with main branch
//
// Revision 1.11.2.2  2010/03/23 11:28:35  buteau
// - relecture de code
// modifs mineures, quelques leaks potentiels corrigés
//
// Revision 1.11.2.1  2010/03/17 08:57:15  abeilleg
// new version with new gammadata wrapper
//
//
// copyleft :    European Synchrotron Radiation Facility
//               BP 220, Grenoble 38043
//               FRANCE

#ifndef _SCIENTAACQUISITION_H
#define _SCIENTAACQUISITION_H

#include <wseswrappermain.h>

 //	Add your own constants definitions here.
 //-----------------------------------------------


namespace ScientaAcquisition_ns
{

/**
 * Class Description:
 * 
 */

/*
 *	Device States Description:
*  Tango::RUNNING :  An acquisition is currently running.
*  Tango::UNKNOWN :  Unknown state of the equipement.
*  Tango::FAULT :    The previous acquisition has failed.
*  Tango::STANDBY :  Everything is OK.
 */


class ScientaAcquisition: public Tango::Device_3Impl
{
public :
	//	Add your own data members here
	//-----------------------------------------

/**
 *	The ses root directory (where ses.exe is installed)
 */
	string	sesRootDirectory;
/**
 *	The name of the instrument file (eg:D:\\dev\\SES-1.2.5-rc6\\data\\Instrument.dat)
 */
	string	instrumentFilePath;

/**@name Constructors
 * Miscellaneous constructors */
//@{
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device Name
 */
	ScientaAcquisition(Tango::DeviceClass *cl,string &s);
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device Name
 */
	ScientaAcquisition(Tango::DeviceClass *cl,const char *s);
/**
 * Constructs a newly allocated Command object.
 *
 *	@param cl	Class.
 *	@param s 	Device name
 *	@param d	Device description.
 */
	ScientaAcquisition(Tango::DeviceClass *cl,const char *s,const char *d);
//@}

/**@name Destructor
 * Only one desctructor is defined for this class */
//@{
/**
 * The object desctructor.
 */	
	~ScientaAcquisition() {delete_device();};
/**
 *	will be called at device destruction or at init command.
 */
	void delete_device();
//@}

	
/**@name Miscellaneous methods */
//@{
/**
 *	Initialize the device
 */
	virtual void init_device();
/**
 *	Always executed method befor execution command method.
 */
	virtual void always_executed_hook();

//@}

/**
 * @name ScientaAcquisition methods prototypes
 */

//@{
/**
 *	Hardware acquisition for attributes.
 */
	virtual void read_attr_hardware(vector<long> &attr_list);
/**
 *	Extract real attribute values for excitationEnergy acquisition result.
 */
	virtual void read_excitationEnergy(Tango::Attribute &attr);
/**
 *	Write excitationEnergy attribute values to hardware.
 */
	virtual void write_excitationEnergy(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for mode acquisition result.
 */
	virtual void read_mode(Tango::Attribute &attr);
/**
 *	Write mode attribute values to hardware.
 */
	virtual void write_mode(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for energyScale acquisition result.
 */
	virtual void read_energyScale(Tango::Attribute &attr);
/**
 *	Write energyScale attribute values to hardware.
 */
	virtual void write_energyScale(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for highEnergy acquisition result.
 */
	virtual void read_highEnergy(Tango::Attribute &attr);
/**
 *	Write highEnergy attribute values to hardware.
 */
	virtual void write_highEnergy(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for lowEnergy acquisition result.
 */
	virtual void read_lowEnergy(Tango::Attribute &attr);
/**
 *	Write lowEnergy attribute values to hardware.
 */
	virtual void write_lowEnergy(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for fixEnergy acquisition result.
 */
	virtual void read_fixEnergy(Tango::Attribute &attr);
/**
 *	Write fixEnergy attribute values to hardware.
 */
	virtual void write_fixEnergy(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for energyStep acquisition result.
 */
	virtual void read_energyStep(Tango::Attribute &attr);
/**
 *	Write energyStep attribute values to hardware.
 */
	virtual void write_energyStep(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for stepTime acquisition result.
 */
	virtual void read_stepTime(Tango::Attribute &attr);
/**
 *	Write stepTime attribute values to hardware.
 */
	virtual void write_stepTime(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for lensMode acquisition result.
 */
	virtual void read_lensMode(Tango::Attribute &attr);
/**
 *	Write lensMode attribute values to hardware.
 */
	virtual void write_lensMode(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for passEnergy acquisition result.
 */
	virtual void read_passEnergy(Tango::Attribute &attr);
/**
 *	Write passEnergy attribute values to hardware.
 */
	virtual void write_passEnergy(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for passMode acquisition result.
 */
	virtual void read_passMode(Tango::Attribute &attr);
/**
 *	Write passMode attribute values to hardware.
 */
	virtual void write_passMode(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for detectorFirstXChannel acquisition result.
 */
	virtual void read_detectorFirstXChannel(Tango::Attribute &attr);
/**
 *	Write detectorFirstXChannel attribute values to hardware.
 */
	virtual void write_detectorFirstXChannel(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for detectorLastXChannel acquisition result.
 */
	virtual void read_detectorLastXChannel(Tango::Attribute &attr);
/**
 *	Write detectorLastXChannel attribute values to hardware.
 */
	virtual void write_detectorLastXChannel(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for detectorFirstYChannel acquisition result.
 */
	virtual void read_detectorFirstYChannel(Tango::Attribute &attr);
/**
 *	Write detectorFirstYChannel attribute values to hardware.
 */
	virtual void write_detectorFirstYChannel(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for detectorLastYChannel acquisition result.
 */
	virtual void read_detectorLastYChannel(Tango::Attribute &attr);
/**
 *	Write detectorLastYChannel attribute values to hardware.
 */
	virtual void write_detectorLastYChannel(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for detectorSlices acquisition result.
 */
	virtual void read_detectorSlices(Tango::Attribute &attr);
/**
 *	Write detectorSlices attribute values to hardware.
 */
	virtual void write_detectorSlices(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for ADCMode acquisition result.
 */
	virtual void read_ADCMode(Tango::Attribute &attr);
/**
 *	Write ADCMode attribute values to hardware.
 */
	virtual void write_ADCMode(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for ADCMask acquisition result.
 */
	virtual void read_ADCMask(Tango::Attribute &attr);
/**
 *	Write ADCMask attribute values to hardware.
 */
	virtual void write_ADCMask(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for discriminatorLevel acquisition result.
 */
	virtual void read_discriminatorLevel(Tango::Attribute &attr);
/**
 *	Write discriminatorLevel attribute values to hardware.
 */
	virtual void write_discriminatorLevel(Tango::WAttribute &attr);
/**
 *	Extract real attribute values for channels acquisition result.
 */
	virtual void read_channels(Tango::Attribute &attr);
/**
 *	Extract real attribute values for slices acquisition result.
 */
	virtual void read_slices(Tango::Attribute &attr);
/**
 *	Extract real attribute values for sweeps acquisition result.
 */
	virtual void read_sweeps(Tango::Attribute &attr);
/**
 *	Extract real attribute values for channelScale acquisition result.
 */
	virtual void read_channelScale(Tango::Attribute &attr);
/**
 *	Extract real attribute values for sliceScale acquisition result.
 */
	virtual void read_sliceScale(Tango::Attribute &attr);
/**
 *	Extract real attribute values for sumData acquisition result.
 */
	virtual void read_sumData(Tango::Attribute &attr);
/**
 *	Extract real attribute values for data acquisition result.
 */
	virtual void read_data(Tango::Attribute &attr);
/**
 *	Read/Write allowed for excitationEnergy attribute.
 */
	virtual bool is_excitationEnergy_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for mode attribute.
 */
	virtual bool is_mode_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for energyScale attribute.
 */
	virtual bool is_energyScale_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for highEnergy attribute.
 */
	virtual bool is_highEnergy_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for lowEnergy attribute.
 */
	virtual bool is_lowEnergy_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for fixEnergy attribute.
 */
	virtual bool is_fixEnergy_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for energyStep attribute.
 */
	virtual bool is_energyStep_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for stepTime attribute.
 */
	virtual bool is_stepTime_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for lensMode attribute.
 */
	virtual bool is_lensMode_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for passEnergy attribute.
 */
	virtual bool is_passEnergy_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for passMode attribute.
 */
	virtual bool is_passMode_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for detectorFirstXChannel attribute.
 */
	virtual bool is_detectorFirstXChannel_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for detectorLastXChannel attribute.
 */
	virtual bool is_detectorLastXChannel_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for detectorFirstYChannel attribute.
 */
	virtual bool is_detectorFirstYChannel_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for detectorLastYChannel attribute.
 */
	virtual bool is_detectorLastYChannel_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for detectorSlices attribute.
 */
	virtual bool is_detectorSlices_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for ADCMode attribute.
 */
	virtual bool is_ADCMode_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for ADCMask attribute.
 */
	virtual bool is_ADCMask_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for discriminatorLevel attribute.
 */
	virtual bool is_discriminatorLevel_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for channels attribute.
 */
	virtual bool is_channels_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for slices attribute.
 */
	virtual bool is_slices_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for sweeps attribute.
 */
	virtual bool is_sweeps_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for channelScale attribute.
 */
	virtual bool is_channelScale_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for sliceScale attribute.
 */
	virtual bool is_sliceScale_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for sumData attribute.
 */
	virtual bool is_sumData_allowed(Tango::AttReqType type);
/**
 *	Read/Write allowed for data attribute.
 */
	virtual bool is_data_allowed(Tango::AttReqType type);
/**
 *	Execution allowed for Start command.
 */
	virtual bool is_Start_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for Stop command.
 */
	virtual bool is_Stop_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for ResetInstrument command.
 */
	virtual bool is_ResetInstrument_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for ResetSupplies command.
 */
	virtual bool is_ResetSupplies_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for TestCommunication command.
 */
	virtual bool is_TestCommunication_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for GetHardwareInfo command.
 */
	virtual bool is_GetHardwareInfo_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for GetLensModeList command.
 */
	virtual bool is_GetLensModeList_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for GetPassEnergyList command.
 */
	virtual bool is_GetPassEnergyList_allowed(const CORBA::Any &any);
/**
 *	Execution allowed for GetPassModeList command.
 */
	virtual bool is_GetPassModeList_allowed(const CORBA::Any &any);
/**
 * Start the acqusition for one region with parameters configured in the attributes.
 *	@exception DevFailed
 */
	void	start();
/**
 * Stop the current acquisition.
 *	@exception DevFailed
 */
	void	stop();
/**
 * Reset instrument.
 *	@exception DevFailed
 */
	void	reset_instrument();
/**
 * Reset supplies.
 *	@exception DevFailed
 */
	void	reset_supplies();
/**
 * Test communication with hardware.
 *	@exception DevFailed
 */
	void	test_communication();
/**
 * Retrieve hardware informations.
 *	@return	A decription of the hardware
 *	@exception DevFailed
 */
	Tango::DevString	get_hardware_info();
/**
 * 
 *	@return	The list of possible lens modes
 *	@exception DevFailed
 */
	Tango::DevVarStringArray	*get_lens_mode_list();
/**
 * 
 *	@return	The list of possible pass energies
 *	@exception DevFailed
 */
	Tango::DevVarDoubleArray	*get_pass_energy_list();
/**
 * 
 *	@return	The possible values for Pass Modes
 *	@exception DevFailed
 */
	Tango::DevVarStringArray	*get_pass_mode_list();

/**
 *	Read the device properties from database
 */
	 void get_device_property();
//@}

	//	Here is the end of the automatic code generation part
	//-------------------------------------------------------------	



protected :	
	//	Add your own data members here
	//-----------------------------------------
	WSESWrapperMain* ses;
	SESWrapperNS::WAnalyzerRegion analyzer;
	SESWrapperNS::WDetectorRegion detector;
	SESWrapperNS::WDetectorInfo info;
};

}	// namespace_ns

#endif	// _SCIENTAACQUISITION_H
