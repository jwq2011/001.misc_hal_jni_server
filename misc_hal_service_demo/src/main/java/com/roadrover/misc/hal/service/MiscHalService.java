package main.java.com.roadrover.misc.hal.service;

public class MiscHalService
{
	private static MiscHalService miscHalService;

	public static MiscHalService getInstance()
	{
		if (miscHalService == null)
			miscHalService = new MiscHalService();

		return miscHalService;
	}

	private MiscHalService()  
	{  
		init(); 
	}
	/* init must be set. */
	public boolean init()
	{
		return _init();
	}
	/* get all of 32bit data. */
	public boolean GetBootConfig(long data)
	{
		return _getBootConfig(data);
	}
	/* set all of 32bit data. */
	public long SetBootConfig(long data)
	{
		return _setBootConfig();
	}
	/* lights is range 0 ~ 100 */
	public boolean SetBootConfigLight(long data)
	{
		return _setBootLights(data);
	}
	/* design for 360 camera flag. */
	public boolean SetBootConfigCamera(long data)
	{
		return _setBootCamera(data);
	}

	//  native method 
	private native boolean _init();
	private native boolean _setBootConfig(long data);
	private native long _getBootConfig();
	private native boolean _setBootLights(long data);
	private native boolean _setBootCamera(long data);

	static
	{
		System.load("/system/lib/misc_hal_jni.so");
	}
}
