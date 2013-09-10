package com.googlecode.morda_tests;

import android.app.Activity;
import android.os.Bundle;

import com.googlecode.morda.MordaActivity;

public class TestActivity extends MordaActivity{

	{
		System.loadLibrary("gnustl_shared");
		System.loadLibrary("tests");
	}


}
