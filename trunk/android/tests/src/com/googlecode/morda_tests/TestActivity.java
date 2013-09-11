package com.googlecode.morda_tests;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.googlecode.morda.MordaActivity;

public class TestActivity extends MordaActivity{
	public static final String LOGTAG = "morda_tests java side";

	{
		Log.d(LOGTAG, "Loading native libraries");
		System.loadLibrary("gnustl_shared");
		System.loadLibrary("tests");
		Log.d(LOGTAG, "Native libraries loaded");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
	}
}
