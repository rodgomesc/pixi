import { Frame } from 'react-native-vision-camera';
import PixiModule from '../PixiModule';

const result = PixiModule.install() as boolean;
if (result !== true) {
  console.error('Failed to install pixi bindings!');
}

interface convertOptions {
  output_width: number;
  output_height: number;
  output_format: 'rgba-8888';
}

export interface PixiHostFunctions {
  convert(frame: Frame, options: convertOptions): Record<string, unknown>;
  sayHello(n: number): void;
}

declare global {
  var __PixiHostFn: PixiHostFunctions;
}

const pixi = global.__PixiHostFn;

export default pixi;
