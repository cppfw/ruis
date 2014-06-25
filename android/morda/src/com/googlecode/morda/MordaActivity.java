package com.googlecode.morda;

import android.app.NativeActivity;
import android.app.Service;
import android.os.Bundle;
import android.view.inputmethod.InputMethodManager;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;


public class MordaActivity extends NativeActivity {
	public static final String LOGTAG = "morda java side";

	private int curKeyDevice;
	private KeyCharacterMap curCharMap;

	public int resolveKeyUnicode(int deviceID, int metaState, int keyCode){
		Log.d(LOGTAG, "resolveKeyUnicode(): invoked, keyCode = " + keyCode);
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
	
	public float getDotsPerCm(){
		Display d = ((WindowManager)getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
		DisplayMetrics m = new DisplayMetrics();
		d.getMetrics(m);
		return ((m.xdpi + m.ydpi) / 2.0f) / 2.54f;
	}
	
	private static native void handleCharacterStringInput(String str);
	
	@Override
	public boolean dispatchKeyEvent(KeyEvent event) {
		Log.d(LOGTAG, "dispatchKeyEvent(): invoked");
		if(event.getAction() == KeyEvent.ACTION_MULTIPLE && event.getKeyCode() == KeyEvent.KEYCODE_UNKNOWN){
			Log.d(LOGTAG, "dispatchKeyEvent(): multiple characters are being delivered, chars = " + event.getCharacters());
			//Multiple characters are being delivered. NativeActivity currently does not
			//support passing multiple characters from KeyEvent to native code, so we need to do it here.
			this.handleCharacterStringInput(event.getCharacters());
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
		this.imm.showSoftInput(this.getWindow().getDecorView(), InputMethodManager.SHOW_FORCED);
	}
	
	public void hideVirtualKeyboard(){
		this.imm.hideSoftInputFromWindow(this.getWindow().getDecorView().getWindowToken(), InputMethodManager.HIDE_IMPLICIT_ONLY);
	}
}
