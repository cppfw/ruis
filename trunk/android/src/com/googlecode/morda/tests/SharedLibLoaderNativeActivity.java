package com.googlecode.morda.tests;

import android.app.NativeActivity;
import android.app.Service;
import android.os.Bundle;
import android.view.inputmethod.InputMethodManager;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.util.Log;


public class SharedLibLoaderNativeActivity extends NativeActivity {
	public static final String LOGTAG = "morda java side";

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
			}catch(Exception e){
				Log.e(LOGTAG, "resolveKeyUnicode(): Could not load KeyCharacterMap");
				return 0;//could not load char map, thus could not resolve character
			}
		}
		
		return this.curCharMap.get(keyCode, metaState);
	}
	
	private static native void handleCharacterStringInput(String str);
	
	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		Log.d(LOGTAG, "dispatchKeyEvent(): invoked");
		if(event.getAction() == KeyEvent.ACTION_MULTIPLE && event.getKeyCode() == KeyEvent.KEYCODE_UNKNOWN){
			Log.d(LOGTAG, "dispatchKeyEvent(): multiple characters are being delivered, chars = " + event.getCharacters());
			//Multiple characters are being delivered. NativeActivity currently does not
			//support passing multiple characters from KeyEvent to native code, so we need to do it here.
			handleCharacterStringInput(event.getCharacters());
			return true;
		}
		return super.dispatchKeyEvent(event);
	}
	
	
	private InputMethodManager imm;
	
	@Override
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		
		this.imm = (InputMethodManager)this.getSystemService(Service.INPUT_METHOD_SERVICE);
	}
	
	
	public void showVirtualKeyboard(){
		imm.showSoftInput(this.getWindow().getDecorView(), InputMethodManager.SHOW_FORCED);
	}
	
	public void hideVirtualKeyboard(){
		imm.hideSoftInputFromWindow(this.getWindow().getDecorView().getWindowToken(), InputMethodManager.HIDE_IMPLICIT_ONLY);
	}
}
