/* eslint-disable prettier/prettier */
import { NativeModules, Platform } from 'react-native';


const LINKING_ERROR =
  "The package 'react-native-pixi' doesn't seem to be linked. Make sure: \n\n" +
  Platform.select({ ios: "- You have run 'pod install'\n", default: '' }) +
  '- You rebuilt the app after installing the package\n' +
  '- You are not using Expo Go\n';



const PixiModule = NativeModules.PixiModule
  ? NativeModules.PixiModule
  : new Proxy(
    {},
    {
      get() {
        throw new Error(LINKING_ERROR);
      },
    },
  );



export default PixiModule;
