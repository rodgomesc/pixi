import { Frame } from 'react-native-vision-camera';
import PixiProxyModule from '../PixiProxyModule';

const result = PixiProxyModule.install() as boolean;
if (result !== true) {
  console.error('Failed to install pixi bindings!');
}

interface convertOptions {
  output_width: number;
  output_height: number;
  output_format: 'rgba-8888';
}

export interface PixiProxyFunctions {
  convert(frame: Frame, options: convertOptions): Record<string, unknown>;
}

declare global {
  var __PixiProxy: () => PixiProxyFunctions;
}

const pixi = global.__PixiProxy();

export default pixi;
