package com.example.pixi;

import android.view.View
import com.facebook.react.ReactPackage;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.uimanager.ReactShadowNode
import com.facebook.react.uimanager.ViewManager;


class PixiPackage : ReactPackage {
    override fun createNativeModules(reactContext: ReactApplicationContext): kotlin.collections.List<PixiModule> =
        listOf(
            PixiModule(reactContext)
        )

    override fun createViewManagers(reactContext: ReactApplicationContext): MutableList<ViewManager<View, ReactShadowNode<*>>> =
        mutableListOf()
}

