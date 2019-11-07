package io.github.igagis.mordavokne;

import android.app.NativeActivity;
import android.app.Service;
import android.content.Context;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

import java.io.IOException;


public class MordaVOkneActivity extends NativeActivity {
	{
		System.loadLibrary("c++_shared");
		System.loadLibrary("mordavokneapp");
	}

	public static final String LOGTAG = "mordavokne java side";

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
	
	public float getDotsPerInch(){
		Display d = ((WindowManager)getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
		DisplayMetrics m = new DisplayMetrics();
		d.getMetrics(m);
		Log.d(LOGTAG, "getDotsPerInch(): xdpi = " + m.xdpi + " ydpi = " + m.ydpi + " density = " + m.density);
		return (m.xdpi + m.ydpi) / 2.0f;
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

	//Override the Back button handler to prevent activity from automatically closing by Back key.
	@Override
	public void onBackPressed() {
	}
	
	private InputMethodManager imm;
	
	@Override
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);

		this.imm = (InputMethodManager)this.getSystemService(Service.INPUT_METHOD_SERVICE);
	}
	
	
	public void showVirtualKeyboard(){
		this.imm.showSoftInput(this.getWindow().getDecorView(), InputMethodManager.SHOW_FORCED);
		Rect r = new Rect();
		this.getWindow().getDecorView().getWindowVisibleDisplayFrame(r);
		Log.d(LOGTAG, "showVirtualKeyboard(): visible rect = " + r.left + ", " + r.right + ", " + r.top + ", " + r.bottom);
	}
	
	public void hideVirtualKeyboard(){
		this.imm.hideSoftInputFromWindow(this.getWindow().getDecorView().getWindowToken(), InputMethodManager.HIDE_IMPLICIT_ONLY);
	}

	public String[] listDirContents(String path){
		try {
			String[] list = getAssets().list(path);

			for(int i = 0; i != list.length; ++i){
				if(getAssets().list(path + "/" + list[i]).length > 0){
					//this is a directory
					list[i] += "/";
				}
			}

//			Log.d(LOGTAG, "file list = " + list);
			return list;
		}catch (IOException e){
			Log.d(LOGTAG, e.getMessage());
		}
		return null;
	}
	
	public String getStorageDir(){
		return this.getFilesDir().getPath();
	}
}
