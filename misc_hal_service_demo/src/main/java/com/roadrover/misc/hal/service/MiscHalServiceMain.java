package main.java.com.roadrover.misc.hal.service;

import android.app.Activity;
import android.os.Bundle;

public class MiscHalServiceMain extends Activity
{
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		MiscHalService miscHalService = MiscHalService.getInstance();
		/* init must be use. */
		miscHalService.init();
		/* get all of 32bit data. */
		miscHalService.GetBootConfig();
		/* set all of 32bit data (actually data is 32bit -- unisgned int type). */
		miscHalService.SetBootConfig(long data);
		/* lights is range 0 ~ 100 */
		miscHalService.SetBootConfigCamera(long data);
		/* design for 360 camera flag. */
		miscHalService.SetBootConfigLight(long data);
	}  
}   