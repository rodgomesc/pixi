package com.example.pixi;

import android.content.res.AssetManager;
import android.util.Log;
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

    @Override
    public String getName() {
        return NAME;
    }

    @ReactMethod(isBlockingSynchronousMethod = true)
    public boolean install() {
        try {
            Log.i(NAME, "Loading C++ library...");
            System.loadLibrary("Pixi");
            long jsContext = getReactApplicationContext().getJavaScriptContextHolder().get();
            AssetManager assetManager = getReactApplicationContext().getAssets();
            Log.i(NAME, "Installing JSI Bindings for Pixi C++ lib...");
            boolean successful = nativeInstall(jsContext, assetManager);
            if (successful) {
                Log.i(NAME, "Successfully installed JSI Bindings!");
                return true;
            } else {
                Log.e(NAME, "Failed to install JSI Bindings for Pixi C++ lib!");
                return false;
            }
        } catch (Exception exception) {
            Log.e(NAME, "Failed to install JSI Bindings!", exception);
            return false;
        }
    }

    private native boolean nativeInstall(long jsiPtr, AssetManager assetManager);
}