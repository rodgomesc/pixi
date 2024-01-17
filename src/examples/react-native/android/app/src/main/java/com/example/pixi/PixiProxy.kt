package com.example.pixi

import android.graphics.Bitmap
import android.util.Log
import androidx.annotation.Keep
import com.facebook.jni.HybridData
import com.facebook.proguard.annotations.DoNotStrip
import com.facebook.react.bridge.ReactApplicationContext
import java.lang.ref.WeakReference
import java.nio.ByteBuffer

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
        Log.d(TAG, "Hello from PixiProxy! $num")
    }

    @DoNotStrip
    @Keep
    fun receiveBuffer(buffer: ByteBuffer, width: Int, height: Int) {
        buffer.rewind()

        val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        bitmap.copyPixelsFromBuffer(buffer)

        Log.d(TAG, "Received buffer with width: $width and height: $height")
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