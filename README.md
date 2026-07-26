# Zain's Demon V0.2

A standalone/VST3 original dark-vocal FX prototype.

Controls:
- INPUT: input gain
- PITCH: pitch shift in semitones (-12 to 0)
- DEMON: overall effect intensity
- DRIVE: nonlinear saturation
- MIX: dry/wet
- OUTPUT: output gain

The pitch stage is an original lightweight real-time pitch shifter using two
crossfaded delay read heads. It is intended as a prototype and may produce
grain/chorus artifacts at extreme settings.

Build requirements:
- Windows 10/11
- Visual Studio 2022, Desktop development with C++
- JUCE 8.x
- CMake 3.22+

Build:
cmake -S . -B build -DJUCE_DIR="C:/path/to/JUCE"
cmake --build build --config Release

Then load the generated VST3 in FL Studio.

This project does not copy Demon Time source code or assets.
