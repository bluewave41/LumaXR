package com.bluewave41.lumaxr;

import android.util.Log;
import android.graphics.SurfaceTexture;
import android.view.Surface;

public class UnitySurface {
    private native void nativeInit(SurfaceTexture surfaceTextre, Surface surface, int instanceId);
    private SurfaceTexture surfaceTexture;
    private Surface surface;
    private int frameCount = 0;
    private long lastTime = System.nanoTime();
    
    static {
        System.loadLibrary("gstplugin");
    }
    
    public UnitySurface(int textureId, int width, int height) {
        surfaceTexture = new SurfaceTexture(textureId);
        surfaceTexture.setDefaultBufferSize(width, height);
        surfaceTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
            @Override
            public void onFrameAvailable(SurfaceTexture st) {
                frameCount++;
                long now = System.nanoTime();
                double seconds = (now - lastTime) / 1_000_000_000.0;
                if (seconds >= 1.0) {
                    double fps = frameCount / seconds;
                    Log.i("FPS", "FPS: " + fps);
                    frameCount = 0;
                    lastTime = now;
                }
            }
        });
        surface = new Surface(surfaceTexture);
    }

    public Surface getSurface() {
        return surface;
    }

    public SurfaceTexture getSurfaceTexture() {
        return surfaceTexture;
    }

    public void init(int instanceId) {
        Log.i("LumaXR", "Calling nativeInit for instance " + instanceId);
        Log.i("LumaXR", "Surface " + surface);
        Log.i("LumaXR", "SurfaceTexture" + surfaceTexture);

        nativeInit(surfaceTexture, surface, instanceId);
    }
}
