import React from 'react';
import {NativeModules, Pressable} from 'react-native';

import {SafeAreaView, StatusBar, StyleSheet, Text, View} from 'react-native';

const {Pixi} = NativeModules;

if (!Pixi) {
  throw new Error('Pixi native module is not available.');
}

function App(): JSX.Element {
  const handleImageResize = () => {
    console.log('resizing...');
    Pixi.install();
  };
  return (
    <SafeAreaView style={styles.container}>
      <Pressable onPress={handleImageResize}>
        <Text>Resize</Text>
      </Pressable>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
});

export default App;
