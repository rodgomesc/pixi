package com.example.pixi

import android.util.Log
import androidx.annotation.Keep
import com.facebook.jni.HybridData
import com.facebook.proguard.annotations.DoNotStrip
import com.facebook.react.bridge.ReactApplicationContext
import java.lang.ref.WeakReference

@Suppress("KotlinJniMissingFunction")
class PixiProxy(context: ReactApplicationContext) {
    companion object {
        const val TAG = "PixiProxy"
        private const val LIBRARY_NAME = "Pixi"

        init {
            try {
                System.loadLibrary(LIBRARY_NAME)
            } catch (e: UnsatisfiedLinkError) {
                throw UnsatisfiedLinkError("Failed to load native library: $LIBRARY_NAME")
            }
        }
    }

    @DoNotStrip
    @Keep
    fun sayHello(num: Int) {
        Log.d("TEST", "Hello from PixiProxy! $num")
    }


    @DoNotStrip
    @Keep
    private var mHybridData: HybridData
    private var mContext: WeakReference<ReactApplicationContext>

    init {
        val jsRuntimeHolder =
            context.javaScriptContextHolder?.get() ?: throw Error("JSI Runtime is null!")
        mContext = WeakReference(context)
        mHybridData = initHybrid(jsRuntimeHolder)
    }

    private external fun initHybrid(jsContext: Long): HybridData
}