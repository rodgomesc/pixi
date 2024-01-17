package com.example.pixi

import android.util.Log
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactContextBaseJavaModule
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.module.annotations.ReactModule

@ReactModule(name = PixiModule.TAG)
class PixiModule(reactContext: ReactApplicationContext?) :
    ReactContextBaseJavaModule(reactContext) {

    companion object {
        const val TAG = "PixiModule"
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    fun install(): Boolean =
        try {
            val proxy = PixiProxy(reactApplicationContext)
            PixiInstaller.installHostObject(proxy)
            true
        } catch (e: Error) {
            Log.e(TAG, e.message.toString())
            false
        }


    override fun getName(): String = TAG

}
