package com.googlecode.morda_tests;

import com.googlecode.morda.MordaActivity;

public class TestActivity extends MordaActivity{
	{
		System.loadLibrary("gnustl_shared");
		System.loadLibrary("tests");
	}
}
