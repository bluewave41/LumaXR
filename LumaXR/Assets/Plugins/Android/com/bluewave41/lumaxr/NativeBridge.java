package com.bluewave41.lumaxr;

public final class NativeBridge {
    static {
        System.loadLibrary("gstplugin");
    }

    public static native void setJavaVM();
}
