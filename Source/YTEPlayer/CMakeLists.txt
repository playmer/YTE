################################################################################
## This source file is a part of YTEPlayer.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Joshua T. Fisher
################################################################################
add_executable(YTEPlayer main.cpp
                         YTEPlayer.rc)

target_include_directories(YTEPlayer 
  PRIVATE
    ${Source_Root}
    ${ParentDirectory}
    ${Dependencies_Root}/assimp/include
)

set_target_properties(YTEPlayer
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

YTE_Source_Group(YTEPlayer_Root YTEPlayer)

target_link_libraries(YTEPlayer PRIVATE YTEMain)

##release:
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkAudioInputSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkCompressorFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkExpanderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkFlangerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkGainFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkGuitarDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkHarmonizerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMatrixReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMemoryMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMeterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMotionGeneratorSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMusicEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkParametricEQFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkPeakLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkPitchShifterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkRecorderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkReflectFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkRoomVerbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSilenceSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSineSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundSeedImpactFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundSeedWindSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundSeedWooshSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSpatialAudio.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkStereoDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkStreamMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSynthOneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkTimeStretchFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkToneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkTremoloFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkVorbisDecoder.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAuroHeadphoneFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAuroPannerMixer.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libMcDSPFutzBoxFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libMcDSPLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZHybridReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashBoxModelerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashDynamicsFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashFiltersFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:PUBLISH>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashMultibandDistortionFX.a>)
#
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkAudioInputSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkCompressorFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkExpanderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkFlangerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkGainFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkGuitarDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkHarmonizerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMatrixReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMemoryMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMeterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMotionGeneratorSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkMusicEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkParametricEQFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkPeakLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkPitchShifterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkRecorderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkReflectFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkRoomVerbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSilenceSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSineSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundSeedImpactFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundSeedWindSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSoundSeedWooshSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSpatialAudio.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkStereoDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkStreamMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkSynthOneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkTimeStretchFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkToneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkTremoloFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAkVorbisDecoder.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAuroHeadphoneFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libAuroPannerMixer.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libMcDSPFutzBoxFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libMcDSPLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZHybridReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashBoxModelerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashDynamicsFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashFiltersFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELEASE>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Release/lib/libiZTrashMultibandDistortionFX.a>)
#
##profile:
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkAudioInputSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkCompressorFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkExpanderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkFlangerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkGainFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkGuitarDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkHarmonizerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMatrixReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMemoryMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMeterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMotionGeneratorSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMusicEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkParametricEQFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkPeakLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkPitchShifterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkRecorderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkReflectFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkRoomVerbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSilenceSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSineSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundSeedImpactFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundSeedWindSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundSeedWooshSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSpatialAudio.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkStereoDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkStreamMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSynthOneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkTimeStretchFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkToneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkTremoloFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkVorbisDecoder.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAuroHeadphoneFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAuroPannerMixer.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libCommunicationCentral.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libMcDSPFutzBoxFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libMcDSPLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZHybridReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashBoxModelerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashDynamicsFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashFiltersFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:MINSIZEREL>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashMultibandDistortionFX.a>)
#
#
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkAudioInputSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkCompressorFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkExpanderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkFlangerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkGainFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkGuitarDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkHarmonizerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMatrixReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMemoryMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMeterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMotionGeneratorSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkMusicEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkParametricEQFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkPeakLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkPitchShifterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkRecorderFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkReflectFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkRoomVerbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSilenceSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSineSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundEngine.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundSeedImpactFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundSeedWindSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSoundSeedWooshSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSpatialAudio.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkStereoDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkStreamMgr.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkSynthOneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkTimeStretchFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkToneSource.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkTremoloFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAkVorbisDecoder.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAuroHeadphoneFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libAuroPannerMixer.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libCommunicationCentral.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libMcDSPFutzBoxFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libMcDSPLimiterFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZHybridReverbFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashBoxModelerFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashDelayFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashDistortionFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashDynamicsFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashFiltersFX.a>)
#target_link_libraries(YTEPlayer PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Profile/lib/libiZTrashMultibandDistortionFX.a>)
#
##debug:
#
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkAudioInputSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkCompressorFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkDelayFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkExpanderFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkFlangerFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkGainFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkGuitarDistortionFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkHarmonizerFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkMatrixReverbFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkMemoryMgr.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkMeterFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkMotionGeneratorSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkMusicEngine.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkParametricEQFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkPeakLimiterFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkPitchShifterFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkRecorderFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkReflectFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkRoomVerbFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSilenceSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSineSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSoundEngine.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSoundSeedImpactFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSoundSeedWindSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSoundSeedWooshSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSpatialAudio.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkStereoDelayFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkStreamMgr.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkSynthOneSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkTimeStretchFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkToneSource.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkTremoloFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAkVorbisDecoder.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAuroHeadphoneFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libAuroPannerMixer.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libCommunicationCentral.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libMcDSPFutzBoxFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libMcDSPLimiterFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libiZHybridReverbFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libiZTrashBoxModelerFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libiZTrashDelayFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libiZTrashDistortionFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libiZTrashDynamicsFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libiZTrashFiltersFX.a)
#target_link_libraries(YTEPlayer PUBLIC debug /home/playmer/UserDependencies/Wwise2017.2.3.6575/SDK/Linux_x64/Debug/lib/libiZTrashMultibandDistortionFX.a)





















#Set the working directory and startup project. 
message("Working Dir: ${YTE_Assets_Root}/Bin")
set_target_properties(YTEPlayer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${YTE_Assets_Root}/Bin)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YTEPlayer) 

YTE_Target_Set_LTCG(YTEPlayer)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # Have to do /LTCG here because it appears as though setting this property resets it.
  #set_target_properties(YTEPlayer PROPERTIES LINK_FLAGS_PUBLISH "/SUBSYSTEM:WINDOWS /LTCG")

  target_compile_options(YTEPlayer PRIVATE 
    PUBLIC
      -permissive- -std:c++17 -MP
    PRIVATE
      -WX- -W4
  )
else()
  target_compile_options(YTEPlayer PRIVATE -std=c++17)
endif()

install(TARGETS YTEPlayer 
            RUNTIME DESTINATION BIN
            LIBRARY DESTINATION BIN
            ARCHIVE DESTINATION BIN
            COMPONENT applications)
