package com.googlecode.morda.tests;

import android.app.NativeActivity;

public class SharedLibLoaderNativeActivity extends NativeActivity {

static {
	System.loadLibrary("gnustl_shared");
	System.loadLibrary("ting");
	System.loadLibrary("stob");
//	System.loadLibrary("morda");
	System.loadLibrary("tests");
}

}
