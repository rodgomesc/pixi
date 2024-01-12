package com.example.pixi;

import android.content.res.AssetManager;
import android.util.Log;

import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = PixiModule.NAME)
public class PixiModule extends ReactContextBaseJavaModule {
    public static final String NAME = "Pixi";

    public PixiModule(ReactApplicationContext reactContext) {
        super(reactContext);
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean install() {
        final String LIBRARY_NAME = "Pixi";
        try {
            System.loadLibrary(LIBRARY_NAME);
            JavaScriptContextHolder jsContext = getReactApplicationContext().getJavaScriptContextHolder();
            boolean res = installJsiRuntime(jsContext.get());
            return res;
        } catch (Error e) {
            throw new UnsatisfiedLinkError("Failed to load native library: " + LIBRARY_NAME);
        }
    }

    private static native boolean installJsiRuntime(long jsiPtr);

    @Override
    public String getName() {
        return NAME;
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean testImageResizing() {
        long jsContext = getReactApplicationContext().getJavaScriptContextHolder().get();
        AssetManager assetManager = getReactApplicationContext().getAssets();
        try {
            boolean successful = nativeTestImageResizing(jsContext, assetManager);
        } catch (Exception exception) {
            Log.e(NAME, "Failed to call nativeTestImageResizing!", exception);
            return false;
        }
        return true;
    }


    private native boolean nativeTestImageResizing(long jsiPtr, AssetManager assetManager);
}
