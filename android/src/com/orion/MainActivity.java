package com.orion;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.os.*;
import android.os.PowerManager;
import android.content.*;
import android.app.*;
import android.util.Log;

public class MainActivity extends QtActivity
{
        private static MainActivity instance = null;
	private static PowerManager.WakeLock wl = null;

	/**Native C++ method calls*/

        /**Singleton getter*/
         public static MainActivity getInstance() {
             if (instance == null)
                 instance = new MainActivity();
             return instance;
         }

	/**Activity callbacks*/
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
                logMsg("Created MainActivity!");
                instance = this;
	}

	/**Wakelock methods -- not in use --*/
	public static void acquireWakeLock() {
		if (wl == null) {
			PowerManager pm = (PowerManager) (getInstance().getSystemService(Context.POWER_SERVICE));
			// assert(pm.isWakeLockLevelSupported(PowerManager.PARTIAL_WAKE_LOCK));
                        wl = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, "OrionWakeLock");
			wl.acquire();

			if (wl.isHeld())
                                logMsg("acquired wakelock!");
			else
                                logMsg("failed to acquire wakelock!");
		}
	}
	public static void releaseWakeLock() {
		if (wl != null) {
			wl.release();
			wl = null;
                        logMsg("released wakelock");
		}
	}

	/**Logger*/
	public static void logMsg(String msg)  {
		Log.w("LOG", msg);
	}
}
