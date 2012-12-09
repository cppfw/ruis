package com.googlecode.morda.tests;

import android.app.NativeActivity;
import android.view.KeyCharacterMap;

public class SharedLibLoaderNativeActivity extends NativeActivity {

static {
	System.loadLibrary("gnustl_shared");
	System.loadLibrary("ting");
	System.loadLibrary("stob");
//	System.loadLibrary("morda");
	System.loadLibrary("tests");
}


	private int curKeyDevice;
	private KeyCharacterMap curCharMap;

	public int resolveKeyUnicode(int deviceID, int keyCode, int metaState){
		if(this.curCharMap == null || curKeyDevice != deviceID){
			try{
				this.curCharMap = KeyCharacterMap.load(deviceID);
				this.curKeyDevice = deviceID;
			}catch(KeyCharacterMap.UnavailableException e){
				return 0;//could not load char map, thus could not resolve character
			}
		}
		
		return this.curCharMap.get(keyCode, metaState);
	}
}
