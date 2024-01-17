import React, {useEffect, useRef, useState} from 'react';

import {
  Camera,
  useCameraDevice,
  CameraPermissionStatus,
  useCameraFormat,
  useFrameProcessor,
} from 'react-native-vision-camera';

// import pixiFrameProcessor from './frame-processors/pixiFrameProcessor';

import pixi from './frame-processors/pixiProcessorNative';
import {StyleSheet} from 'react-native';

const CameraPage = (): React.ReactElement | null => {
  const [cameraPermission, setCameraPermission] =
    useState<CameraPermissionStatus>();

  const camera = useRef<Camera>(null);

  const device = useCameraDevice('front');
  const targetFps = 30;

  const format = useCameraFormat(device, [
    {fps: targetFps},
    {videoResolution: 'max'},
  ]);

  useEffect(() => {
    (async () => {
      const status = await Camera.requestCameraPermission();
      setCameraPermission(status);
    })();
  }, []);

  useEffect(() => {
    pixi.sayHello(777);
  }, []);

  const frameProcessor = useFrameProcessor(frame => {
    'worklet';

    const result = pixi.convert(frame, {
      output_height: 100,
      output_width: 100,
      output_format: 'rgba-8888',
    });
    console.log(result);
  }, []);

  if (!cameraPermission || !device) {
    return null;
  }

  return (
    <Camera
      ref={camera}
      style={StyleSheet.absoluteFill}
      isActive={true}
      format={format}
      pixelFormat="yuv"
      frameProcessor={frameProcessor}
      device={device}
    />
  );
};

export default CameraPage;
